
/**  sqlite  client  **/

/**  Usage:
     # gcc -c client.c -O2 -Wall -Werror -o client.o
     # gcc    client.o -lc               -o sql
     # ./sqlite_server test.sqlite3db
     # ./sql "SELECT * FROM Cars;"
**/


#ifndef  __linux__
#error   "Really sorry, but only for Linux !"
#endif


#include  <errno.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/socket.h>
#include  <sys/types.h>
#include  <sys/un.h>
#include  <unistd.h>

#define   ERR(msg)   { fprintf( stderr, #msg ); return -1; }
#define   SOCKPATH   "/tmp/sqlite.socket"
#define   BUFFERSZ   ( 32 * 4096 )


int
subwrite( int fd, const void * buf, size_t count ) {
 ssize_t  rc;
 const void * ptr = buf;
  while( count ) {
    rc = write( fd, ptr, count );
    if( -1 == rc ) return -1;
    count -= rc;
    ptr   += rc;
  }
 return 0;
}

int
subread( int fd, void * buf, size_t count ) {
 ssize_t  rc;
 int  retcum = 0;
 void * ptr = buf;
  while( count ) {
    rc = read( fd, ptr, count );
    if( -1 == rc ) return -1;
    if(  0 == rc ) return retcum;
    count  -= rc;
    ptr    += rc;
    retcum += (int) rc;
  }
 return 0;
}


int
main( int argc, char **argv ) {


 uint64_t  msglen;
 int  clisock;
 struct sockaddr_un  addr;
 char  buffer[ BUFFERSZ ];

  if( 2 > argc ) ERR( argc too few.\n )
  msglen = (uint64_t) strlen( argv[ 1 ] );
  (void) memset( &addr, 0, sizeof(struct sockaddr_un) );
  addr.sun_family = AF_UNIX;
  (void) strncpy( addr.sun_path, SOCKPATH, sizeof(addr.sun_path) - 1 );
  if( -1 == ( clisock = socket( AF_UNIX, SOCK_STREAM, 0 ))) ERR( socket failed.\n )
  if(( connect( clisock, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un) ))) ERR( connect failed.\n )
  if(( subwrite( clisock, &msglen, 8 ))) ERR( subwrite query length failed.\n )
  if(( subwrite( clisock, argv[ 1 ], (size_t) msglen ))) ERR( subwrite query failed.\n )
  if(( subread( clisock, &msglen, 8 ))) ERR( subread response length failed.\n )
  while( msglen > sizeof(buffer) ) {
    if(( subread( clisock, buffer, sizeof(buffer) ))) ERR( subread response failed.\n )
    (void) fwrite( buffer, 1, sizeof(buffer), stdout );
    msglen -= sizeof(buffer);
  }
  if(( subread( clisock, buffer, (size_t) msglen ))) ERR( subread response failed.\n )
  (void) fwrite( buffer, 1, (size_t) msglen, stdout );
  (void) close( clisock );

 return 0;
}

