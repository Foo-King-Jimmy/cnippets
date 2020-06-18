#!/usr/local/bin/tcc  -run  -Wall -Werror  /usr/local/lib/libcurl.so


/**   settime_http.run.c   **/


#ifndef  __linux__
#error   "Really sorry, but only for Linux ! Using memfd_create() internally ."
#endif


#define   _XOPEN_SOURCE
#define   _GNU_SOURCE

#include  <curl/curl.h>
#include  <errno.h>
#include  <regex.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/mman.h>
#include  <sys/types.h>
#include  <time.h>
#include  <unistd.h>


#define   DBG       if( 1 )
#define   TIMEGOV   "https://www.time.gov/"
#define   UAGENT    "Hill-Billy Gates wanted to air the house, he opened the ..."
#define   CURL_CACERT_PEM    "/path/to/curl/cacert.pem"


/**  FORWARDS  **/

size_t  curlwritecb ( void * /* ptr */, size_t /* size */, size_t /* nmemb */, void * /* stream */ );
int     curltofile  ( const char * /* url */, FILE * /* outfile */, int /* mode */ );
int     fdmmap      ( int /* fdesc */, int /* fsize */, const char ** /* here */ );
char *  regexsearch ( const char * /* string */, const char * /* regexpattern */ );

/**  MAIN  **/

