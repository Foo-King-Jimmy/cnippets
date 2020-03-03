

/**  rawdisplay.c  **/


#include  <ctype.h>
#include  <fcntl.h>
#include  <linux/fs.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/ioctl.h>
#include  <sys/types.h>
#include  <unistd.h>

#define   UNIT   ( 512 )
#define   DBG    if( 0 )


int
setoutfile( FILE **output_, char * outfilename_, size_t outfilenamesz ) {
  fprintf( stderr, "\n  ENTER OUT FILE NAME :\n  > " );
  (void) memset( outfilename_, 0, outfilenamesz );
  scanf( "%s", outfilename_ );
  if( NULL != *output_ ) { fflush( *output_ ); fclose( *output_ ); }
  if( NULL == ( *output_ = fopen( outfilename_, "ab" ))) {
    fprintf( stderr, "\n fopen() failed - %s .\n", outfilename_ );
    return -1;
  }
  fprintf( stderr, "  %s opened for append.\n", outfilename_ );
 return 0;
}


void
setbyteoffest( off_t * position_ ) {
 int  indexj =  0;
 const char * numberin;
 char  byteoffset[ 128 ];
  (void) memset( byteoffset, 0, sizeof( byteoffset ));
  fprintf( stderr, "\n  ENTER BYTE OFFSET :\n  > " );
  scanf( "%s", byteoffset );
  numberin = byteoffset;
  *position_ = (off_t) 0;
  while( *numberin ) {
    if(( isdigit( *numberin ))) {
      *position_ *= 10;
      *position_ += ( *numberin - '0' );
    }
    else break;
    ++numberin;
  }
  indexj = *position_ % UNIT;
  *position_ -= indexj;
 return;
}


void
printing( FILE * output_, const uint8_t * buffer_, size_t buffersz ) {
 unsigned int  indexi = (unsigned int) 0;
  fprintf( output_, "\n---------------------------------------------------------------------------------\n" );
  for( ; indexi < buffersz; ++indexi ) {
    if(( isprint( buffer_[ indexi ] ))) {
      if( 127 > buffer_[ indexi ] ) fputc( buffer_[ indexi ], output_ );
      else                          fputc( ' ', output_ );
    }
    else {
      if(      '\n' == buffer_[ indexi ] ) fputc( buffer_[ indexi ], output_ );
      else if( '\r' == buffer_[ indexi ] ) { }
      else                                 fputc( ' ', output_ );
    }
  }
  fprintf( output_, "\n---------------------------------------------------------------------------------\n" );
  (void) fflush( output_ );
 return;
}


int
main( int argc, char **argv ) {


 FILE * output = NULL;
 uint8_t   buffer[ UNIT ];
 ssize_t  ssizeret;
 off_t  position = (off_t) 0;
 int  fdesc0 = -1;
 size_t  buffersize;
 char  device[ 16 ],
       command[ 128 ],
       outfilename[ 4096 ];


  buffersize = sizeof( buffer );
  // enter device ( ONCE ):
  (void) memset( device, 0, sizeof( device ));
  fprintf( stderr, "\n\n  ENTER DEVICE NAME as \'/dev/sdXX\' :\n  > " );
  scanf( "%s", device );
  if( -1 == ( fdesc0 = open( device, O_RDONLY ))) {
    fprintf( stderr, "\n open() failed .\n" );
    return -1;
  }
  fprintf( stderr, "  %s opened.\n", device );

  // enter out-filename ( FIRST ):
  if(( setoutfile( &output, outfilename, sizeof( outfilename )))) return -1;

  // enter byte-position ( FIRST ):
  (void) memset( buffer, 0, sizeof( buffer ));
  setbyteoffest( &position );

  (void) memset( buffer, 0, sizeof( buffer ));
  if( -1 == ( ssizeret = pread( fdesc0, buffer, sizeof( buffer ), position ))) {
    fprintf( stderr, "\n pread() failed .\n" );
    return -1;
  }
  printing( stderr, buffer, buffersize );

  // menu:
  while( 1 ) {
    fprintf( stderr, "\n---------------------------------------------------------------------------------\n" );
    fprintf( stderr, "  o UTFILENAME :  %s\n", outfilename );
    fprintf( stderr, "  b YTEPOSITION:  %llu\n", (unsigned long long) position );
    fprintf( stderr, "  x:exit  n:next  p:previous  w:write\n" );
    fprintf( stderr, "  o:set_output_filename  b:set_byte_offset" );
    fprintf( stderr, "\n---------------------------------------------------------------------------------\n" );
    fprintf( stderr, "  ENTER NEW COMMAND :\n  > " );
    (void) memset( command, 0, sizeof( command ));
    scanf( "%s", command );
    if( 'n' == *command ) {
      position += 512;
      (void) memset( buffer, 0, sizeof( buffer ));
      if( -1 == ( ssizeret = pread( fdesc0, buffer, sizeof( buffer ), position ))) {
        fprintf( stderr, "\n pread() failed .\n" );
        return -1;
      }
      printing( stderr, buffer, buffersize );
      continue;
    }
    else if( 'p' == *command ) {
      position -= 512;
      (void) memset( buffer, 0, sizeof( buffer ));
      if( -1 == ( ssizeret = pread( fdesc0, buffer, sizeof( buffer ), position ))) {
        fprintf( stderr, "\n pread() failed .\n" );
        return -1;
      }
      printing( stderr, buffer, buffersize );
      continue;
    }
    else if( 'b' == *command ) {
      setbyteoffest( &position );
      (void) memset( buffer, 0, sizeof( buffer ));
      if( -1 == ( ssizeret = pread( fdesc0, buffer, sizeof( buffer ), position ))) {
        fprintf( stderr, "\n pread() failed .\n" );
        return -1;
      }
      printing( stderr, buffer, buffersize );
      continue;
    }
    else if( 'o' == *command ) {
      if(( setoutfile( &output, outfilename, sizeof( outfilename )))) return -1;
    }
    else if( 'w' == *command ) {
      fprintf( output, "\n---------------------------------------------------------------------------------\n" );
      fprintf( output, "  BYTE-POSITION:  %llu", (unsigned long long) position );
      printing( output, buffer, buffersize );
      continue;
    }
    else if( 'x' == *command ) {
      fprintf( stderr, "\n  . Exiting.\n" );
      break;
    }
    else {
      fprintf( stderr, "\n  . Unknown command.\n" );
      continue;
    }
  }  // while( 1 )
  (void) fclose( output );
  (void) close( fdesc0 );
 return 0;
}

