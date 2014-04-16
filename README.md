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

GET, HEAD, and DELETE work!
Strings are now passed around sanely.  
No caching and no authentication.



Unresolved Issues:
------------------

- implement PUT.  It appears that telnet only sends the request line so implementing PUT is going to have be hacky...
- Fix all lines marked !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! or ///////////////////////////////////////////


Miscellaneous Notes:
--------------------

Empress is using gcc version 4.1.2 20080704 Red Hat 4.1.2-54
