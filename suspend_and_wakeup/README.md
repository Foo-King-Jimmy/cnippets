# x



/* ident: suspend_and_wakeup.c */


 #error   "Sorry, only for Linux. Uses timerfd_* internally."

/*---------------------------------------------*\
  standby == Power-On Suspend (if supported)
  freeze  == Suspend-To-Idle
  disk    == Suspend-To-Disk == hibernation
  mem
  s2idle  == Suspend-To-Idle (always available)
  shallow == Power-On Suspend
  deep    == Suspend-To-RAM
\*---------------------------------------------*/
standby", "freeze", "disk", "s2idle", "shallow", "deep" };


https://www.kernel.org/doc/Documentation/power/states.txt
/sys/power/state
/sys/power/mem_sleep
 
