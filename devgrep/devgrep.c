

/**  devgrep.c  **/


// argv[ 1 ] => PATTERN searched for
// argv[ 2 ] => OUTPUTFILE


#include  <fcntl.h>
#include  <linux/fs.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/ioctl.h>
#include  <sys/types.h>
#include  <unistd.h>

#define   THISDEVICE   "/dev/sdb2"
#define   UNIT         ( 512 )
#define   DBG          ( 1 )


int
main( int argc, char **argv ) {


 uint8_t   buffer[ ( 2 * UNIT ) ],
           chara,
         * pattern;
 size_t  patternlen;
 unsigned long  blocksize = 0LU;
 uint64_t  blocksize64,
           gigabytecapacity,
           match    = (uint64_t) 0,
           errcount = (uint64_t) 0,
           maxblockidx,
           indexi = (uint64_t) 0;
 int  fdesc0 = -1,
      retval = -1,
      indexj =  0;
 FILE * output = stderr;
 ssize_t  ssizeret = (ssize_t) -1;


  if( -1 == ( fdesc0 = open( THISDEVICE, O_RDONLY ))) {
    fprintf( stderr, "\n open() failed .\n" );
    return -1;
  }
  if( -1 == ( retval = ioctl( fdesc0, BLKGETSIZE64, &blocksize64 ))) {
    fprintf( stderr, "\n ioctl(BLKGETSIZE64) failed .\n" );
    return -1;
  }
  if( -1 == ( retval = ioctl( fdesc0, BLKGETSIZE,   &blocksize   ))) {
    fprintf( stderr, "\n ioctl(BLKGETSIZE) failed .\n" );
    return -1;
  }
  fprintf( stderr, " . BLKGETSIZE64: %llu bytes.\n", (unsigned long long) blocksize64 );
  fprintf( stderr, " . BLKGETSIZE  : %lu sector.\n", (unsigned long ) blocksize );
  fprintf( stderr, " . UNIT        : %llu bytes/sector.\n", (unsigned long long)( blocksize64 / blocksize ));
  gigabytecapacity = blocksize64 / ( 1024 * 1024 * 1024 );
  fprintf( stderr, " . CAPACITY IN GIGABYTES: %llu gigabytes (floor).\n", (unsigned long long) gigabytecapacity );
  maxblockidx = blocksize64 / UNIT;
  fprintf( stderr, " . MAX BLOCK INDEX: %llu .\n", (unsigned long long)( maxblockidx ));
  fprintf( stderr, " . MODULO: %llu .\n", (unsigned long long)( blocksize64 % UNIT ));
//  (void) getchar();


  if( 1 == argc ) return 0;
  if( 3 != argc ) {
    fprintf( stderr, "\n argc not 3 .\n" );
    return -1;
  }
  pattern = (uint8_t *) argv[ 1 ];
  patternlen = strlen( pattern );
  chara = *pattern;
  fprintf( stderr, " . PATTERN: %s .\n", pattern );
  fprintf( stderr, " . OUTPUTFILE: %s .\n", argv[ 2 ] );
  fprintf( stderr, " . . .\n" );

  if( NULL == ( output = fopen( argv[ 2 ], "ab" ))) {
    fprintf( stderr, "\n fopen() failed - %s .\n", argv[ 2 ] );
    return -1;
  }
  fprintf( output, "\n\n . . .\n" );
  fprintf( output, " . BLKGETSIZE64: %llu bytes.\n", (unsigned long long) blocksize64 );
  fprintf( output, " . BLKGETSIZE  : %lu sector.\n", (unsigned long ) blocksize );
  fprintf( output, " . UNIT        : %llu bytes/sector.\n", (unsigned long long)( blocksize64 / blocksize ));
  fprintf( output, " . CAPACITY IN GIGABYTES: %llu gigabytes (floor).\n", (unsigned long long) gigabytecapacity );
  fprintf( output, " . MAX BLOCK INDEX: %llu .\n", (unsigned long long)( maxblockidx ));
  fprintf( output, " . MODULO: %llu .\n", (unsigned long long)( blocksize64 % UNIT ));
  fprintf( output, " . OUTPUTFILE: %s .\n", argv[ 2 ] );
  fprintf( output, " . PATTERN: %s .\n", pattern );
  fprintf( output, " . . .\n\n" );
  fflush( output );


  for( indexi = (uint64_t) 0; indexi < ( maxblockidx - 1 ); ++indexi ) {
    if( -1 == ( ssizeret = pread( fdesc0, buffer, sizeof( buffer ), (off_t)( indexi * UNIT )))) {
      ++errcount;
#if DBG
      fprintf( stderr, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n pread() failed .\n\n\n\n\n\n\n\n\n\n\n\n" );
      fprintf( output, "\n\n pread() failed .\n" );
      fprintf( output, " !!!:  %llu  - index failed .\n", (unsigned long long) indexi );
      fprintf( output, " !!!:  %llu  - at start-byte failed .\n\n", (unsigned long long)( indexi * UNIT ));
#endif
//    return -1;
      continue;
    }
#if DBG
    fprintf( stderr, " >>  %llu :  %llu - %llu .\n",
             (unsigned long long)( indexi ),
             (unsigned long long)( indexi * UNIT ),
             (unsigned long long)( indexi * UNIT + sizeof( buffer )));
#endif
    for( indexj = 0; indexj < ( 2 * UNIT - patternlen + 1 ); ++indexj ) {
      if( chara == buffer[ indexj ] ) {
        if( !memcmp( buffer + indexj, pattern, patternlen )) {
          ++match;
          fprintf( stderr, "\n\n\n $ M A T C H ! ! ! .\n\n" );
          fprintf( stderr, " $ %llu .\n\n\n\n", (unsigned long long)( indexi * UNIT + indexj ));
          fprintf( output, " $ BYTE :  %llu  ,\n", (unsigned long long)( indexi * UNIT + indexj ));
          fprintf( output, " $ BLOCK:  %llu  .\n\n", (unsigned long long)( indexi * UNIT ));
//        (void) getchar();
          fflush( output );
        }
      }
    }
  }

  fprintf( stderr, " . match-count: %llu .\n", (unsigned long long) match );
  fprintf( output, " . match-count: %llu .\n", (unsigned long long) match );
  fprintf( stderr, " ! pread()-error-count: %llu .\n", (unsigned long long) errcount );
  fprintf( output, " ! pread()-error-count: %llu .\n\n\n", (unsigned long long) errcount );
  (void) fclose( output );
  (void) close( fdesc0 );

 return 0;
}

