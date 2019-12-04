#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

int main (void)
{
    struct timeval tv;
//    struct tm daytime;
//    time_t ti;

//    gettimeofday(&tv, NULL);

//    ti = tv.tv_sec;
//    localtime_r(&ti, &daytime);

//	printf("%04d-%02d-%02d %02d:%02d:%02d.%03d  feedWatchDog\n",
//        daytime.tm_year + 1900,
//        daytime.tm_mon + 1,
//        daytime.tm_mday,
//        daytime.tm_hour,
//        daytime.tm_min,
//        daytime.tm_sec,
//        (int)tv.tv_usec / 1000);
    
    return 0;
}
