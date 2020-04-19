# set time by (a) http (page)

**Only for demonstration, do NOT actually use this!**

---

+ This script updates the system-time.
+ Obviously, NOT a replacement of NTP.
+ Depends on (lib)curl.

Some info/functions on:
+ how to use libcurl,
+ how to mmap a file,
+ how to parse it with (extended) regex in C.

**DO NOT ACTUALLY USE THIS SCRIPT!**
because

timestamp is gained from the site/page:  https://www.time.gov/

and

they prohibit the actual usage of their timestamp and servers.

Excerpt from (a) file downloaded from  www.time.gov :

+ ////////////////////////////////////////////////////////////////////////////////////
+ ////// USE OF THIS .CGI BY OUTSIDE SITES OR APPLICATIONS IS STRICTLY PROHIBITED ////
+ // OR USING THE TIME FROM THIS SITE IN ANY WAY FOR OTHER SITES IS ALSO PROHIBITED //
+ ///////////////////////////////////////////////////////////////////////////////////

