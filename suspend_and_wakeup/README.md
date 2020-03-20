# suspend and wake up

**Simple program that sets suspend mode and then wakes up the computer later**

---

This is only for Linux as it uses timerfd_* (Linux-specific) functions internally.

Reading materials:

+ http://man7.org/linux/man-pages/man2/timerfd_create.2.html
+ https://www.kernel.org/doc/Documentation/power/states.txt

You may check these commands:

+ $ cat /sys/power/state
+ $ cat /sys/power/mem_sleep

Possible suspend modes (if supported on the platform):

+ **FOR: /sys/power/state**
+ standby == Power-On Suspend (if supported)
+ freeze  == Suspend-To-Idle
+ disk    == Suspend-To-Disk == hibernation
+ mem
+ **FOR: /sys/power/mem_sleep**
+ s2idle  == Suspend-To-Idle (always available)
+ shallow == Power-On Suspend
+ deep    == Suspend-To-RAM

---

**USAGE:**

Eg.: program's name ( argv[0] ) : ./suspendandwakeup

1st (mandatory) argument ( argv[1] ) : how many minutes till suspend mode ( minimum is 0 )

2nd (mandatory) argument ( argv[2] ) : how many minutes to be in suspend mode ( minimum is 1 )

3rd (optional) argument ( argv[3] ) : type of suspend mode ( default is 'deep' )

All possible types for the program : standby, freeze, disk, s2idle, shallow  and deep .

**Examples:**

+ Type this for help, for usage information:

**$** ./suspendandwakeup

+ Type this for immediately suspend the computer and wake it up in a minute from RAM (deep) mode:

**$** sudo su

**$** ./suspendandwakeup 0 1

+ Type this for suspend after one minute for a hour long suspension in Suspend-To-Idle mode:

**$** sudo su

**$** ./suspendandwakeup 1 60 freeze

---

**Notes:**

+ From hibernation ('disk') the computer did NOT wake up (for me).
+ The mode 'standby' did NOT work for me, did NOT enter suspend at all.
+ Simply use only the (default) 'deep' mode, it is superior solution, anyway.

---
