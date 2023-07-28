# webserv
The project typically involves implementing a HTTP server that can handle basic HTTP requests and serve static files.
<br/> <br/>
> usage


`$ make all`
<br/>
`$ ./webserv <config_file_name>`

<br/>
___________________________________________________________________________
<br/><br/>

## Here are some key points that are commonly covered in the web server project:

### 1- Understanding HTTP:
  We are expected to have a solid understanding of the HTTP protocol, including the different HTTP methods (GET, POST, etc.), headers, and response codes.

### 2- Socket Programming:
  The project involves socket programming to establish a connection with clients and handle incoming HTTP requests. We typically used system calls like socket(), bind(), listen(), and accept() to create and manage server sockets.

### 3- Parsing HTTP Requests:
  We had to parse incoming HTTP requests to extract relevant information such as the requested file path, query parameters, and headers. Proper parsing and handling of the HTTP request is essential for responding appropriately.

### 4- Handling Static Files:
  The web server should be able to serve static files such as HTML, CSS, JavaScript, images, etc. We need to read and send file contents in response to the client's request.

### 5- Response Generation:
  We are responsible for generating proper HTTP responses based on the received requests. This includes setting the appropriate response headers, response codes, and sending the response body (file content or error messages).

### 6- Concurrent Connections:
  Implementing support for handling multiple concurrent connections is an important aspect of a web server. We explored techniques such as threading, forking, or non-blocking I/O to handle multiple client connections efficiently.

### 7- Error Handling and Security:
  It is crucial to handle errors gracefully and implement basic security measures to protect against common vulnerabilities, such as path traversal attacks or unauthorized access to sensitive files.

<br/>

![server-is-fine-burn](https://github.com/stronk-exe/webserv/assets/94312066/dc336a27-51c4-40db-b760-beb160137e3a)

