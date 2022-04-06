import cgi
print("Content-type:text/html\r\n\r\n")
print('<pre>')
form = cgi.FieldStorage()
for var in form:
    print("field:", var)
    print("value:", form[var].value)
print('</pre>') 