/* who4.c - who with buffered reads
*  Identity crisis: The manual page for who lists who   am   i as an acceptable *usage of     
*   the command. It also lists whoami. Modify who2.c so it supports the who am i *usage. 
*   Experiment with the command whom and read the manage. How does it differ from who    
*   am i ? write a program that works like whoami.
*/
//Dakota Sicher
//Adding Who am i functionality
/* who2.c  - read /etc/utmp and list info therein
 *         - suppresses empty records
 *         - formats time nicely
 */
#include    <stdio.h>
#include    <unistd.h>
#include    <utmp.h>
#include    <fcntl.h>
#include    <time.h>
#include    <stdlib.h>
#include    <string.h>

#define      SHOWHOST

void showtime(long);
void show_info(struct utmp *);

int main(int ac, char* av[])
{
/*
after i added this to check for the "am i" i discoverd that the "who am i" actually 
works with any two non-option arguments with who, so i didn't neccessarily need to
check specifically for "am" or "i," just ac == 3 but i just left it in
*/
    int am_i = 0;
    if(ac == 3 && strcmp(av[1],"am")==0 &&
        (strcmp(av[2],"i")==0 || strcmp(av[2],"I")==0))
        am_i=1;

//    printf("am_i == %i\n",am_i);

    struct utmp     utbuf;          /* read info into here */
    int             utmpfd;         /* read from this descriptor */

    if ( (utmpfd = open(UTMP_FILE, O_RDONLY)) == -1 ){
        perror(UTMP_FILE);
        exit(1);
    }

    while( read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf) ){
        // if "am i" then only print the current user 
	if(am_i == 1){
            if(strcmp(getlogin(),utbuf.ut_name)==0) show_info(&utbuf);
        }
	//otherwise print everthing
        else show_info(&utbuf);
    }

    close(utmpfd);
    return 0;
}
/*
 *      show info()
 *                      displays the contents of the utmp struct
 *                      in human readable form
 *                      * displays nothing if record has no user name
 */
void show_info( struct utmp *utbufp )
{
        if ( utbufp->ut_type != USER_PROCESS )
                return;

        printf("%-8.8s", utbufp->ut_name);      /* the logname  */
        printf(" ");                            /* a space      */
        printf("%-8.8s", utbufp->ut_line);      /* the tty      */
        printf(" ");                            /* a space      */
        showtime( utbufp->ut_time );            /* display time */
#ifdef SHOWHOST
        if ( utbufp->ut_host[0] != '\0' )
                printf(" (%s)", utbufp->ut_host);/* the host    */
#endif
        printf("\n");                          /* newline      */
}

void showtime( long timeval )
/*
 *      displays time in a format fit for human consumption
 *      uses ctime to build a string then picks parts out of it
 *      Note: %12.12s prints a string 12 chars wide and LIMITS
 *      it to 12chars.
 */
{
        char    *cp;                    /* to hold address of time      */

        cp = ctime(&timeval);           /* convert time to string       */
                                        /* string looks like            */
                                        /* Mon Feb  4 00:46:40 EST 1991 */
                                        /* 0123456789012345.            */
        printf("%12.12s", cp+4 );       /* pick 12 chars from pos 4     */
}
