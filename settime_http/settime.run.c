#!/usr/local/bin/tcc  -run


/**   settime.run.c   **/


#include  <time.h>

#define   TIME_SRC_URL   ""
#define   CURL_PATH      ""

int
main( int argc, char **argv ) {


/*  GET URL header+body > into 2 files w curl ,
    parse header for - 200
                     - Date: header to get TIME_1_URL
    parse body for TIME_2_UNIX
    sanity checks
    settime() in Linux
*/



//  https://www.time.gov/zzz__c4e4433c0157b757f0b0bb53565e1e6473ad7eca.cgi?disablecache=
//    <timestamp time2="1585125652133852" time3="1585125652133868"/>
//    <timestamp time2="1585125652 133852" \
                 time3="1585125652 133868"/>
////  http://worldclockapi.com/api/json/utc/now



///// timeout error code (28)
///// 0.5 means 500 milliseconds
///// curl --max-time 0.9  https://example.com/
///// User-Agent: ff (friendlyfire)



//  http://man7.org/linux/man-pages/man2/clock_settime.2.html
//    int clock_getres(clockid_t clk_id, struct timespec *res);
//    int clock_gettime(clockid_t clk_id, struct timespec *tp);
//    int clock_settime(clockid_t clk_id, const struct timespec *tp);
//  https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/qnx/clock_settime.html


CLOCK_REALTIME

struct timespec {
  time_t   tv_sec;        /* seconds */
  long     tv_nsec;       /* nanoseconds */
};

ERRORS         top
       EFAULT tp points outside the accessible address space.

       EINVAL The clk_id specified is not supported on this system.

       EINVAL (clock_settime()): tp.tv_sec is negative or tp.tv_nsec is
              outside the range [0..999,999,999].

       EINVAL (since Linux 4.3)
              A call to clock_settime() with a clk_id of CLOCK_REALTIME
              attempted to set the time to a value less than the current
              value of the CLOCK_MONOTONIC clock.

       EPERM  clock_settime() does not have permission to set the clock
              indicated.





 return 0;
}

