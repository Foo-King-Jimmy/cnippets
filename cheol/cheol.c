#!/usr/local/bin/tcc  -run


/**   cheol.c   **/


// USAGE:  cheol.c [-uw] file1 [ file2 ... fileN ]
//         cheol.c  -u   file   => [unix] delete all \r, only \n left
//         cheol.c  -w   file   => [win]  all ending will be \r\n


const char * tempfile = "/tmp/cheol.run.tmp.file";

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>
#include  <unistd.h>

 void
 usage( const char * programname ) {
   fprintf( stderr, "\n\
\
 USAGE:  %s [-uw] file1 [ file2 ... fileN ]\n\
         %s  -u   file   => [unix] delete all \\r, only \\n left\n\
         %s  -w   file   => [win]  all ending will be \\r\\n\n\
\
\n", programname, programname, programname );
  exit( EXIT_FAILURE );
 }


int
main( int argc, char **argv )  {


  if( 2 > argc ) usage( argv[ 0 ] );

 FILE * filep = NULL,
      * tempp = NULL;
 int tempargc = argc,
     charc    = 0,
     tounix   = 0,
     option   = 0;

  if( !strcmp( "-u", argv[ 1 ] )) { option = 1; tounix = 1; --tempargc; }
  if( !strcmp( "-w", argv[ 1 ] )) { option = 1; tounix = 2; --tempargc; }
  if( 1 == tempargc ) usage( argv[ 0 ] );

  while( --tempargc ) {

    // open the file to be converted :
    if( NULL == ( filep = fopen( argv[ argc - tempargc ], "rb" ))) {  // PARTIAL ERROR
      fprintf( stderr, " Error: Can not open file \'%s\' !\n", argv[ argc - tempargc ] );
      continue;
    }

    // if no option selected then examine the file about needed conversion :
    if( !option ) {
      while( EOF != ( charc = fgetc( filep ))) {
        if( '\r' == charc ) { tounix = 1; break; }
        if( '\n' == charc ) { tounix = 2; break; }
      }
      rewind( filep );
    }

    // open the temp file for writing :
    if( NULL == ( tempp = fopen( tempfile, "wb+" ))) {  // FATAL ERROR
      fprintf( stderr, " ERROR: Can not open tempfile \'%s\' !\n", tempfile );
      fclose( filep );
      return -1;
    }

    // actual conversion into tempfile :
    if( 1 == tounix ) {
      while( EOF != ( charc = fgetc( filep ))) if( '\r' != charc ) fputc( charc, tempp );
    }
    else {  // win
      while( EOF != ( charc = fgetc( filep ))) {
        if( '\r' == charc ) continue;
        if( '\n' == charc ) {
          fputc( '\r',  tempp );
          fputc( charc, tempp );
          continue;
        }
        fputc( charc, tempp );
      }
    }

    // reopen original file to load back converted content :
    fclose( filep );
    if( NULL == ( filep = fopen( argv[ argc - tempargc ], "wb" ))) {  // FATAL ERROR
      fprintf( stderr, " ERROR: can not reopen file \'%s\' for writing back content !"  \
                       " Terminating .\n", argv[ argc - tempargc ] );
      fclose( tempp );
      return -1;
    }
    rewind( tempp );  // reposition tempfile before performing read-operation
    // load back converted content :
    while( EOF != ( charc = fgetc( tempp ))) fputc( charc, filep );

    // clean-up :
    fclose( tempp );
    fclose( filep );

  }  // while

  (void) remove( tempfile );

 return 0;
}

