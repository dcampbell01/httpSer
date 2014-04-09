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

Bare bones in place!



Unresolved Issues:
------------------

- rearrange ReadRequest to parse requests and select between the four required HTTP functions
- implement HTTPServer::GetIPAddress()    (Currently hardcoded to 127.0.0.1.  Worst case, hardcode to 144.37.1.24)
- implement HEAD, GET, PUT, DELETE.
- Fix all lines marked !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! or ///////////////////////////////////////////



Miscellaneous Notes:
--------------------

Empress is using gcc version 4.1.2 20080704 Red Hat 4.1.2-54
