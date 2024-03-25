/*
Macro for time measuring.

To use this macro, you need to declare two double type variables to store the start time and the end time. The macro GET_TIME() will return the time to the variable you put in. e.g., for your program, you need to declare two varibles first:
double start, end;
at where you want to record the starting time, put
GET_TIME(start);
at where you want to record the end time, put
GET_TIME(end);
Then the elapsed time would be
end-start

*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <sys/time.h>

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

#endif
