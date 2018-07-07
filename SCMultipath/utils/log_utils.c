#include <stdio.h>
#include <stdlib.h>
#include "log_utils.h"

#define LOG_FILENAME "machete_log.txt"

/*
 * Variables
 */

FILE *log_file;

/*
 * Functions
 */

void open_log_file()
{
	// setup log file
	log_file = fopen(LOG_FILENAME, "a");
	log_message("<---------- Begin logging PREMIUM -----------");
}

void close_log_file()
{
	// close log file

	log_message("<---------- Ending logging PREMIUM -----------");
	fclose(log_file);
}

struct timeval *begin_clock()
{
	struct timeval *begin = (struct timeval *) malloc (sizeof(struct timeval));

	gettimeofday(begin, NULL);

	return begin;
}

void end_clock(struct timeval *begin, char *log_text)
{

	long int end_ms;
	long int begin_ms;
	long int result_ms;

	struct timeval *end = (struct timeval *) malloc (sizeof(struct timeval));

	gettimeofday(end, NULL);
	
	end_ms = end->tv_sec * 1000 + end->tv_usec/1000;
	begin_ms = begin->tv_sec * 1000 + begin->tv_usec/1000;
	result_ms = end_ms - begin_ms;

	fprintf(log_file,"-> %s = %ld ms\n", log_text, result_ms);
	printf(":::::::: %s %ld \n", log_text, result_ms);

	free(begin);
	free(end);
}

void log_message(char *log_text)
{
	fprintf(log_file,"-> %s \n", log_text);
}

void log_message_with_int_value(char *log_text, int value)
{
	fprintf(log_file,"-> %s %d\n", log_text, value);
}

void log_message_with_str_value(char *log_text, char *value)
{
	fprintf(log_file,"-> %s %s\n", log_text, value);
}
