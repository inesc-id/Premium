#ifndef _UTILS_
#define _UTILS_

// Common includes

#include <time.h>

// definitions

#define TRUE 1
#define FALSE 0

#define TIME_MAX_LENGHT 35

char *duplicate_string(const char *s);

char *get_current_time_string();
char *get_time_custom_string(struct tm *tm_info);
char *get_time_asctime_string(const struct tm *timeptr);
struct tm *get_current_time();
struct tm *duplicate_time_structure(struct tm* tm_info_to_copy);

#endif /* _UTILS_ */