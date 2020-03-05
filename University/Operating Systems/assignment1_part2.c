/*
	CS3307
	ASSIGNMENT 1
	PART 2

	Riley Lankshear
	250885525
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>


int main( int argc, char *argv[] ){

	//Error check to ensure user has entered an x and y value
	if ( (argc < 3) || (argc > 3) ) {
		printf("Error: Incorrect number of arguments provided \n");
		exit(1);
	}

	void integerCheck(char check[]){

		int i=0;
		if (check[0] == '-')
			i=1;
		for(; check[i] !=0; i++){
			if (!isdigit(check[i])) {
				printf("Error: Not an integer \n");
				exit(1);
			}
		}
	}

	integerCheck(argv[1]);
	integerCheck(argv[2]);

	//INITIALIZING VARIABLES
 	char c[sizeof(argv[2])];
  	int fd[2];
  	pid_t pid;

	//CALLING THE PIPE ON THE FD
	pipe(fd);

	//IF PIPE IS LESS THAN ZERO THEN THE PIPE HAS NOT BEEN SUCCESSFUL
  	if (pipe(fd) < 0){
    		perror("pipe error");
    		exit(0);
  	}

	//CALLING THE FORK TO CREATE PARENT AND CHILD 
  	pid = fork();

	//IF PIF IS LESS THAN ZERO THEN THE FORK HAS MADE AN ERROR 
	if (pid < 0) {
    		perror("fork error");
    		exit(0);
 	 }



 	 if(pid > 0) { //IF PID IS GREATER THAN ZERO WE ARE IN THE PARENT AND WE CAN CARRY OUT SOME PROCESSES

		printf("A pipe is created for communication between parent (PID %d) and child \n", getpid());
 	  	//reading in the x value from the command line
		printf("Parent (PID %d) reading x = %s from the user \n", getpid(), argv[1]);
		printf("parent (PID %d) reading Y from the pipe \n", getpid());

		//reading in the y value from the child process
    		read (fd[0],c,sizeof(c));
		//printing the addition of x and y values, but first converting them form a string into an interger so operations can be performed
 	   	printf("Parent (PID %d) adding X + Y = %d \n", getpid(), ( atoi(c) + atoi( (argv[1]) ) ) );

  	}

	else { //ELSE WE ARE IN THE CHILD


	printf("parent (PID %d) created a child (PID %d) \n", getppid(), getpid());
	//Getting the y value from the command prompt
	printf("child (PID %d) Reading Y = %s from the user \n", getpid(), argv[2]);
	printf("child (PID %d) Writing Y into the pipe \n", getpid());

	//writing the y value into the pipe so that it can be accessed by the parent 
    	write (fd[1],argv[2], sizeof(argv[2]));
  	}

	return 0;

}
