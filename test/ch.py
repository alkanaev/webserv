import requests

def gen():
    var1 = "x=FOO&"
    var2 = "y=FOO123465789"
    x = var1.encode('utf8')
    y = var2.encode('utf8')
    yield x
    yield y

def chunked_post_no_upload(port: int) -> str:
    headers = {'Connection': 'keep-alive', 'Content-Type': 'application/x-www-form-urlencoded',
           'Transfer-Encoding': 'chunked'}
    r = requests.post("http://localhost:" + str(port) + "/cgi/file.tester", data=gen(), headers=headers)
    if (r.status_code != 200 and r.status_code != 201):
        return "Bad Status Code"
    if (len(r.text) != 78):
        print(len(r.text))
        return "Bad Length Response Body"
    return ""