//CS4-53203: Systems Programming
//Name:Dakota Sicher
//Date:14Apr23
//SicherDakota-CS43203-pipex.txt\

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define  oops(m,x)   { perror(m); exit(x); }

int main(int ac, char **av)
{
   int   thepipe[2],       /* two file descriptors */
         newfd,            /* useful for pipes  */
         pid;           /* and the pid    */

   if ( ac < 3 ){
      fprintf(stderr, "usage: pipe cmd1 cmd2 ... cmdn\n");
      exit(1);
   }

   while((--ac) > 1){
      if ( pipe( thepipe ) == -1 )
         oops("Cannot get a pipe", 1);
      if ( (pid = fork()) == -1 )
         oops("Cannot fork", 2);
      ++av;
      //parent becomes next cmd
      //parrent writes to pipe, child reads from pipe
      //child continues to execute pipex
      if ( pid > 0 ){
         close(thepipe[1]);   //parent(pipex) does not write to pipe

         if ( dup2(thepipe[0], 0) == -1 ) //stdin -> read end of pipe
            oops("could not redirect stdin",3);

         close(thepipe[0]);   //stdin is duped, close pipe
      }
      else{
         close(thepipe[0]); //child(cmd) does not read from pipe
         if ( dup2(thepipe[1], 1) == -1 )    //stdout -> write end of pipe
            oops("could not redirect stdout", 4);
         close(thepipe[1]);      /* stdout is duped, close pipe */
         execlp( *av, *av, NULL);   //execute next cmd
         oops(*av, 4);
      }

   }
   //last cmd gets executed without forking or making a pipe and
   //redirecting output
   //input is from pipe created in last loop
   //output is to stdout
   ++av;
   execlp(*av, *av, NULL);
}


