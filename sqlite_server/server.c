
/**  sqlite  server  using  UNIX DOMAIN SOCKET  **/

/**  Usage:
     # gcc -c server.c -O2 -Wall -Werror -o server.o
     # gcc server.o    sqlite3.o -lm -lc -o sqlite_server
or   # gcc server.o  -lsqlite3   -lm -lc -o sqlite_server
     # ./sqlite_server test.sqlite3db
     # ./sql "SELECT * FROM Cars;"
**/


#ifndef  __linux__
#error   "Really sorry, but only for Linux ! Using memfd_create() internally ."
#endif


#define   _GNU_SOURCE

#include  <errno.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/mman.h>
#include  <sys/socket.h>
#include  <sys/types.h>
#include  <sys/un.h>
#include  <unistd.h>

#include  "sqlite3.h"

#define   SOCKPATH   "/tmp/sqlite.socket"
#define   BACKLOG    ( 8 )
#define   TMPBUFSZ   ( 4096 )
#define   SRVBUFSZ   ( 32 * 4096 )
#define   ERRO(msg)  { rv = -1; (void) fprintf( errfile, #msg ); goto _END; }


int  gcnt;

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

void
fprint_html( FILE * out, const char * z ) {
 int i;
 if( 0 == z ) z = "";
  while( *z ) {
    for( i = 0; z[ i ]          &&
                z[ i ] != '<'   &&
                z[ i ] != '&'   &&
                z[ i ] != '>'   &&
                z[ i ] != '\"'  &&
                z[ i ] != '\''     ; ++i ) { }
    if( 0 < i ) fprintf( out, "%.*s", i, z );
    if     ( '<'  == z[ i ] ) fprintf( out, "&lt;"   );
    else if( '&'  == z[ i ] ) fprintf( out, "&amp;"  );
    else if( '>'  == z[ i ] ) fprintf( out, "&gt;"   );
    else if( '\"' == z[ i ] ) fprintf( out, "&quot;" );
    else if( '\'' == z[ i ] ) fprintf( out, "&#39;"  );
    else break;
    z += i + 1;
  }
 return;
}

int
frmtcb_html( void * aptr, int numColumns, char** txtColumns, char** nameColumns ) {
 FILE * fh = (FILE *) aptr;
 int  iii = 0;
  if( 0 == ++gcnt ) {
    fprintf( fh, "<tr>" );
    for( ; iii < numColumns; ++iii ) {
      fprintf( fh, "<th>" );
      fprint_html( fh, nameColumns[ iii ] );
      fprintf( fh, "</th>\n" );
    }
    fprintf( fh, "</tr>\n" );
  }
  if( NULL == txtColumns ) return 0;
  fprintf( fh, "<tr>" );
  for( iii = 0; iii < numColumns; ++iii ) {
    fprintf( fh, "<td>" );
    fprint_html( fh, txtColumns[ iii ] ? txtColumns[ iii ] : "(null)" );
    fprintf( fh, "</td>\n" );
  }
  fprintf( fh, "</tr>\n" );
 return 0;
}


int
main( int argc, char **argv ) {


 uint64_t  msglen;
 sqlite3 * db = NULL;
 off_t  tmpoff;
 int  rv      =  0,
      conn    = -1,
      clisock = -1,
      memfd   = -1;
 int (*frmtcb) ( void * , int, char** , char** );  // format-callback-function-ptr
 char * err    = NULL,
      * dynbuf = NULL,
        buffer[ SRVBUFSZ ],
        tmpbuf[ TMPBUFSZ ];
 const char * dbpath = NULL,
            * query  = NULL;
 FILE * errfile = stderr,
      * memstrm = NULL;
 struct sockaddr_un  addr;


  if( 2 > argc ) ERRO( argc too few missing db path.\n )
  dbpath = argv[ 1 ];
  frmtcb = frmtcb_html;
  (void) unlink( SOCKPATH );
  if( -1 == ( memfd = memfd_create( "sqlite_socket", (unsigned int) 0 ))) ERRO( memfd_create failed.\n )
  if( NULL == ( memstrm = fdopen( memfd, "r+b" ))) ERRO( fdopen failed.\n )
  (void) fseek( memstrm, 0L, SEEK_SET );  // a rewind() .
  (void) sqlite3_initialize();
  if( SQLITE_OK != sqlite3_open( dbpath, &db )) {
    (void) fprintf( errfile, "sqlite3_open failed with error message: %s .\n", sqlite3_errmsg( db ));
    (void) sqlite3_close( db ); db = NULL; ERRO( sqlite3_open failed.\n )
  }
  (void) memset( &addr, 0, sizeof(struct sockaddr_un) );
  addr.sun_family = AF_UNIX;
  (void) strncpy( addr.sun_path, SOCKPATH, sizeof(addr.sun_path) - 1 );
  if( -1 == ( conn = socket( AF_UNIX, SOCK_STREAM, 0 ))) ERRO( socket failed.\n )
  if( -1 == bind( conn, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un) )) ERRO( bind failed.\n )
  if( -1 == listen( conn, BACKLOG )) ERRO( listen failed.\n )

