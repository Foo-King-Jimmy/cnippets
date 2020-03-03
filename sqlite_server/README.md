# Server-Client front for the sqlite3 library

**Working, but absolutely BASIC skeleton C code**

**Sorry, only for Linux**

---
-lm
needed !!!


---

gcc -c client.c -O2 -Wall -Werror -o client.o 2> __ERR__1.txt 1> __OUT__1.txt 

gcc client.o -lc -o sql  2> __ERR__2.txt 1> __OUT__2.txt 


!!!

 .. .. .. gcc -c server.c -O2 -Wall -Werror -o server.o 
 .. .. .. gcc server.o ./.libs/sqlite3.o **-lm** -lc  -o sqlite_server

///// gcc server.o    -lsqlite3 **-lm** -lc  -o sqlite_server


./sqlite_server test.sqlite3db 
./sql "SELECT * FROM Cars;"




---

gcc -c ./sqlive.client.0.c -O2 -Wall -Werror -o client.o

gcc client.o -lc -o sql

gcc -c ./sqlive.server.7.c -O2 -Wall -Werror -o server.o

gcc server.o -lsqlite3 -lm -lc -o sqlite_server

gcc server.o sqlite3.o -lm -lc -o sqlite_server

gcc server.o -l:/lib/path/libsqlite3.so.0.8.6 -lm -lc -o sqlite_server

./sqlite_server test.sqlite3db

---

+ sqlite3.o
+ libsqlite3.a
+ libsqlite3.so.0.8.6
+ -lsqlite3

---

./sqlite_server test.sqlite3db

sql "SELECT * FROM Cars;"
