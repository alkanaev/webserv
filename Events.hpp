/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Events
 * @created     : Wednesday Mar 16, 2022 14:33:33 CET
 */

#ifndef EVENTS_HPP

#define EVENTS_HPP

# include "VHost.hpp"
# include "Client.hpp"

# include <vector>
# include <unordered_map>
# include <stdexcept>

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>

# define MAX_CONNECT 4092
# define MAX_TIME 600
# define CLEAN_TRESHOLD 500

class Events
{
	typedef std::unordered_map<int, VHost*> VHostObject;
	typedef std::unordered_map<int, Client*> ClientObject;

	private:
	/* private data */
	struct timespec *_timeout;
	bool			_on;
	int				event_fd;

	VHostObject		_vhost;
	ClientObject	_clients;

	public:
	explicit Events ( struct timespec *timeout ):
		_timeout(timeout),
		_on(1),
		event_fd(-1){}

	~Events () {
		for (VHostObject::iterator it = _vhost.begin(); it != _vhost.end(); ++it)
			delete it->second;
		for (ClientObject::iterator it =  _clients.begin(); it != _clients.end(); ++it)
			delete it->second;
		if (event_fd != -1)
			close(event_fd);
	}

	void	init( std::vector<serverBlock*> hosts ) {
		if (!_create_kqueue())
			throw std::runtime_error("kqueue() failed.");
		if (!_add_hosts(hosts))
			throw std::runtime_error("Error while adding virtual hosts.");
		if (!_add_stdin())
			throw std::runtime_error("Error while adding stdin.");
	}

	int	run() {
		struct kevent	events[MAX_CONNECT];
		int				total_event;

		std::cout << "[⚙️] server running...\n";
		while (_on) {
			int nfds = kevent(event_fd, 0, 0, events, MAX_CONNECT, _timeout);
			total_event += nfds;
			for (int i = 0; i < nfds; ++i) {
				int ev_fd = events[i].ident;
				if (ev_fd == STDIN_FILENO) {
					_handle_stdin();
					continue ;
				}
				if (events[i].flags & EV_ERROR) {
					_handle_error(ev_fd);
					continue ;
				}
				if (events[i].filter == EVFILT_READ) {
					VHostObject::iterator it = _vhost.find(ev_fd);
					if (it != _vhost.end())
						_handle_connection(it->second, ev_fd);
					else
						_handle_clientRead(ev_fd);
				} else if (events[i].filter ==  EVFILT_WRITE) {
					_handle_clientWrite(ev_fd);
				}
				if (!nfds || total_event > CLEAN_TRESHOLD) {
					_clean(); total_event = 0;
				}
			}
		}
		return (0);
	}

	private:
	/* private methods */
	// ------------------------------  INIT -------------------------------|
	bool	_create_kqueue() {
		event_fd = kqueue();
			return (event_fd >= 0);
	}

	bool	_add_hosts( std::vector<serverBlock*> hosts ) {
		for (std::vector<serverBlock*>::const_iterator cit = hosts.begin();
				cit != hosts.end(); ++cit) {
				if (!_addHost(*cit)) {
				std::cerr << "unable to add " << (*cit)->get_name() << ":" <<
				(*cit)->get_port() << "\n";
				return (false);
				}
		}
		return (true);
	}

	bool	_addHost( serverBlock const *host ) {
		VHost *new_host;

		try {
			new_host = new VHost(*host);
			if (!new_host) {
				std::cerr << "_addHost(): alloc failed\n";
				return (false);
			}
		} catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << "\n";
			return (false);
		}

