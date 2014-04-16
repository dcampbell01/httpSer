HTTP Server for CS 436
======================

Compilation Process:
--------------------

Type the following then press enter:
make




How to Run the Server:
----------------------

Make sure that a file named index.html, containing valid html, exists in the same 
directory as the executable.  In that directory, type the following then press 
enter at the end of each line:
chmod 777 index.htm
./server 12345

Test the server with a browser or with telnet, at IP Address 144.37.1.24.




The State of the Program:
-------------------------

GET, HEAD, and DELETE, and PUT work!*
  * PUT works in a strange way because telnet doesn't send full HTTP/1.0

Binds to the port you specify and all IP addresses that Empress (or host 
machine) has available.  For Empress, 144.37.1.24 and 127.0.0.1 are usually 
available.

No caching and no authentication.




Unresolved Issues:
------------------

* Sending 1 or 2 arguments will crash the server.  0, 3, and more are fine
* Would be nice to have it automatically find files of different file extensions from the same filename ( index.* , 
  where * could be .html, .htm, .txt, no extension, and possibly other)



Miscellaneous Notes:
--------------------

Empress is using gcc version 4.1.2 20080704 Red Hat 4.1.2-54
