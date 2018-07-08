#ifndef LOG_UTILS_H_
#define LOG_UTILS_H_

#include <sys/time.h>

void open_log_file();

void close_log_file();

struct timeval *begin_clock();

void end_clock(struct timeval *begin, char *log_text);

void log_message(char *log_text);

void log_message_with_int_value(char *log_text, int value);

void log_message_with_str_value(char *log_text, char *value);

#endif /* LOG_UTILS_H_ */