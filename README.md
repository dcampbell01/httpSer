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
------------------------

 Finally figured out how to pass things around sanely in C.  All that needs to be implmented are the required 
HTTP methods (for HTTP\0.9 functionality).



Unresolved Issues:
------------------

- implement HEAD, GET, PUT, DELETE.
- Fix all lines marked !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! or ///////////////////////////////////////////
- Test that our server respods to HTTP\1.0 responses with an HTTP\0.9 response


Miscellaneous Notes:
--------------------

Empress is using gcc version 4.1.2 20080704 Red Hat 4.1.2-54
