# Rudimentary Server-Client front for the sqlite3 library

**Working, but absolutely basic skeleton C code**

**Sorry, only for Linux**

**Using UNIX DOMAIN SOCKET**

---

+ no error recovery
+ no real error checking
+ no dot commands
+ just a skeleton program
+ next to useless

---

**-lm** needed !!!

---

gcc -c client.c -O2 -Wall -Werror -o client.o

gcc    client.o -lc               -o sql

gcc -c server.c -O2 -Wall -Werror -o server.o

gcc server.o    sqlite3.o **-lm** -lc -o sqlite_server

or

gcc server.o  -lsqlite3   **-lm** -lc -o sqlite_server

./sqlite_server test.sqlite3db

./sql "SELECT * FROM Cars;"

---

+ -l:/lib/path/libsqlite3.so.0.8.6
+ sqlite3.o
+ libsqlite3.a
+ libsqlite3.so.0.8.6
+ -lsqlite3

---
