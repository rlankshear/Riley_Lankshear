/*
	CS 3307
	ASSIGNMENT 1
	PART 1

	RILEY LANSKHEAR
	250885525
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>


int main () {
	pid_t pid, pid2, pid3;
	/*
		FIRST FORK BEING CALLED: CREATING FIRST CHILD
	*/

	pid=fork();

	if ( pid < 0 ) {// less than zero implies the fork was unsccuessful
		printf("\n fork unsuccessful /n");
		exit(1);
 	}

	if ( pid > 0 ){ // IN PARENT --> enforce wait here 
		//forcing the parent to wait
		wait(NULL);
		//calling the second fork to be created (creating child 1.1)
		pid2=fork();
		//error checking the child 1.1
		if ( pid2 < 0 ){
			printf("\n fork() unsuccessful \n");
			exit(1);
		}

		if( pid2 > 0 ){ //pid2 > 0 implies that we are in the parent so we will enforce wait
			wait(NULL);
		}

		if ( pid2 == 0 ) { //we are in the second child where we will make a system call to an external program
			printf("\n parent (PID %d) created child_2 (PID %d) \n", getpid(), getppid());
			printf("\n child_2 (PID %d) is calling an external program external_program.out and leaving child_2... \n", getpid());
			printf("\n");
			execl("external_program.out", NULL);

		}
	}

	/*
		CHILD 1
	*/

	if ( pid == 0 ) { //pid == 0 and we are in the child
   		printf("\n Parent process (PID %d) created child_1 (PID %d) \n", getppid(), getpid());

		/*
			SECOND FORK IS BEING CALLED: CREATING THE CHILD OF CHILD ONE (THE GRANDCHILD)
		*/

		pid3=fork();

		if ( pid3 < 0 ) { //UNSUCCESSFUL FORK()
			printf("\n fork() unsuccessful \n");
			exit(1);
		}

		if ( pid3 > 0 ) { // IN PARENT --> enforce wait
			wait(NULL);
		}

		if ( pid3 == 0 ) { // IN CHILD 1.1 (GRANDCHILD)
			printf("\n parent (PID %d) is waiting for child_1 (PID %d) to complete before creating child_2 \n", getppid(), getpid());
			printf("\n Child_1 (PID %d) created child_1.1 (PID %d) \n", getppid(), getpid());
			printf("\n child_1 (PID %d) is now completed \n", getpid());
		}
  	}

	return 0;
}
