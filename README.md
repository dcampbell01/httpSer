HTTP Server for CS 436
======================

Compilation Process:
--------------------

Type the following then press enter:
make




How to Run the Server:
----------------------

Make sure that a file named index.htm, containing valid html, exists in the same directory as the executable.
In that directory, type the following then press enter at the end of each line:
chmod 777 index.htm
./server 12345




The State of the Program:
-------------------------

Receives HTTP\1.0 responses and processes them as if they were HTTP\0.9 (simple) requests.  This means no 
caching or authentication.  It creates and sends valid HTTP\1.0 responses. (or at least is meant to do that;
currently none of the HTTP methods are functioning properly)

Strings are now passed around sanely.  



Unresolved Issues:
------------------

- implement HEAD, GET, PUT, DELETE.
- Fix all lines marked !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! or ///////////////////////////////////////////
- Test that our server respods to HTTP\1.0 responses with an HTTP\0.9 response


Miscellaneous Notes:
--------------------

Empress is using gcc version 4.1.2 20080704 Red Hat 4.1.2-54
