#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int MAX_SIZE = 10000;

/*
    SHORTEST JOB FIRST STRUCT
    ---> used so that we can keep track of the index of the process as well as its process value 
*/
typedef struct sjf_struct {
    int index;
    int value;
} sjf_object;

/*
    ROUND ROBIN STRUCT
*/
typedef struct rr_struct {
    int index;
    int value;
    int processID;
    int run_time;

} rr_object;

/*
    waiting_times_cummulative method
    ---> used to add the cummulative processes together
    ---> also used to do the average
*/
int waiting_times_cummulative(int array_value, int cummulative_process_time){
        cummulative_process_time += array_value;
        return cummulative_process_time;
}

/*
    waiting_times_total_avg
    ---> sums up all the numbers
    ---> used to calculate the total average of processes   
*/
int waiting_times_total_avg(int total_avg, int cummulative_process_time){
        total_avg += cummulative_process_time;
        return total_avg;
}

/*
    waiting_times_print 
    ---> prints out the waiting times for prisoners  
*/
void waiting_times_print(int array_value, int index, int cummulative_process_time, int total_avg, FILE *output_file){
    fprintf(output_file, "p%d = %d \n", index, cummulative_process_time);
}

/*
    average_calculation 
    ---> calculates the average   
*/
int average_calculation(int total_avg, int cummulative_process_time, int num){
    total_avg -= cummulative_process_time;
    total_avg = total_avg / num;
    return total_avg;
}

/*
  COMPARATOR_SJF 
  ---> Used to compare two pointers for the qsort function   
*/
int comparator_sjf(const void *p, const void *q) 
{ 
    struct sjf_struct *x = (struct sjf_struct *)p;
    struct sjf_struct *y = (struct sjf_struct *)q;

    //if else if statements to descern what to return for the qsort function
    if ((*x).value > (*y).value)
        return 1;
    else if ((*x).value < (*y).value)
        return -1;
    else
        return 0;
} 



/*
*
*
*       MAIN METHODS STARTING HERE
*
*
*/

/*
    ROUND ROBIN
*/ 
void round_robin(char **arr_rr, int p_counter, int tq, FILE *output_file){
 
    int i,j, rr_counter = 0, tail_pointer, done;
    


    rr_object RR_Array[p_counter];
    rr_object copy_array[p_counter];


    /*
        FOR LOOP
        ---> will fill in the RR_Array list with the information into struct format
        ---> do the same thing for a temporary array
        ---> and calculate how much total memory will be needed once round robin has been called
    */
    for(i=0;i<p_counter;i++){
        //storing info into struct format
        RR_Array[i].index = i;
        RR_Array[i].value = atoi(arr_rr[i]);
        RR_Array[i].processID = i+1;
        //storing info into strucr format for temporary array
        copy_array[i].index = i;
        copy_array[i].value = atoi(arr_rr[i]);
        copy_array[i].processID = i+1;

        //printf("PROCESS ID FOR RR_ARRAY = %d \n", RR_Array[i].processID);



        //IF ELSE STATEMENT --> here to count how many process will be needed for size of array requirements
        if ( (RR_Array[i].value < tq) || (RR_Array[i].value == tq) ) {
            rr_counter++;            
        }//end if statement
        else {
            rr_counter++;
            while(!(RR_Array[i].value<=tq)){
                RR_Array[i].value -= tq;
                rr_counter++;
            }//---> end while loop
        } //---> end else statment

    } //---> end for loop

    //creating the temporary array of size rr_counter which will encompass any 
    rr_object tmpArray[rr_counter+1];
    rr_object finished_array[p_counter];

    //for loop that will fill in the temporary array with the pre round robin array 
    for(i=0; i<p_counter; i++){
        tmpArray[i].index = i;
        tmpArray[i].value = atoi(arr_rr[i]);
        tmpArray[i].processID = i+1;
    }

    //initializing pointer for the tail (first empty space)
    tail_pointer = p_counter;
    //boolean variable for the while loop
    done = 0;
    i = 0;
    int difference, running_time=0, jj = 0;
    //while not true 
    while (done != 1){

        //if the tail_pointer or i is equal to rr_counter then we have reached then absolute end of the RR array and therefore we are done
        if ( (tail_pointer > rr_counter) || (i >= rr_counter))
            done = 1;
        else { 
          //  difference = tmpArray[i].value-tq;

        if ( i < p_counter)
            tmpArray[i].run_time = running_time;
            
//if the value of the process is greater than the tq value then we have to make adjustments
            if (tmpArray[i].value > tq){

                running_time+=tq;
        

                tmpArray[i].index = i;
                tmpArray[tail_pointer].value = tmpArray[i].value-tq;
        tmpArray[tail_pointer].run_time = tmpArray[i].run_time;
               //printf("Temp array [i] value = %d \n", tmpArray[tail_pointer].value);

                tmpArray[tail_pointer].index = tail_pointer;
                tmpArray[tail_pointer].processID = tmpArray[i].processID;
                //printf("Processing ID = %d  Tail pointer = %d \n ", tmpArray[i].processID, tmpArray[tail_pointer].processID);
                //increment pointers for next insertion
                tail_pointer++;
                i++;
            }//--> end if statement 

            else { //else tmpArray[i] is LESS THAN the time quantum value
        running_time += tmpArray[i].value;
                
        finished_array[jj].value = running_time;
                finished_array[jj].run_time = tmpArray[i].run_time;
                finished_array[jj].processID = tmpArray[i].processID;

        jj++;
                i++;

            } //-->end second outer else statement            
        }//--> end outer else statement 
    } // --> end while loop

fprintf(output_file, "\n");
    

    fprintf(output_file, "\nOrder of selection by CPU: \n");
    for (i=0; i<rr_counter;i++){
        fprintf(output_file, "P%d ", tmpArray[i].processID);
    }

//printing out the turnaround times of the functions
fprintf(output_file, "\n\n");

    int finished_time = 0, start_time = 0, tmp_time = 0, turnaround_time = 0;

    for ( int k = 0; k < p_counter; k++){

        fprintf(output_file, "p%d = %d \n", finished_array[k].processID, finished_array[k].value - finished_array[k].run_time);
    }

    fprintf(output_file, "\n\n");

}//--> end round robin method method




