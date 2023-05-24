#!/usr/bin/python3

import cgi

# Get form data
form = cgi.FieldStorage()

# Set content type
print("Content-type: text/html")

# Start HTML response
print("<html>")
print("<head>")
print("<title>CGI Script Example</title>")
print("</head>")
print("<body>")

# Process form data
if "name" in form:
    name = form["name"].value
    print("<h1>Hello, {}!</h1>".format(name))
else:
    print("<h1>Hello, anonymous user!</h1>")

# End HTML response
print("</body>")
print("</html>")
