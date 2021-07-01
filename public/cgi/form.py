#!/usr/bin/python
  
# Import CGI and CGIT module
import cgi, cgitb              
  
# to create instance of FieldStorage 
# class which we can use to work 
# with the submitted form data
form = cgi.FieldStorage()      
your_name = form.getvalue('name')    
  
# to get the data from fields
email = form.getvalue('email')   

print ("Content-type:text/html\n")
print ("<html>")
print ("<head>")
print ("<title>Python program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello, %s is can be contact with his email: %s</h2>" 
       % (your_name, email))
print ("</body>")
print ("</html>")