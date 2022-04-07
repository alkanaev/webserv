import cgi
print("Content-type:text/html\r\n\r\n")
print('<pre>')
form = cgi.FieldStorage()
for var in form:
    if (var != "Submit"):
        print(var, ": ", form[var].value)
print('</pre>') 
