
readme


#ifdef
__linux__
#error
#endif


#if 0
  gcc -c ./sqlive.client.0.c -o ccc.o
  gcc ccc.o -lc -o sql
  gcc -c ./sqlive.server.7.c  -o sss.o
  gcc sss.o  /usr/lib/i386-linux-gnu/libsqlite3.so.0.8.6 -lc -o sqlive
  ./sqlive test.1.db
#endif


