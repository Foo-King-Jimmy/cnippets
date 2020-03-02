## How to compile sqlite from the rawest source

### How to cc (c compile) sqlite (sqlite3, sqlite3.o, libsqlite3.a, libsqlite3.so) from the rawest source.


use -lm

use sqlite3_init
           _shutdown
...
needed tcl, tclsh
needed gcc, and toolchain
not from preprocessed code
creates amalgation make sqlite3.c

check sqlite.org website for doc
link compile
link pragmas
link directives
link download

find stuff in .libs directory




mkdir -m777  /tmp/ccsqlite  && 
curl  https://www.sqlite.org/2020/sqlite-src-3310100.zip  --insecure --silent  --output /tmp/ccsqlite/sqlite.zip  && 
cd  /tmp/ccsqlite  && 
unzip  ./sqlite.zip  && 
rm  ./sqlite.zip  && 
cd  ./sqlite*  && 
./configure  \
   --enable-option-checking  \
   --enable-largefile  \
   --disable-threadsafe  \
   --disable-tcl  \
   --disable-editline  \
   --enable-tempstore=always  \
   --enable-shared=yes  \
   --enable-static=yes  \
   --disable-readline  \
   --disable-debug  \
   --enable-amalgamation  \
   --disable-load-extension  \
   --disable-memsys5  \
   --disable-memsys3  \
   --disable-fts3  \
   --enable-fts4  \
   --enable-fts5  \
   --enable-json1  \
   --enable-update-limit  \
   --disable-geopoly  \
   --disable-rtree  \
   --enable-session  \
   --disable-gcov  \
   --with-gnu-ld  \
   --without-readline-lib  \
   --without-readline-inc  \
   --without-tcl  \
   CFLAGS="  \
      -O2  \
      -Wall  \
      -Werror  \
      -Wno-unused-but-set-variable  \
      -DSQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=0  \
      -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=0  \
      -DSQLITE_ENABLE_JSON1  \
      -DSQLITE_ENABLE_FTS4  \
      -DSQLITE_ENABLE_FTS5  \
      -DHAVE_USLEEP  \
      -DSQLITE_USE_ALLOCA  \
      -DSQLITE_OMIT_DEPRECATED  \
      -DSQLITE_OMIT_PROGRESS_CALLBACK  \
      -DSQLITE_DQS=0  \
      -DSQLITE_OMIT_SHARED_CACHE  \
      -DSQLITE_LIKE_DOESNT_MATCH_BLOBS  \
      -DSQLITE_OMIT_DECLTYPE  \
      -DSQLITE_DEFAULT_MEMSTATUS=0  \
      -DSQLITE_MAX_EXPR_DEPTH=0  \
      -DSQLITE_OMIT_AUTOINIT  \
      -DSQLITE_OMIT_LOAD_EXTENSION  \
      -DSQLITE_DEFAULT_PCACHE_INITSZ=64  \
      -DSQLITE_DEFAULT_CACHE_SIZE=-8192  \
      -DSQLITE_DEFAULT_MMAP_SIZE=1073741824  \
      -DSQLITE_MAX_MMAP_SIZE=1073741824  \
      -DSQLITE_DEFAULT_PAGE_SIZE=8192  \
      -DSQLITE_DEFAULT_LOCKING_MODE=1  \
      -DSQLITE_TEMP_STORE=3  \
      -DSQLITE_DEFAULT_SYNCHRONOUS=0  \
      -DSQLITE_OMIT_WAL  \
      -DSQLITE_THREADSAFE=0 "  \
   LIBS="  \
      -lm  \
      -lc "  \
   2> _err_2.txt   1> _out_2.txt  && 
make  sqlite3.c  2> _err_3.txt   1> _out_3.txt  && 
make             2> _err_4.txt   1> _out_4.txt  && 
ls -al  ./.libs              >> _list_.txt  && 
find .  -name  "sqlite3"     >> _list_.txt  && 
find .  -name  "sqlite3.o"   >> _list_.txt  && 
find .  -name "*sqlite3.a"   >> _list_.txt  && 
find .  -name "*sqlite3.so"  >> _list_.txt  && 
echo  'DONE. END.'





