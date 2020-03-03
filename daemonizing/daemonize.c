
// DAEMONIZE :

{

 // Used variables:
 pid_t        procid;
 int          indexi;
 sigset_t     mysignalset;
 long         longval;
 FILE       * pidlogfp;
 const char   pidlogfilename[ ] = "your_pid_logging_file_name_path";

  // Process:
  // 1st fork:
  if( 0 > ( procid = fork() )) { ERROR; }
  // parent returns:
  if( 0 != procid ) return 0;
  // setsid:
  if( (pid_t) -1 == setsid() ) { ERROR; }

  // Signal handling:
  // http://man7.org/linux/man-pages/man7/signal.7.html
  // https://stackoverflow.com/questions/10046916/is-it-possible-to-ignore-all-signals

  // Block all RT (real-time) signals:
  if(( sigemptyset( &mysignalset ))) { ERROR; }
  for( indexi = SIGRTMIN; SIGRTMAX >= indexi; ++indexi ) {
    if(( sigaddset( &mysignalset, indexi ))) { ERROR; }
  }
  if(( sigprocmask( SIG_SETMASK, &mysignalset, NULL ))) { ERROR; }

  // Signals that impossible to deal with:
  //   SIGKILL
  //   SIGSTOP

  // Signals to ignore:
  // http://man7.org/linux/man-pages/man2/signal.2.html
  // sighandler_t xxx = signal( int signum, sighandler_t handler );
  // if( SIG_ERR == xxx ) { ERROR_HAPPENED; }
  //jj: I just void the return value here ...
  (void) signal( SIGCHLD,   SIG_IGN );  // IMPORTANT to ignore in order to avoid filling up proc-table !
  (void) signal( SIGTERM,   SIG_IGN );
  (void) signal( SIGTRAP,   SIG_IGN );
  (void) signal( SIGQUIT,   SIG_IGN );
  (void) signal( SIGALRM,   SIG_IGN );
  (void) signal( SIGINT,    SIG_IGN );
  (void) signal( SIGHUP,    SIG_IGN );
  (void) signal( SIGUSR1,   SIG_IGN );
  (void) signal( SIGUSR2,   SIG_IGN );
  (void) signal( SIGTSTP,   SIG_IGN );
  (void) signal( SIGTTIN,   SIG_IGN );
  (void) signal( SIGTTOU,   SIG_IGN );
  (void) signal( SIGURG,    SIG_IGN );
  (void) signal( SIGPROF,   SIG_IGN );
  (void) signal( SIGIO,     SIG_IGN );
  (void) signal( SIGPWR,    SIG_IGN );
  (void) signal( SIGWINCH,  SIG_IGN );
  (void) signal( SIGVTALRM, SIG_IGN );
  (void) signal( SIGSTKFLT, SIG_IGN );
#ifdef  SIGEMT
  (void) signal( SIGEMT,    SIG_IGN );
#endif

  // Signals that are left as they are,
  // one may add a proper signal-handler to them:
  //   SIGSEGV
  //   SIGFPE
  //   SIGPIPE
  //   SIGXCPU
  //   SIGBUS
  //   SIGILL
  //   SIGXFSZ
  //   SIGSYS

  // Signals, other signals, being left as they are:
  //   SIGABRT  // used by abort()
  //   SIGCONT

  // End-Of-Signals .


  // 2nd fork:
  if( 0 > ( procid = fork() )) { ERROR; }
  // parent returns:
  if( 0 != procid ) return 0;

  // CHILD :

  // optional:
  // procid = getpid();

  // umask here:
  (void) umask( (mode_t)( S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH ));  // : 0077

  // Closing (open) file-descriptors:
  // (void) fclose( stdin  );
  // (void) fclose( stdout );
  // (void) fclose( stderr );
  if( (long) -1 == ( longval = sysconf( _SC_OPEN_MAX ))) { ERROR; }
  for( ; 0L <= longval; --longval ) close( longval );

  // Optional:
  // Sometimes it is usefull to keep the file-descriptors 0,1,2 ( STDIN/OUT/ERR ) .
  // This part re-opens them on /dev/null in order to keep them for later:
  if( -1 == ( indexi = open( "/dev/null", O_RDONLY ))) { ERROR; }
  if( STDIN_FILENO != indexi ) {
    if( -1 == dup2( indexi, STDIN_FILENO )) { ERROR; }
    (void) close( indexi );
  }
  if( -1 == ( indexi = open( "/dev/null", O_WRONLY ))) { ERROR; }
  if( STDOUT_FILENO != indexi ) {
    if( -1 == dup2( indexi, STDOUT_FILENO )) { ERROR; }
    (void) close( indexi );
  }
  if( -1 == ( indexi = open( "/dev/null", O_WRONLY ))) { ERROR; }
  if( STDERR_FILENO != indexi ) {
    if( -1 == dup2( indexi, STDERR_FILENO )) { ERROR; }
    (void) close( indexi );
  }

  // Optional:
  // This last part only to be used if you want to know/store
  // the process ID of the daemon. ( Maybe at some point you
  // want to check if this file actually exists. )
  if( NULL == ( pidlogfp = fopen( pidlogfilename, "wb" ))) { ERROR; }
  (void) fprintf( pidlogfp, "%d", procid );
  (void) fclose( pidlogfp );

}

// END-OF-DAEMONIZE .

