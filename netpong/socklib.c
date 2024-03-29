#include       <stdio.h>
#include       <sys/types.h>
#include       <sys/socket.h>
#include       <netinet/in.h>
//#include       <arpa/inet.h>
#include       <netdb.h>
#include       <strings.h>
#include       <unistd.h>

/*
 *	socklib.c
 *
 *	This file contains functions used lots when writing internet
 *	client/server programs.  The two main functions here are:
 *
 *	make_server_socket( portnum )	returns a server socket
 *					or -1 if error
 *
 *	connect_to_server(char *hostname, int portnum)
 *					returns a connected socket
 *					or -1 if error
 */ 

int make_server_socket( int portnum )
{
    struct  sockaddr_in   saddr;   /* build our address here */
	struct	hostent		*hp;   /* this is part of our    */
	char	hostname[256];         /* address 	         */
	int	sock_id;	       /* line id, file desc     */

       /*
        *      step 1: build our network address
        *               domain is internet, hostname is local host,
        *               port is some arbitrary number
        */
       gethostname( hostname , 256 );          /* where am I ?         */
        puts(hostname);
       hp = gethostbyname( hostname );         /* get info about host  */
       if ( hp == NULL )
		return -1;

       bzero( (char *)&saddr, sizeof(saddr) ); /* zero struct          */
                                               /* fill in hostaddr     */
       bcopy( (char *)hp->h_addr, (char *)&saddr.sin_addr, hp->h_length);
       saddr.sin_family = AF_INET ;            /* fill in socket type  */
       saddr.sin_port = htons(portnum);        /* fill in socket port  */

       /*
        *      step 2: ask kernel for a socket, then bind address
        */
       // printf("setting up socket\n");
       sock_id = socket( AF_INET, SOCK_STREAM, 0 );    /* get a socket */
       if ( sock_id == -1 ) return -1;
						       /* give it      */
       if ( bind(sock_id, (struct sockaddr *) &saddr, sizeof(saddr)) != 0 )
                return -1;				/* an address	*/

       /*
        *      step 3: tell kernel we want to listen for calls
        */

       if ( listen(sock_id, 1) != 0 ) return -1;
       printf("listening\n");
       return sock_id;
}

int connect_to_server( char *hostname, int portnum )
{
	struct sockaddr_in  servadd;        /* the number to call */
	struct hostent      *hp;            /* used to get number */
	int    sock_id;                     /* the socket         */

       /*
        *      build the network address of where we want to call
        */

       hp = gethostbyname( hostname );
       if ( hp == NULL ) return -1;

       bzero( (char *)&servadd, sizeof(servadd));  /* zero the address     */
       servadd.sin_family = AF_INET ;          /* fill in socket type  */
                                               /* and machine address  */
       bcopy( (char *)hp->h_addr, (char *)&servadd.sin_addr, hp->h_length);
//       servadd.sin_addr.s_addr = inet_addr("131.123.35.17");
       servadd.sin_port = htons(portnum);      /* host to num short    */

       /*
        *        make the connection
        */

       sock_id = socket( AF_INET, SOCK_STREAM, 0 );    /* get a line   */
       if ( sock_id == -1 ) return -1;          	/* or fail      */
       printf("connecting...\n");                                                /* now dial     */
       if( connect(sock_id,(struct sockaddr *)&servadd,sizeof(servadd)) != 0 )
               return -1;

       /*
        *      we're connected to that number, return the socket
        */
    printf("connected.\n");
	return sock_id ;
}