		int host_fd = new_host->get_fd();
		if (host_fd == -1) {
			std::cerr << "_addHost(): invalide fd\n";
			delete new_host;
			return (false);
		}
		if (!_ev_addRead(host_fd)) {//add the vhost fd to read event interest list
			std::cerr << "_ev_addRead: failed to add fd to read envent list\n";
			delete new_host;
			return (false);
		}
		_vhost[host_fd] = new_host;
		return (true);
	}

	bool	_add_stdin() {
		return (_ev_addRead(STDIN_FILENO));
	}


	//-------------------
	// ------------------------------ Event Handel Functions ---------------|

	void	_handle_stdin() {
		static std::string line;

			std::getline(std::cin, line);
			if (line == "quit" || line == "exit") {
				_on = false;
				std::cout << "[🔑] shutting down...\n";
			}
	}

	void	_handle_error( int const ev_fd ) {
		ClientObject::iterator it = _clients.find(ev_fd);
		if (it != _clients.end())
			_delete_client(ev_fd, it->second);
		else
			close(ev_fd);
	}

	void	_handle_connection(serverBlock  *serv, int fd) {
		Client *client = new Client(serv, fd);
		if (!client) {
			std::cerr << "handle_connection(): alloc failed\n";
			return ;
		}

		int new_fd = client->get_fd();
		if (new_fd == -1) {
			delete client;
			std::cerr << "handle_connection(): invalid fd\n";
			return ;
		}

		if (!_ev_addClient(new_fd)) {
			delete client;
			std::cerr << "handle_connection(): faild to add client to event list\n";
			return ;
		}
		_clients[new_fd] = client;
	}

	void	_handle_clientRead( int const ev_fd ) {
		ClientObject::iterator it = _clients.find(ev_fd);
		if (it == _clients.end()) {
			std::cerr << "_handle_clientRead(): invalid fd\n";
			return ;
		}

		Client	*client = it->second;
		int ret = client->read_request();
		if (ret == Client::READ_EOF || ret == Client::READ_ERROR) //not the best way to handel EOF
			return _delete_client(ev_fd, client);
		_ev_switchState(ev_fd, 1);
	}

	void	_handle_clientWrite(int ev_fd) {
		Client *client = _clients[ev_fd];
		if (!client) {
			std::cerr << "_handle_clientWrite(): invalid fd\n";
			return;
		}

		if (client->send_response())
			return (_delete_client(ev_fd, client));
		_ev_switchState(ev_fd, 0);
		
	}

	void	_delete_client(int ev_fd, Client *client) {
//		close(ev_fd); // not sure
		/* not sure it is necessary */
		struct kevent kev[2];
		EV_SET(kev, ev_fd, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, 0);
		EV_SET(kev + 1, ev_fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, 0);
		kevent(ev_fd, kev, 2, 0, 0, 0);
		/* --- */
		delete client;
		_clients.erase(ev_fd);
	}
	
	void	_clean() {
		struct timeval now;
		gettimeofday(&now, NULL);

		ClientObject::iterator it = _clients.begin();
		for (; it != _clients.end(); it++) {
			if (it->second->is_expired(now.tv_sec)) {
				_delete_client(it->first, it->second);
				return _clean();
			}
		}
	}
	//--------------------------
	//---------------------------------- EV FUNCTION -----------------
	bool	_ev_addRead( int const sckfd ) {
		static struct kevent kev;

		EV_SET(&kev, sckfd, EVFILT_READ, EV_ADD, 0, 0, 0);
		if (kevent(event_fd, &kev, 1, 0, 0, 0) == -1)
			return (false);
		return (true);
	}

	bool	_ev_addClient( int const sckfd ) {
		static struct kevent kev[2];

		EV_SET(kev, sckfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
		EV_SET(kev + 1, sckfd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, 0);
		if (kevent(event_fd, kev, 2, 0, 0, 0) == -1)
			return (false);
		return (true);
	}

	//0 => enable read / disabe write
	//1 => disable read / enable write
	void	_ev_switchState( int const sckfd, bool rw ) {
		static u_short flags[2] = {EV_ENABLE, EV_DISABLE};
		static struct kevent kev[2];

		EV_SET(kev, sckfd, EVFILT_READ, flags[rw], 0, 0, 0);
		EV_SET(kev + 1, sckfd, EVFILT_WRITE, flags[(rw + 1) & 1], 0, 0, 0);
		kevent(event_fd, kev, 2, 0, 0, 0);
	}
};

#endif /* end of include guard EVENTS_HPP */

