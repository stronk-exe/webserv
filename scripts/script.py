# #!/usr/bin/python3

# import cgi

# # Get form data
# form = cgi.FieldStorage()

# # Set content type
# print("Content-type: text/html")

# # Start HTML response
# print("<html>")
# print("<head>")
# print("<title>CGI Script Example</title>")
# print("</head>")
# print("<body>")

# # Process form data
# if "name" in form:
#     name = form["name"].value
#     print("<h1>Hello, {}!</h1>".format(name))
# else:
#     print("<h1>Hello, anonymous user!</h1>")

# # End HTML response
# print("</body>")
# print("</html>")

# #!/usr/bin/python
# print("Content-Type: text/html")  # Set the content type to HTML
# print()  # Print a blank line to indicate the end of headers

# # Output the HTML with CSS
# print("""
# <!DOCTYPE html>
# <html>
# <head>
#     <title>CGI Python Script</title>
#     <style>
#         body {
#             background-color: #f2f2f2;
#             font-family: Arial, sans-serif;
#         }
#         h1 {
#             color: #333;
#         }
#         p {
#             color: #666;
#         }
#     </style>
# </head>
# <body>
#     <h1>Hello, CGI Python!</h1>
#     <p>This is a Python script with CSS styling.</p>
# </body>
# </html>
# """)

#!/usr/bin/python

# Import CGI module and necessary libraries
import cgi
import cgitb

# Enable CGI error reporting for debugging (remove in production)
cgitb.enable()

# Create CGI form instance
form = cgi.FieldStorage()

# Set the content type to HTML and output the header
print("Content-Type: text/html\n")

# Output the HTML with CSS and image
print("""
<!DOCTYPE html>
<html>
<head>
    <title>CGI Python Script</title>
    <style>
        body {
            background-color: #f2f2f2;
            font-family: Arial, sans-serif;
        }
        h1 {
            color: #333;
        }
        p {
            color: #666;
        }
        img {
            max-width: 200px;
            height: auto;
        }
    </style>
</head>
<body>
    <h1>Hello, CGI Python!</h1>
    <p>This is a Python script with HTML, CSS, and an image.</p>
    <img src="https://img-comment-fun.9cache.com/media/apLx9WD/aYJpY0a2_700w_0.jpg" alt="Image">
</body>
</html>
""")
