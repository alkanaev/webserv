# congif:
NAME="ruServ"
PATH="../"
CONF="../configs/simple.conf"

import time
import subprocess
import requests #layzy
import sys
import json
import threading

def get_ports(conf: str) -> list:
    _file = open(conf, "r")
    port = []
    location = []
    for line in _file:
        if "listen" in line:
            last_p = (line.split()[1][:-1])
            port.append(last_p)
        elif "location" in line:

            port.append(last_p + line.split()[1])
            
    return port

def worker(location: list):
    for u in location:
        time.sleep(1);
        g = requests.get(u)

def main():
    pname = PATH + NAME
    if (len(sys.argv) > 1):
        conf = sys.argv[1]
    else:
        conf = CONF
    ports = get_ports(conf)
    print("open port: {0}".format(ports)) #test

#    webserv = subprocess.Popen([pname, conf], stdin=subprocess.PIPE)

    urls = ["http://127.0.0.1:" + p for p in ports]

    threads = []
    for i in range(50):
        t = threading.Thread(target=worker, args=(urls,))
        threads.append(t)
        time.sleep(0.01)
        t.start()
    
#    webserv.stdin.write("quit")
#    print (webserv.communicate()[0])
#    webserv.stdin.close()
#    webserv.wait()
    time.sleep(1);

if __name__ == "__main__":
    main()
    print("\n__END__");

