import cgi
print("Content-type:text/html\r\n\r\n")
print('<pre>')
form = cgi.FieldStorage()
name: str = ""
surname: str = ""
for var in form:
    if (var == "first_name"):
        name = form[var].value
    if (var == "last_name"):
        surname = form[var].value
print("***********************************************")
print("Hello dear " + name + " " + surname + "! We wish you a great day <3")
print("***********************************************")
print('</pre>') 
