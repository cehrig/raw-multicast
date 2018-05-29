#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "config.h"
#include "log.h"

int verbose = 0;

FILE * openlog(int daemon)
{
    FILE * fd;

    fd = fopen(LOGFILE, "a+");

    if(fd == NULL) {
        fprintf(stdout, "Can not open logfile!");
        exit(255);
    }

    if(daemon) {
        dup2(fileno(fd), STDOUT_FILENO);
        close(fileno(fd));
    }

    return fd;
}

void writelog(LOG_LEVEL level, const char * msg, ...)
{
    if(!verbose && level < LOG_INFO) {
        return;
    }

    char * lvl;
    switch(level) {
        case LOG_INFO:
            lvl = "INFO";
            break;
        case LOG_DEFAULT:
            lvl = "Ok";
            break;
        case LOG_DEBUG:
            lvl = "Debug";
            break;
        case LOG_ERROR:
            lvl = "Error";
            break;
        case LOG_CRITICAL:
            lvl = "Critical";
    }

    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("%s - %s\t: ", buffer, lvl);

    va_list ap;
    va_start (ap, msg);
    while(*msg) {
        if(*(msg) != '%') {
            printf("%c", *msg);
        } else {
            switch(*++msg) {
                case 's':
                    printf("%s", va_arg (ap, char *));
                    break;
                case 'd':
                    printf("%d", va_arg (ap, int));
                    break;
                case 'c':
                    printf("%c", va_arg (ap, int));
                    break;
                case 'f':
                    printf("%f", va_arg (ap, double));
                    break;
                case 'l':
                    printf("%ld", va_arg (ap, long));
                    break;
                case 'p':
                    printf("%p", va_arg (ap, void *));
                    break;
                case 'x':
                    printf("%x", va_arg (ap, int));
                    break;
                case 'u':
                    printf("%u", va_arg (ap, int));
                    break;
                case 'y':
                    printf("%Lf", va_arg (ap, long double));
                    break;
            }
        }
        msg++;
    }

    va_end (ap);
    printf("\n");
    fflush(stdout);

    if(level == LOG_ERROR) {
        if(errno) {
            writelog(LOG_DEBUG, "%s", strerror(errno));
        }
        exit(EXIT_FAILURE);
    }
}
