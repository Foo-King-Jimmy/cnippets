# How to compile sqlite from raw source

**How to cc (c compile) sqlite (sqlite3) from the rawest source**

**in order to make sqlite3, sqlite3.o, libsqlite3.a, libsqlite3.so, etc.**

---

You will need a C compiler (**gcc**), possibly other basic utilities (can not list them).

You will definitely need Tcl (**tcl**, tclsh) to be successful.

Usefull link: Documentation : **https://sqlite.org/docs.html**

Usefull link: Compilation : **https://sqlite.org/howtocompile.html**

---

Download the raw(est) source file ( , NOT the preprocessed C source ) !

Check the official website !

Usefull link: Download : **https://sqlite.org/download.html**

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

Or get it with **curl**:

curl https://www.sqlite.org/2020/sqlite-src-3310100.zip --insecure --silent --output /tmp/ccsqlite/sqlite.zip

---

Decompress then, as you would:

unzip ./sqlite.zip

---

For help one can type:

./configure --help | more

Usefull link: Configuration : **https://sqlite.org/compile.html**

Usefull link: PRAGMAs : **https://sqlite.org/pragma.html**

---

Passing an extended **CFLAGS** variable to **./configure** :

FOR ME a simple CFLAGS=" -O2 -Wall -Werror " was just not enough.

I HAD TO add **-Wno-unused-but-set-variable** to avoid an error/warning and in order

to actually yield the shell program **sqlite3** .



Also pay attention to the linking phase that may break on missing math library !

So do not forget **-lm** , or by giving this variable to **./configure** :

LIBS=" -lm -lc "

---

You can produce the "amalgation" .c file by:

make sqlite3.c

Or simply everything with the command:

make

---

Output should appear in the **.libs** subdirectory:

ls -al ./.libs

+ sqlite3.o
+ libsqlite3.a
+ libsqlite3.so

Except the shell program **sqlite3** that is in **./** :

ls -al ./

+ sqlite3

---

---

---





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