/*
    SHORTEST JOB FIRST    
*/
void shortest_job_first(char **array_queue, int p_counter, FILE *output_file){

    //Initializing variables
    int tmpArray[p_counter];
    sjf_object process_objects[p_counter];
    int i, j;
    int tmp;
    int total_avg = 0, cummulative_process_time = 0;
    float average_process_time = 0;

    //fill in temporary array
    for (i=0; i<p_counter;i++){
        tmpArray[i] = atoi(array_queue[i]);
    }//end for loop

    //fill in struct array
    for (i=0; i<p_counter;i++){
        process_objects[i].value = atoi(array_queue[i]);
        process_objects[i].index = i;
    }//end for loop

    //using in built c function qsort to sort the array
    qsort(process_objects, p_counter, sizeof(process_objects[0]), comparator_sjf);



    fprintf(output_file, "\nOrder of selection by CPU: \n");

    //for loop for printing out the order slection of the CPU
    for (i=0; i<p_counter; i++){
        fprintf(output_file, "p%d ", process_objects[i].index + 1);
    }

    fprintf(output_file, "\n\nIndividual waiting times for each process: \n");
    //for loop for printing out the individual waiting times of each process
    //implementing sort algorithm of O(n^2) 
    for (i =0; i < p_counter; i++){
        for (j = i+1; j < p_counter; j++){
            if (tmpArray[j] < tmpArray[i]){
                //rearranging values in a temporary holder
                tmp = tmpArray[i];
                tmpArray[i] = tmpArray[j];
                tmpArray[j] = tmp; 
            }// ---> end if statement
        } //---> end for loop 2
        //calling all sumation and average calculations
        //printing average 
        waiting_times_print(process_objects[i].index+1, i+1, cummulative_process_time, total_avg, output_file);
        cummulative_process_time = waiting_times_cummulative(tmpArray[i], cummulative_process_time);
        total_avg = waiting_times_total_avg(total_avg, cummulative_process_time);
    } // ---> end for loop 1
    fprintf(output_file, "\nAverage waiting time = %0.1f \n", (double)average_calculation(total_avg, cummulative_process_time, i));
} //---> end shortest job first algorithm



/*
    FIRST COME FIRST SERVE --> 
*/