int
main( int argc, char **argv ) {

 char * text       = NULL,
      * tempp1     = NULL,
      * tempp2     = NULL;
 int  retsize      = -1,
      memfd_1      = -1;
 FILE * outmemfh_1 = NULL;

  //(0):  open memfd, memfh:
  if( -1 == ( memfd_1 = memfd_create( "settime_http_1", (unsigned int) 0 /* flags */ ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): memfd_create() failed - 1 .\n" );
    return -1;
  }
  if( !( outmemfh_1 = fdopen( memfd_1, "w+b" ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): fdopen() failed - 1 .\n" );
    return -1;
  }
  // (void) rewind( outmemfh_1 );

  //(1):  curl/download  https://time.gov/
  //      Content-Length: (around) 17,533 bytes
  if( 1 > ( retsize = curltofile( TIMEGOV, outmemfh_1, 1 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): curltofile() - 1 - failed: %d .\n", retsize );
    return -1;
  }
  (void) rewind( outmemfh_1 );
  if( -1 == fdmmap( memfd_1, retsize, &text )) {
    DBG  (void) fprintf( stderr, " ERROR: main(): fdmmap() failed - 1 .\n" );
    return -1;
  }

  //(2):  parse file/string: search for:  scripts/zzz__2af5949814feb5b8e34bbbed39cbc00cf54f0bd4.js
  //      <script type="text/javascript" src="scripts/zzz__2af5949814feb5b8e34bbbed39cbc00cf54f0bd4.js">
 const char  regexpattern_1[ ] =  \
   "<script[ \t]+type=\"text/javascript\"[ \t]+src=\"scripts/zzz__[abcdef0123456789]{40}\\.js\">";
  if( !( tempp1 = regexsearch( text, regexpattern_1 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): regexsearch() failed or NO_MATCH - 1 .\n" );
    return -1;
  }
  if( !( tempp2 = strstr( tempp1, "zzz__" ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): strstr() failed w NULL - 1 .\n" );
    return -1;
  }
  tempp2 += 5;

  // Request URL: https://www.time.gov/scripts/zzz__2af5949814feb5b8e34bbbed39cbc00cf54f0bd4.js
  // Connection: close
  // Content-Length: 14966
  // Referer: https://www.time.gov/

  //(3):  extract string for the next url,
  //      curl/download this url,
  //      mmap()  &&  parse, search for ... :
 char  url2[ ] = TIMEGOV "scripts/zzz__XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX.js\0";
  tempp1 = url2 + 34;
  (void) memcpy( tempp1, tempp2, (size_t) 40 );
  (void) munmap( (void *) text, (size_t) retsize );
  text    = NULL;
  retsize = -1;
  // (void) rewind( outmemfh_1 );
  if(( ftruncate( memfd_1, (off_t) 0 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): ftruncate() failed - 1 .\n" );
    return -1;
  }
  // HEADERs :
  //   Request URL: https://www.time.gov/scripts/zzz__2af5949814feb5b8e34bbbed39cbc00cf54f0bd4.js
  //   Connection: close               [ RESPONSE ]
  //   Content-Length: 14966           [ RESPONSE ]
  //   Referer: https://www.time.gov/  [ REQUEST  ]
  if( 1 > ( retsize = curltofile( url2, outmemfh_1, 2 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): curltofile() - 2 - failed: %d .\n", retsize );
    return -1;
  }
  (void) rewind( outmemfh_1 );
  if( -1 == fdmmap( memfd_1, retsize, &text )) {
    DBG  (void) fprintf( stderr, " ERROR: main(): fdmmap() failed - 2 .\n" );
    return -1;
  }

#error   "This file is only for demonstration purpose, do NOT actually use it! See these lines down !"
/**  Excerpt from file downloaded from www.time.gov  **/
////////////////////////////////////////////////////////////////////////////////////
////// USE OF THIS .CGI BY OUTSIDE SITES OR APPLICATIONS IS STRICTLY PROHIBITED ////
// OR USING THE TIME FROM THIS SITE IN ANY WAY FOR OTHER SITES IS ALSO PROHIBITED //
///////////////////////////////////////////////////////////////////////////////////

  // parse file/string: search for:  xmlHttp.open("GET", "/zzz__4c0ea8c77a29f753d60e28b71b297d8a9278f061.cgi?disablecache="
  //   xmlHttp.open("GET", "/zzz__4c0ea8c77a29f753d60e28b71b297d8a9278f061.cgi?disablecache=" + d.getTime(), false);
 const char  regexpattern_2[ ] =  \
   "xmlHttp\\.open[ \t]*\\([ \t]*\"GET\"[ \t]*,[ \t]*\"/zzz__[abcdef0123456789]{40}\\.cgi\\?disablecache=\"";
  if( !( tempp1 = regexsearch( text, regexpattern_2 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): regexsearch() failed or NO_MATCH - 2 .\n" );
    return -1;
  }
  if( !( tempp2 = strstr( tempp1, "zzz__" ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): strstr() failed w NULL - 2 .\n" );
    return -1;
  }
  tempp2 += 5;

  // extract string for the next url :
 time_t  now;
 char  url3[ ] = TIMEGOV "zzz__XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX.cgi\?disablecache=XXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0";
  tempp1 = url3 + 26;
  (void) memcpy( tempp1, tempp2, (size_t) 40 );
  (void) munmap( (void *) text, (size_t) retsize );
  text    = NULL;
  retsize = -1;
  // (void) rewind( outmemfh_1 );
  if(( ftruncate( memfd_1, (off_t) 0 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): ftruncate() failed - 1 .\n" );
    return -1;
  }
  tempp1 = url3 + 84;
  now = time( (time_t *) 0 );
//
  DBG  (void) fprintf( stdout, "now:  %lld .\n", (long long) now );
//
  (void) sprintf( tempp1, "%lld", (long long) now );
  now %= 1000;
  while( *tempp1 ) ++tempp1;
  (void) sprintf( tempp1, "%03lld", (long long) now );

  // HEADERs :
  //   Request URL: https://www.time.gov/zzz__4c0ea8c77a29f753d60e28b71b297d8a9278f061.cgi?disablecache=1586850571243
  //   Connection: close
  //   Referer: https://www.time.gov/
  //   Transfer-Encoding: chunked

  //(4):  curl/download this url :
  if( 1 > ( retsize = curltofile( url3, outmemfh_1, 3 ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): curltofile() - 3 - failed: %d .\n", retsize );
    return -1;
  }
  (void) rewind( outmemfh_1 );
  if( -1 == fdmmap( memfd_1, retsize, &text )) {
    DBG  (void) fprintf( stderr, " ERROR: main(): fdmmap() failed - 3 .\n" );
    return -1;
  }

  // parse time before setting :
 int64_t  temp64 = (int64_t) 0;
 struct timespec thistimespec;
 // thistimespec.tv_sec   // time_t -     seconds
 // thistimespec.tv_nsec  // long   - nanoseconds
  // (void) memset( &thistimespec, 0, sizeof( struct timespec ));
  if( !( tempp2 = strstr( text, "time3=\"" ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): strstr() failed w NULL - 3 .\n" );
    return -1;
  }
  tempp2 += 7;
 short  idx = (short) 0;
  for( ; 18 > idx; ++idx, ++tempp2 ) {
    if( '0' <= *tempp2  &&  '9' >= *tempp2 ) {
      temp64 *= (int64_t) 10;
      temp64 += (int64_t)( *tempp2 - '0' );
    }
    else break;
  }
  thistimespec.tv_nsec = (long)   (( temp64 % 1000000 ) * 1000 );
  thistimespec.tv_sec  = (time_t)  ( temp64 / 1000000 );
//
  DBG  (void) fprintf( stdout, "text:  %s .\n"  , text );
  DBG  (void) fprintf( stdout, "tvnsec:  %lld .\n", (long long) thistimespec.tv_nsec );
  DBG  (void) fprintf( stdout, "tvsec:  %lld .\n", (long long) thistimespec.tv_sec  );
//

  if(( clock_settime( (clockid_t) CLOCK_REALTIME, &thistimespec ))) {
    DBG  (void) fprintf( stderr, " ERROR: main(): clock_settime() failed .\n" );
    return -1;
  }

  // CLEAN-UP :
  if( 0 < retsize  &&  (text) ) (void) munmap( (void *) text, (size_t) retsize );
  if( outmemfh_1 ) (void) fclose( outmemfh_1 );
  //               (void)  close(    memfd_1 );

 return 0;
}

/**  END-OF-MAIN  **/

/**  FUNCTIONS  **/

size_t
curlwritecb( void * ptr, size_t size, size_t nmemb, void * stream ) {
 return fwrite( ptr, size, nmemb, (FILE *) stream );
}

int
curltofile( const char * url, FILE * outfile, int mode ) {
 CURL * curlctx;
 CURLcode  curlretval;
 struct curl_slist * headerchunk = NULL;
 int  retval = 0;
  if( 1 > mode || 3 < mode ) {
    DBG  (void) fprintf( stderr, " ERROR: curl_easy_perform() failed: wrong mode .\n" );
    return -1;
  }
  (void) curl_global_init( CURL_GLOBAL_ALL );
  if( !( curlctx = curl_easy_init())) {
    (void) curl_global_cleanup();
    return -1;
  }
  // TLS :
  (void) curl_easy_setopt( curlctx, CURLOPT_SSL_VERIFYPEER, 1L );
  (void) curl_easy_setopt( curlctx, CURLOPT_SSL_VERIFYHOST, 2L );
  (void) curl_easy_setopt( curlctx, CURLOPT_CAINFO, CURL_CACERT_PEM );
  // GENERAL OPTIONS :
  (void) curl_easy_setopt( curlctx, CURLOPT_WRITEDATA,      outfile );
  (void) curl_easy_setopt( curlctx, CURLOPT_WRITEFUNCTION,  curlwritecb );
  (void) curl_easy_setopt( curlctx, CURLOPT_NOPROGRESS,     1L );
  (void) curl_easy_setopt( curlctx, CURLOPT_FOLLOWLOCATION, 1L );
  // HTTP-HEADERS :
  //                 example:  removing a header:   \
  headerchunk = curl_slist_append( headerchunk, "Accept:" );
  headerchunk = curl_slist_append( headerchunk, "User-Agent: " UAGENT );
  if( 2 == mode  ||  3 == mode )
    headerchunk = curl_slist_append( headerchunk, "Referer: https://www.time.gov/" );
  //headerchunk = curl_slist_append( headerchunk, "Host: www.time.gov" );
  (void) curl_easy_setopt( curlctx, CURLOPT_HTTPHEADER, headerchunk );
  // URL :
  (void) curl_easy_setopt( curlctx, CURLOPT_URL, url );
  // ACTION :
  if( CURLE_OK != ( curlretval = curl_easy_perform( curlctx ))) {
    DBG  (void) fprintf( stderr, " ERROR: curl_easy_perform() failed: %s .\n", curl_easy_strerror( curlretval ));
    retval = -1;
  }
  else {
    (void) fputc( '\0', outfile );
    retval = (int) ftell( outfile );
    (void) fflush( outfile );
  }
  // CLEAN-UP :
  (void) curl_slist_free_all( headerchunk );
  (void) curl_easy_cleanup( curlctx );
  (void) curl_global_cleanup();
 return retval;
}

int
fdmmap( int fdesc, int fsize, const char **here ) {
  *here = mmap( (void *) NULL, (size_t) fsize, (int)( PROT_READ | PROT_WRITE ),
                (int) MAP_PRIVATE, fdesc, (off_t) 0 );
  if( MAP_FAILED == *here ) {
    DBG  (void) fprintf( stderr, " ERROR: fdmmap() mmap() failed.\n" );
    DBG  (void) fprintf( stderr, " ERROR: %s .\n", strerror( errno ));
    return -1;
  }
 return 0;
}

char *
regexsearch( const char * string, const char * regexpattern ) {
 char * retptr = NULL;
 regex_t  regexvar;
 regmatch_t  found[ 1 ];
 int  retval;
  (void) memset( &regexvar, 0, sizeof( regex_t ));
  (void) memset( found,     0, sizeof( regmatch_t ));
  if(( retval = regcomp( &regexvar, regexpattern, REG_EXTENDED ))) {
    DBG  (void) fprintf( stderr, " ERROR: regexsearch() regcomp() failed: %d .\n", retval );
    (void) regfree( &regexvar );
    return NULL;
  }
  if(( retval = regexec( &regexvar, string, (size_t) 1, found, (int) 0 ))) {
    (void) regfree( &regexvar );
    return NULL;
  }
  retptr = (char *)( string + found[ 0 ].rm_so );
  (void) regfree( &regexvar );
 return retptr;
}
