#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
    struct tm daytime;
    time_t ti = 0;
    char *fmt;
    char timeFormat[100];

    if (argc != 2)
    {
        printf (" usage : strptime format(%%Y%%m%%d%%H%%M%%S) \n");
        return ti;
    }

    fmt  = argv[1];

    strptime(fmt, "%Y%m%d%H%M%S", &daytime);

	sprintf(timeFormat, "%04d-%02d-%02d %02d:%02d:%02d",
        daytime.tm_year + 1900,
        daytime.tm_mon + 1,
        daytime.tm_mday,
        daytime.tm_hour,
        daytime.tm_min,
        daytime.tm_sec);

    printf("%s \n", timeFormat);

    ti = mktime(&daytime);

    return ti;
}

