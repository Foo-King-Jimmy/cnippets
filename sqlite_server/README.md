# Server-Client front for the sqlite3 library

**Working, but absolutelly BASIC skeleton C code***

**Sorry, only for Linux***

#ifdef

__linux__

#error

#endif

---

#ifndef DEBUG

#error "Only Debug builds are supported"

#endif

---

gcc -c ./sqlive.client.0.c -O2 -Wall -Werror -o client.o

gcc client.o -lc -o sql

gcc -c ./sqlive.server.7.c -O2 -Wall -Werror -o server.o

gcc server.o  /usr/lib/i386-linux-gnu/libsqlite3.so.0.8.6 -lm -lc -o sqlite_server

./sqlite_server test.sqlite3db &

---

+ sqlite3.o
+ libsqlite3.a
+ libsqlite3.so.0.8.6

---

./sqlite_server test.sqlite3db &

sql "SELECT * FROM Cars;"

// u may use sqlite_init

// u may use sqlite_shutdown