void first_come_first_serve(char **array_queue, int p_counter, FILE *output_file){

    //initializing variables 
    int i, total_avg = 0, cummulative_process_time = 0;
    float average_process_time = 0;

    //fprintf(output_file, "\nOrder of selection by CPU: \n");
    fprintf(output_file, "\nOrder of selection by CPU: \n");

    //first come first serve CPU selection is easy, just in normal order
    for (i=0; i < p_counter; i++) {
        fprintf(output_file, "p%d ", i+1);
    } //---> end of for loop

    //fprintf(output_file, "\nOrder of selection by CPU: \n");
    fprintf(output_file, "\n\nIndividual waiting times for each process: \n");
    for (i = 0; i<p_counter; i++){
        //calling all sumation and average calculations
        //printing average 
        waiting_times_print(atoi(array_queue[i]), i+1, cummulative_process_time, total_avg, output_file);
        cummulative_process_time = waiting_times_cummulative(atoi(array_queue[i]), cummulative_process_time);
        total_avg = waiting_times_total_avg(total_avg, cummulative_process_time);
    } //---> end of for loop
    //printing average wait time
    fprintf(output_file, "\nAverage waiting time = %0.1f \n", (double)average_calculation(total_avg, cummulative_process_time, i));
} //---> end of first come first serve function



/*
    
    MAIN FILE 
    ---> Where the reading of the file will take place
    ---> Storing lines in an array
    ---> Tokenizing lines
    ---> calling various scheduling functions

*/
int main() {
    
    //initializing
    char *token;
    char *lineArray[MAX_SIZE];
    int line_counter = 0;
    char line[1024];

    //file pointer
    FILE * fp;
    FILE *output_file;

    //opening file
    fp = fopen("cpu_scheduling_input_file.txt", "r");

    //quick error check
    if (!fp){
        printf("ERROR: unable to open the text file 'cpu_scheduling_input_file.txt' \n");
        exit(1);
    }

    //creating output file    
    output_file = fopen("cpu_scheduling_output_file.txt", "a");
    // Check if the file can be opened
    if (!output_file)
    {
        fprintf(output_file, "ERROR: unable to open cpu_scheduling_output_file.txt\n");
        exit(1);
    }


    /*
        WHILE LOOP --> will read the file and store into an array each line
    */
    while (fgets(line, sizeof(line), fp) != NULL) {
        lineArray[line_counter] = malloc(sizeof(line));
        strcpy(lineArray[line_counter], line);
        line_counter++;
    }

    //initializing more variables
    size_t i, j, k, tq, queue_number;
    int p_counter, process_counter, token_counter;

    //creating new arrays 
    char *token_array[MAX_SIZE];
    char *processArray[MAX_SIZE];

    for (i = 0; i < line_counter; i++){
        p_counter = 0;
        //FOR LOOP --> running through the each line in the array and counting how many processes there are
        for (j =0; j < strlen(lineArray[i]); j++){
            if(lineArray[i][j] == 'p'){
                p_counter++;
            }
        } //end for loop for checking how many processes there are

        token_counter = 0;
        //setting up tokenizer
        token = strtok(lineArray[i], " ");

        //while token is not null split the lines up into tokens and store them into the array token array 
        do {

            token_array[token_counter] = malloc(strlen(lineArray[i]));
            strcpy(token_array[token_counter], token);
            token = strtok(NULL, " ");
            token_counter++;
        } while (token != NULL);

        //getting the time quantum of each line 
        tq = atoi(token_array[3]);
        process_counter = 0;

        //calculating how many processes there are and creating a new array to contain just the processes
        for (k = 5; k < token_counter; k = k + 2) {
            processArray[process_counter] = (char*)malloc(strlen(token_array[k]) +1);
            strcpy(processArray[process_counter], token_array[k]);
            process_counter++;
        }



        /*
            READ FOR CALLING THE SCHEUDLING PROCESSES
        */

        fprintf(output_file, "\n\nReady Queue %lu Applying FCFS Scheduling:\n", i+1);
        first_come_first_serve(processArray, process_counter, output_file);

        fprintf(output_file, "\n\nReady Queue %lu Applying FCFS Scheduling:\n", i+1);
        shortest_job_first(processArray, process_counter, output_file);

        fprintf(output_file, "\n\nReady Queue %lu Applying RR Scheduling:\n", i+1);
        round_robin(processArray, process_counter, tq, output_file);

    } //end main for loop 



    //closing files
    fclose(fp);
    fclose(output_file);

    return 0;

}//end main function

