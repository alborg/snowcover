/*
 * NAME:
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */

#include <fmutil.h>

/*
 * PURPOSE:
 * To chop of newline characters in text strings.
 */
void fmremovenewline(char *mystr) {

    int i;

    for (i=0; i<strlen(mystr); i++) {
	if (mystr[i] == '\n') mystr[i] = '\0';
    }
}

