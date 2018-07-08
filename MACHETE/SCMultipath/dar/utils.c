#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/*
	Same as strdup, but redefined this because it is not part of the standard C library
*/
char *duplicate_string (const char *s) {
	char *aux = (char*) malloc(sizeof(char)*(strlen(s)+1)); 
    if (aux != NULL) { 
        strcpy(aux,s);
    }
    return aux;
}


// Time Utils

/*
	Time format: 2017-11-18 17:04:43
*/
char *get_current_time_string() 
{
	time_t timer;
    char *buffer = (char*) malloc(sizeof(char)*(TIME_MAX_LENGHT));
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, TIME_MAX_LENGHT, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

char *get_time_custom_string(struct tm *tm_info)
{
    char *buffer = (char*) malloc(sizeof(char)*(TIME_MAX_LENGHT));
    strftime(buffer, TIME_MAX_LENGHT, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

char *get_time_asctime_string(const struct tm *timeptr)
{
    return duplicate_string(asctime(timeptr));
}

struct tm *get_current_time() 
{
    time_t timer;
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    struct tm* new_time = duplicate_time_structure(tm_info);

    return new_time;
}

struct tm* duplicate_time_structure(struct tm* tm_info_to_copy)
{
    struct tm* new_time = (struct tm*) malloc(sizeof(struct tm));
    new_time->tm_sec = tm_info_to_copy->tm_sec;
    new_time->tm_min = tm_info_to_copy->tm_min;
    new_time->tm_hour = tm_info_to_copy->tm_hour;
    new_time->tm_mday = tm_info_to_copy->tm_mday;
    new_time->tm_mon = tm_info_to_copy->tm_mon;
    new_time->tm_year = tm_info_to_copy->tm_year;
    new_time->tm_wday = tm_info_to_copy->tm_wday;
    new_time->tm_yday = tm_info_to_copy->tm_yday;
    new_time->tm_isdst = tm_info_to_copy->tm_isdst;
    return new_time;
}
