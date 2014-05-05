/*
 * NAME:
 * fmerrmsg
 * fmlogmsg
 *
 * PURPOSE:
 * Write error and log messages to screen or wherever needed...
 *
 * NOTES:
 * Time of logging event is added to output. This time is taken from the
 * time function to retain compability backwards.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 * MODIFIED:
 * Øystein Godøy, met.no/FOU, 23.02.2004
 * Added LOGMSG and changed interface...
 */

#include <fmutil.h>

void fmerrmsg(char *where, char *what) {

    time_t wtime;
    struct tm *mytime;
    char ftime[20];

    wtime = time(NULL);
    mytime = localtime(&wtime);
    strftime(ftime,19,"%F %R",mytime);

    fprintf(stderr,"\n");
    fprintf(stderr," ERRMSG [%s %s]:\n", where, ftime);
    fprintf(stderr,"\t%s\n", what);
    fprintf(stderr," \n");
}

void fmlogmsg(char *where, char *what) {

    time_t wtime;
    struct tm *mytime;
    char ftime[20];

    wtime = time(NULL);
    mytime = localtime(&wtime);
    strftime(ftime,19,"%F %R",mytime);

    fprintf(stdout," \n");
    fprintf(stdout," LOGMSG [%s %s]:\n", where, ftime);
    fprintf(stdout,"\t%s\n", what);
    fprintf(stdout," \n");
}
