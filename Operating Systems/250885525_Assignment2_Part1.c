/*
	CS3307
	ASSIGNMENT 2
	PART 1

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
#include <pthread.h> 

//initializing z_sum to add the two numbers together
int z_sum = 0;

//THREAD FUNCTION --> adds integers together
void *thread_sum (void *num) {

	//initializing the number to be added as the one passed in
	int *num_to_add = (int*) num;

	//cummulatively adding numbers
	z_sum += *num_to_add;

	return 0;
} // ---- thread function done

//MAIN FUNCTION BEGINS
int main () {

	printf("\n");

	//initializing variables
	pid_t pid, pid2, pid3;
	int x=10, y=20, z=0;

	/*
		FIRST FORK BEING CALLED: CREATING FIRST CHILD
	*/
	pid=fork();

	if ( pid < 0 ) {// less than zero implies the fork was unsccuessful
		printf("\n fork unsuccessful /n");
		exit(1);
 	}//end of fork error check

	if ( pid > 0 ){ // IN PARENT -->

		printf("IN PARENT");
		printf("\n");

		wait(NULL); // waiting for child process to add x + y

		printf("Child completed. \n");
		printf("\n");

		printf("RETURN TO PARENT \n");

		printf("The value of z after child has executed is: z = %d \n", z);

		//BEGINNING THREAD WORK

		pthread_t thread_1, thread_2;

		printf("\n");
		printf("Parent waiting for thread to be complete... \n");

		if ( (pthread_create(&thread_1, NULL, thread_sum, NULL)) || (pthread_create(&thread_2, NULL, thread_sum, NULL)) )  {
			printf("ERROR: Creation of thread unsuccessful \n");
			exit(1);
		}

		pthread_create(&thread_1, NULL, thread_sum, &x);
		pthread_create(&thread_2, NULL, thread_sum, &y);

		pthread_join(thread_1, NULL);
		pthread_join(thread_2, NULL);

		// ---- Thread work done :)

		printf("Thread is complete. \n");
		printf("\n");

		//assigning z to the value of the sum calculate in the the thread function
		z = z_sum;

		printf("The value of z is: z = %d \n", z);
		printf("\n");

		printf("\n");
		printf("PROGRAM HAS FINISHED EXECUTING \n");
		printf("\n");

	} // ---- parent done

	if ( pid == 0 ) { //IN THE CHILD --> PERFORM OPTERATIONS
		printf("Parent is waiting for child to be complete... \n");
		printf("\n");

		printf("IN CHILD \n");
		printf("Instruction z = x + y will now be executed. \n");
		z = x + y;
		printf("\n");
	} // ----- end child


	return 0;


} // ---- end of main function



