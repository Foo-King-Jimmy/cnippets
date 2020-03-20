#!/usr/local/bin/tcc  -run


/* ident: suspendandwakeup.c */


#ifndef   __linux__
 #error   "Sorry, only for Linux. Uses timerfd_* internally."
#endif

#include  <stdint.h>    // uint64_t
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/timerfd.h>
#include  <time.h>
#include  <unistd.h>

/*---------------------------------------------*\
  standby == Power-On Suspend (if supported)
  freeze  == Suspend-To-Idle
  disk    == Suspend-To-Disk == hibernation
  mem
  s2idle  == Suspend-To-Idle (always available)
  shallow == Power-On Suspend
  deep    == Suspend-To-RAM
\*---------------------------------------------*/

#define   OPT_STANDBY   (0)
#define   OPT_FREEZE    (1)
#define   OPT_DISK      (2)
#define   OPT_IDLE      (3)
#define   OPT_SHALLOW   (4)
#define   OPT_DEEP      (5)

const char *
suspendmode[ ] = { "standby", "freeze", "disk", "s2idle", "shallow", "deep" };


int
main( int argc, char **argv ) {


 struct itimerspec  alarmtime;
 struct timespec    nowtime;
 FILE * fhandle = NULL;
 uint64_t  expiry;
 int    timerfd = -1,
        charc,
        minstillsuspend = 0,
        suspendmins     = 1,
        modechoice      = OPT_DEEP;

 // See: https://www.kernel.org/doc/Documentation/power/states.txt
 const char  syspowerstate[ ] = "/sys/power/state",
             syspowermem  [ ] = "/sys/power/mem_sleep";

  if( 4 != argc  &&  3 != argc ) {

_USAGE:

    (void) printf( "HELP: \'%s\' usage:\n", argv[0] );
    (void) printf( "You will probably need superuser privilege to run this program successfully!\n" );
    (void) printf( "\n" );
    (void) printf( "%s  x1  x2  [x3]\n", argv[0] );
    (void) printf( "   x1 :  minutes till suspend state [ min: 0 ],\n" );
    (void) printf( "   x2 :  minutes of being suspended [ min: 1 ],\n" );
    (void) printf( "   x3 :  type of suspend mode [ OPTIONAL, default: deep ].\n" );
    (void) printf( "\n" );
    if( !( fhandle = fopen( syspowerstate, "rb" ))) {
      (void) printf( "Could not open file '%s' .\n", syspowerstate );
    }
    else {
      (void) printf( "Content of file '%s' :\n   ", syspowerstate );
      while( EOF != ( charc = fgetc( fhandle ))) (void) fputc( charc, stdout );
      (void) fclose( fhandle );
      (void) printf( "\n" );
      if( !( fhandle = fopen( syspowermem, "rb" )))
        (void) printf( "Could not open file '%s' .\n", syspowermem );
      else {
        (void) printf( "Content of file '%s' :\n   ", syspowermem );
        while( EOF != ( charc = fgetc( fhandle ))) (void) fputc( charc, stdout );
        (void) fclose( fhandle );
      }
    }
    (void) fputc( '\n', stdout );
    return 0;
  }


  // minutes till suspend state:
  if( strlen( argv[1] ) != strspn( argv[1], "1234567890" )) goto _USAGE;
  else minstillsuspend = atoi( argv[1] );
  if( 0 > minstillsuspend ) goto _USAGE;

  // minutes of being suspended:
  if( strlen( argv[2] ) != strspn( argv[2], "1234567890" )) goto _USAGE;
  else suspendmins = atoi( argv[2] );
  if( 1 > suspendmins ) goto _USAGE;

  // type of suspend mode:
  if( 4 == argc ) {
    if(      !strcmp( argv[3], suspendmode[ OPT_STANDBY ] )) modechoice = OPT_STANDBY;
    else if( !strcmp( argv[3], suspendmode[ OPT_FREEZE  ] )) modechoice = OPT_FREEZE;
    else if( !strcmp( argv[3], suspendmode[ OPT_DISK    ] )) modechoice = OPT_DISK;
    else if( !strcmp( argv[3], suspendmode[ OPT_IDLE    ] )) modechoice = OPT_IDLE;
    else if( !strcmp( argv[3], suspendmode[ OPT_SHALLOW ] )) modechoice = OPT_SHALLOW;
    else if( !strcmp( argv[3], suspendmode[ OPT_DEEP    ] )) modechoice = OPT_DEEP;
    else goto _USAGE;
  }

  if( -1 == clock_gettime( CLOCK_REALTIME, &nowtime )) {
    (void) fprintf( stderr, "ERROR: clock_gettime() failed.\n" );
    return -1;
  }
  alarmtime.it_value.tv_sec  = nowtime.tv_sec + ( suspendmins + minstillsuspend ) * 60;
  alarmtime.it_value.tv_nsec = nowtime.tv_nsec;
  alarmtime.it_interval.tv_sec = alarmtime.it_interval.tv_nsec = 0;  // only 1 alarm

  if( -1 == ( timerfd = timerfd_create( CLOCK_REALTIME_ALARM, 0 ))) {
    (void) fprintf( stderr, "ERROR: timerfd_create() failed.\n" );
    return -1;
  }
  if( -1 == timerfd_settime( timerfd, TFD_TIMER_ABSTIME, &alarmtime, NULL )) {
    (void) fprintf( stderr, "ERROR: timerfd_settime() failed.\n" );
    return -1;
  }


  (void) fprintf( stderr, "\n Timer started !\n" );
  if( minstillsuspend ) (void) sleep( minstillsuspend * 60 );

  switch( modechoice ) {
    case OPT_STANDBY:
    case OPT_FREEZE:
    case OPT_DISK:
      if( !( fhandle = fopen( syspowerstate, "w+b" ))) {
        (void) fprintf( stderr, "Could not open file '%s' (2).\n", syspowerstate );
        return -1;
      }
      (void) fprintf( fhandle, "%s\n", suspendmode[ modechoice ] );
      (void) fclose( fhandle );
      break;
    case OPT_IDLE:
    case OPT_SHALLOW:
    case OPT_DEEP:
      if( !( fhandle = fopen( syspowermem, "w+b" ))) {
        (void) fprintf( stderr, "Could not open file '%s' (2).\n", syspowermem );
        return -1;
      }
      (void) fprintf( fhandle, "%s\n", suspendmode[ modechoice ] );
      (void) fclose( fhandle );
      if( !( fhandle = fopen( syspowerstate, "w+b" ))) {
        (void) fprintf( stderr, "Could not open file '%s' (3).\n", syspowerstate );
        return -1;
      }
      (void) fprintf( fhandle, "mem\n" );
      (void) fclose( fhandle );
      break;
    default:
      (void) fprintf( stderr, "ERROR: Impossible choice error.\n" );
      return -1;
  }


  if( (ssize_t) sizeof(uint64_t) != read( timerfd, &expiry, sizeof(uint64_t) )) {
    (void) fprintf( stderr, "ERROR: read() failed.\n" );
    return -1;
  }


 return 0;
}


