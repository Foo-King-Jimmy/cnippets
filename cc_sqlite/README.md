# How to compile sqlite from raw source

**How to cc (c compile) sqlite (sqlite3) from the rawest source**

**in order to make sqlite3, sqlite3.o, libsqlite3.a, libsqlite3.so, etc.**

---

You will need a C compiler (gcc), possibly other basic utilities (can not list them).

You will definitely need Tcl (tcl, tclsh) to be successful.

---

Download the raw(est) source file ( , NOT the preprocessed C source ) !

Check the official website: https://www.sqlite.org/download.html

Scroll down for something like this:

---

**Alternative Source Code Formats**

sqlite-src-3310100.zip

(11.93 MiB)

Snapshot of the complete (raw) source tree for SQLite version 3.31.1.

See How To Compile SQLite for usage details.

---

NOT for this:

---

sqlite-preprocessed-3310100.zip

(2.48 MiB)

Preprocessed C sources for SQLite version 3.31.1.

---

Or get it with curl:

curl  https://www.sqlite.org/2020/sqlite-src-3310100.zip  --insecure --silent  --output /tmp/ccsqlite/sqlite.zip

---

Decompress then, as you would:

unzip  ./sqlite.zip

---

Get some help by typing:

./configure --help  |  more

---

Pay attention to the linking phase that may break on the missing  math  library,

therefore do not forget  **-lm**  , or by giving this line to  ./configure  :

LIBS=" -lm -lc "

---

You can produce the  "amalgation" .c file  by:

make  sqlite3.c

Or simply everything by the command:

make

---

Output should appear in the  .libs  subdirectory:

ls -al  ./.libs

+ sqlite3.o, libsqlite3.a, libsqlite3.so

Except the shell program:

ls -al  .

+ sqlite3

---

---

---

check sqlite.org website for doc
link compile
link pragmas
link directives
link download


./configure  \
   CFLAGS="  \
      -O2  \
      -Wall  \
      -Werror  \
      -Wno-unused-but-set-variable  \

-DSQLITE_OMIT_AUTOINIT  \

use sqlite3_init
           _shutdown



     -DSQLITE_OMIT_WAL  
        -DSQLITE_THREADSAFE=0 "  \