_NEXT:

  gcnt = -1;
  if( dynbuf ) { (void) free( dynbuf ); dynbuf = NULL; }
  else (void) memset( buffer, 0, sizeof(buffer) );
  if( -1 == ( clisock = accept( conn, NULL, NULL ))) ERRO( accept failed.\n )
  if(( subread( clisock, &msglen, 8 ))) ERRO( subread query length failed.\n )
  if( sizeof(buffer) > msglen ) {
    if(( subread( clisock, buffer, (size_t) msglen ))) ERRO( subread query clisock-buffer failed.\n )
    query = buffer;
  }
  else {
    if( NULL == ( dynbuf = calloc( (size_t)( 1 + msglen ), sizeof(char) ))) ERRO( calloc failed.\n )
    if(( subread( clisock, dynbuf, (size_t) msglen ))) ERRO( subread query clisock-dynbuf failed.\n )
    query = dynbuf;
  }
  if( SQLITE_OK != sqlite3_exec( db, query, frmtcb, (void *) memstrm, &err )) {
    (void) fprintf( errfile, "sqlite3_exec failed with error message: %s .\n", sqlite3_errmsg( db ));
    ERRO( sqlite3_exec failed.\n )
  }
  (void) fflush( memstrm );
  if( (off_t) -1 == ( tmpoff = lseek( memfd, (off_t) 0, SEEK_CUR ))) ERRO( lseek SEEK_CUR failed.\n )
  msglen = (uint64_t) tmpoff;
  if(( subwrite( clisock, &msglen, 8 ))) ERRO( subwrite response length failed.\n )
  // a rewind() :
  (void) fseek( memstrm, 0L, SEEK_SET );
  while( sizeof(tmpbuf) < msglen ) {
    if(( subread( memfd, tmpbuf, sizeof(tmpbuf) ))) ERRO( subread memfd-tmpbuf failed.\n )
    if(( subwrite( clisock, tmpbuf, sizeof(tmpbuf) ))) ERRO( subwrite tmpbuf-clisock failed.\n )
    msglen -= sizeof(tmpbuf);
  }
  if(( subread( memfd, tmpbuf, (size_t) msglen ))) ERRO( subread memfd-tmpbuf-left failed.\n )
  if(( subwrite( clisock, tmpbuf, (size_t) msglen ))) ERRO( subwrite tmpbuf-clisock-left failed.\n )
  (void) close( clisock );
  // a rewind() again:
  (void) fseek( memstrm, 0L, SEEK_SET );
  // or you write:
  // if( (off_t) 0 != lseek( memfd, (off_t) 0, SEEK_SET )) ERRO( lseek SEEK_SET failed.\n )
  if(( ftruncate( memfd, (off_t) 0 ))) ERRO( ftruncate failed.\n )
  goto _NEXT;

_END:

  if( dynbuf ) (void) free( dynbuf );
  if( -1 != clisock ) (void) close( clisock );
  if( err ) (void) sqlite3_free( err );
  if( db ) (void) sqlite3_close( db );
  (void) sqlite3_shutdown();
  if( -1 != conn ) (void) close( conn );
  (void) unlink( SOCKPATH );

 return rv;
}

