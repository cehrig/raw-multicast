#ifndef XML2QUEUE_LOG_H
#define XML2QUEUE_LOG_H

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define LOGFILE "/var/log/mouztrac.log"

typedef enum
{
    LOG_INFO,
    LOG_DEFAULT,
    LOG_DEBUG,
    LOG_ERROR,
    LOG_CRITICAL
} LOG_LEVEL;

FILE * openlog(int);
void writelog(LOG_LEVEL level, const char *, ...);

extern int verbose;

#endif //XML2QUEUE_LOG_H
