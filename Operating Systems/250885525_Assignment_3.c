/*
        Riley Lankshear
        250885525
        CS3305
        Assignment 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>


/*
    GLOBALLY DEFINED VARIABLES
*/

int MAX_SIZE = 1024;
int transaction_count, deposit_count = 0, client_count, num_clients;
int overdraft_limit = -5000;
int account_count;
bool depositers_finished_flag = false;
int depositers_finished_count = 0;
int transaction_fee_bracket = 500;

//Defined for mutual exclusion
pthread_mutex_t lock;


/*

    -------------------------- CREATING STRUCTS --------------------------

*/

//struct for the an account+
typedef struct account_struct {
	//type will be an int where 1 = business and 0 = personal
	int type; //[2]
    int balance;
	char *account_id; //[0] 
	int transaction_limit; //[10]
    int transaction_count;
    bool transaction_excess_flag; 
	bool overdraft_option; //[13] --> 1 = yes, 0 = no
	//fees
	int deposit_fee; //[4] 
	int withdraw_fee; //[6] 
	int transfer_fee; //[8] 
	int overdraft_fee; //[14] 
	int transaction_fee; //[11] 
} account_object;

//Struct for an individual client
struct client_struct{
    int id;
    char string[256];
};

//initializing
struct client_struct *clients;
account_object *accounts;


/*

    -------------------------- STRUCTS FINSIHED --------------------------

*/


/*

    ************************************************************************************************
                                            HELPER FUNCTIONS
    ************************************************************************************************

*/

/*
    account_type_print
    ---> will take in a binary integer (corresponding to the type of account)
    ---> given that it will return a pointer to a character array to represent the string format
*/
char* account_type_print(int type){

    char *pointer;
    pointer = malloc(sizeof(char)*8);
    // 1 = business
    if (type == 1){
        pointer = "business";
    }
    else { //else, 0 = personal
        pointer = "personal";
    }
    return pointer;
}



/*
    account_type_discern
    ---> take the string (character array) and assign it to a binary value
*/
int account_type_discern(char a_type[]){
    if(a_type[0] == 'b')
        return 1;
    else if (a_type[0] =='p')
        return 0;
    else
        exit(-1);
}

/*
    account_overdraft_option
    ---> will take in a character array and assign the boolean to true if the account has an overdraft option 'Y' or false if not
*/
bool account_overdraft_option(char a_type[]){
    if(a_type[0] == 'Y')
        return true;
    else
        return false;
}

/*
    MAIN ACCOUNT FUNCTION
    account_breakdown
    ---> will take a line representing an account 
    ---> load all the applicable information about the account into an account object
    ---> return the account object to be stored in an array representing all accounts
*/
account_object account_breakdown(char line[]){

    char *token;
    int token_counter = 0;
    char *account_array[MAX_SIZE];

    token = strtok(line, " ");
    //tokenize the line and store it in a local array
    while (token!=NULL){
        account_array[token_counter] = malloc(strlen(line));
        strcpy(account_array[token_counter], token);
        token = strtok(NULL, " ");
        token_counter++;
    }

    account_object account;

    //filling in the account objects
    account.type = account_type_discern(account_array[2]);
    account.balance = 0;   
    account.account_id = account_array[0];
    account.overdraft_option = account_overdraft_option(account_array[13]);
    account.transaction_limit = atoi(account_array[10]);
    account.deposit_fee = atoi(account_array[4]);
    account.withdraw_fee = atoi(account_array[6]);
    account.transfer_fee = atoi(account_array[8]);
    account.overdraft_fee = atoi(account_array[14]);
    account.transaction_fee = atoi(account_array[11]);

    return account;

}

/*
    overdraft_id
    ---> assigns takes an amount and returns an id which corresponds to what overdraft bracket the account is in
    ---> id of 1 implies the balance is in the bracket of (-1 to -500 inclusive)
*/
int overdraft_id(int balance)
{
	int tmp_count = 0;
	balance = abs(balance);

	while(balance > transaction_fee_bracket){
        balance -= transaction_fee_bracket;
        tmp_count++;
    }	    
    tmp_count++;

    return tmp_count;
}

/*
    account_overdraft_option
    ---> calculate the amount of the fee for the overdraft using the amount to be withdrawn, accounts OD fee and the index of the account
*/
int transaction_overdraft_fee_calculator(int amount, int fee, int index){

    int i, total_fee, tmp_count, borrow, balance, overdraftID=0,tmpOverdraftID=0;

    balance = accounts[index].balance;

    //if the balance is greater than zero
    if (balance > 0){
    	balance -= amount;
    	overdraftID = overdraft_id(balance);
    	tmp_count = overdraftID;
        total_fee = tmp_count * fee;
        //if we substract the fee will it be a different OD bracket
        if ( overdraft_id( (balance-total_fee) ) != overdraftID) {
            //if it does then we need to add an additional amount
            total_fee += fee;
        } // -----> end inner if statement

        return total_fee;
    } // ----> end first if statement

    else { //else the balance is negative
    	//get current overdraftID
    	tmpOverdraftID = overdraft_id(balance);
    	//check
    	balance -= amount;
    	//get new overdraftID
    	overdraftID = overdraft_id(balance);
        //IF the two ids are not the same it means we have moved to another bracket 
    	if(tmpOverdraftID != overdraftID){
    		tmp_count = abs(tmpOverdraftID-overdraftID);
            total_fee = tmp_count * fee;
            //again, checking to see if the fee is applied will it move us to another bracket of OD
            if ( overdraft_id( (balance-total_fee) ) != overdraftID) {
                total_fee += fee;
            } // ----> end inner if statement 
            return total_fee;
    	} // ----> end if statement 
    	else {
    		tmp_count = tmpOverdraftID;

            return 0;
    	} //----> end else statement 
    } // ----> end outter else statement
} //------> END FUNCTION

/*

    ************************************************************************************************
                                        HELPER FUNCTIONS DONE
    ************************************************************************************************

*/



/*

    **********************************************************************************************************
    THIS NEXT SECTION IS THE THREE FUNCTIONS THAT REPRESENT THE TRANSACTIONS; WITHDRAWL, DEPOSIT, AND TRANSFER
    **********************************************************************************************************

*/


/*
    FUNCTION DEPOSIT_TRANSACTION --> carries out the deposit process
    ---------> the parameter transfer_transaction is essentially a boolean 0 = no 1 = yes as to 
                indicate whether it is being called in the transfer transaction or as it's own seperate operation 
*/

void deposit_transaction(char *line[], int transaction_flag, int account_count, int transfer_transaction){

   
    int i=0, deposit_amount_flag, deposit_account_flag, additional_fee=0, overdraft_cost = 0;

    //setting up my flags for accessing
    deposit_account_flag = transaction_flag;
    deposit_amount_flag = transaction_flag + 1;

    //FOR LOOP --> finds the correct account destination for the deposit
    for(i=0; i<account_count; i++){
        //IF --> the IDs are the same
        if(strcmp(accounts[i].account_id,line[deposit_account_flag]) == 0)
        {
            if ( transfer_transaction == 0 ) //this implies it is not being called from a transfer function
            {
                additional_fee = accounts[i].deposit_fee;
            }
            else { //not using deposit fee
                additional_fee = accounts[i].transfer_fee;
            }

            //IF statement to check to see if the transaction limit will be reached
            if (accounts[i].transaction_count+1 > accounts[i].transaction_limit)
            {
                accounts[i].balance -= accounts[i].transaction_fee;
            }

            //update the balance with the corresponding quantities
            accounts[i].balance += atoi(line[deposit_amount_flag]);
            accounts[i].balance -= additional_fee;
            accounts[i].balance -= overdraft_cost;
            accounts[i].transaction_count++;
        }// ---->end if statement 
    } // ----> end for loop
} // ----> END OF FUNCTION



/*
    FUNCTION WITHDRAW_TRANSACTION --> carries out the withdraw process
    ---------> the parameter transfer_transaction is essentially a boolean 0 = no 1 = yes as to 
                indicate whether it is being called in the transfer transaction or as it's own seperate operation 
    ---------> withdraw function also will check to see if the balance will dip below 0 or not
    ---------> If the account will go negative post withdrawl then it will check to see if overdraft is allowed for the account
    ---------> If overdraft is allowed, it will carry out the withdrawl and then call another function to calculate the added fees, if no overdraft it will set the account balance = 0
*/
void withdraw_transaction(char *line[], int transaction_flag, int account_count, int transfer_transaction){

    int i, withdraw_amount_flag, withdraw_account_flag, additional_fee; //additional_fee is either the withdraw fee or the transaction fee depending on where the withdraw function is being called
    //initializing the positions of the account destination and the amount to be transfered
    withdraw_account_flag = transaction_flag;

    if(transfer_transaction == 0){
        withdraw_amount_flag = transaction_flag + 1; 
    }
    else { //ELSE it is a transfer transaction and we will need to add to get the position of the a
        withdraw_amount_flag = transaction_flag + 2;
    }

    if (-atoi(line[withdraw_amount_flag]) >= overdraft_limit)
    {
        //FOR LOOP --> will check each account and find the matching id that corresponds to the transaction destination account
        for(i=0; i<account_count; i++){
            //IF --> there is a matching ID 
            if(strcmp(accounts[i].account_id,line[withdraw_account_flag]) == 0){
                //IF statement checks to see what fee should be applied
                if ( transfer_transaction == 0)
                {
                    additional_fee = accounts[i].withdraw_fee;
                }
                else 
                {
                    additional_fee = accounts[i].transfer_fee;
                }
                //IF statement checking to see if transaction limit will be reached
                if (accounts[i].transaction_count+1 > accounts[i].transaction_limit)
                {
                    accounts[i].balance -= accounts[i].transaction_fee;
                }
                //IF --> checking to see if we did the withdrawl option, whether or not the account balance will be greater than zero
                if ((accounts[i].balance - atoi(line[withdraw_amount_flag])) >= 0)
                {
                    //if the balance after the transaction is complete will be greater than zero, then carry out the withdrawl
                    accounts[i].balance -= atoi(line[withdraw_amount_flag]);
                    // ----> withdrawl fee being applied
                    accounts[i].balance -= additional_fee;
                } // --->end if statement
                //ELSE --> the account will turn negative and we must check if there is an overdraft option...
                else 
                {
                    // IF --> there is NOT an overdraft option, then we can set the accounts balance at zero 
                    if (accounts[i].overdraft_option == 0) 
                    {
                        accounts[i].balance = 0;
                    } // ---> end if statement
                    //ELSE --> the account has an overdraft option 
                    else 
                    {
                        //Now do the withdrawl process and additionally deduct the corresponding transaction fee
                        int overdraft_cost;
                        overdraft_cost = transaction_overdraft_fee_calculator(atoi(line[withdraw_amount_flag]), accounts[i].overdraft_fee, i);
                        accounts[i].balance -= atoi(line[withdraw_amount_flag]);
                        accounts[i].balance -= additional_fee;
                        accounts[i].balance -= overdraft_cost;
                    } // ---> end else statment
                } // ---> end else statment 
            } // ---> end if statement 
        } // ---> end for loop
    }  
}// ---> END FUNCTION


/*
    FUNCTION TRANSFER_TRANSACTION --> carries out the transfer process
    ---------> Transfer function just calls the deposit and withdraw function
*/
void transfer_transaction(char *line[], int transaction_flag, int account_count){

    int i, transaction_amount_flag, withdraw_account_flag, deposit_account_flag;

    transaction_amount_flag = transaction_flag + 3; //gets the location of the amount
    withdraw_account_flag = transaction_flag + 1; //gets location of the account to withdraw funds from and transfer to another
    deposit_account_flag = transaction_flag + 2; //gets location of the account which will be recieving transfer funds

    //for loop runs till it finds the matching account
    for(i=0; i<account_count; i++){
        if(strcmp(accounts[i].account_id,line[withdraw_account_flag]) == 0){
            withdraw_transaction(line, withdraw_account_flag, account_count, 1); //passing in 1 to indicate yes this a call from a transfer function 
        } //---> end if statement
    } //-----> end for loop
    
    //for loop runs till it finds the matching account
    i = 0;
    for(i=0; i<account_count; i++){
        if(strcmp(accounts[i].account_id,line[withdraw_account_flag]) == 0){
            deposit_transaction(line, deposit_account_flag, account_count, 1);
        } //---> end if statement
    } //-----> end for loop    
} // ----> END TRANSFER_TRANSACTION FUNCTION



/*

    **********************************************************************************************************
                                        TRANSACTION FUNCTIONS ARE DONE
    **********************************************************************************************************

*/


/*
    --------------------------------------------------------------------------------------------------------------------------------
                                        THREADING FUNCTION ----> banking_action
    ------> will tokenize the line passed in, then compare each token to the three letters; d,w,t to get the appropritae transaction
    ------> It will then call the appropriate transction
    --------------------------------------------------------------------------------------------------------------------------------

*/
void *banking_action(void *arg){

    int local_ID = *((int *) arg);
    int i;
    int token_counter = 0;
    char line[256];
    char *tmp_token_array[MAX_SIZE];
    
    //for loop will copy the string of the matching id that has been passed
    for (i=0; i<num_clients; i++){
        if (clients[i].id == local_ID){
            strcpy(line, clients[i].string);
        }
    }


    pthread_mutex_lock(&lock);  // ENTRY REGION
    char *token;
    token = strtok(line, " ");
    while (token!=NULL){
        tmp_token_array[token_counter] = malloc(sizeof(line)*15);
        strcpy(tmp_token_array[token_counter], token);
        token = strtok(NULL, " ");
        token_counter++;
    }
    pthread_mutex_unlock(&lock); // EXIT REGION



    //main for loop that will run figure out which transcation needs to occur; deposit, withdraw, or transfer 
    for(i=1;i<token_counter;i++)
    {
        if(*tmp_token_array[i] == 'd')
        {
            pthread_mutex_lock(&lock);  // ENTRY REGION
            deposit_transaction(tmp_token_array, i+1, account_count, 0);
            pthread_mutex_unlock(&lock); // EXIT REGION

        }//---> end if statement 
        else if(*tmp_token_array[i] == 'w')
        {
            pthread_mutex_lock(&lock);  // ENTRY REGION
            withdraw_transaction(tmp_token_array, i+1, account_count, 0);
            pthread_mutex_unlock(&lock); // EXIT REGION

        }//---> end else if
        else if(*tmp_token_array[i] == 't')
        {
            pthread_mutex_lock(&lock);  // ENTRY REGION
            transfer_transaction(tmp_token_array, i, account_count);
            pthread_mutex_unlock(&lock); // EXIT REGION
        } //--->end else if
    } //--->end for loop

    return 0;
} //---> END THREAD FUNCTION


//MAIN FUNCTION
int main() {
	
    char *token, *type_test = "type";
	char line_counter = 0;
	char *lineArray[MAX_SIZE*2];
	char *tmpLineArray[MAX_SIZE*2];
    char line[1024];

    //file pointer
    FILE * fp;
    FILE *output_file;

    //opening file
    fp = fopen("assignment_3_input_file.txt", "r");

    //quick error check
    if (!fp){
        printf("ERROR: unable to open the text file 'assignment_3_input_file.txt.txt' \n");
        exit(1);
    }

    //creating output file    
    output_file = fopen("assignment_3_output_file.txt", "a");
    // Check if the file can be opened
    if (!output_file)
    {
        fprintf(output_file, "ERROR: unable to open output_file.txt\n");
        exit(1);
    }

    /*
        WHILE LOOP --> will read the file and store into an array each line
    */
    while (fgets(line, sizeof(line), fp) != NULL) {
        lineArray[line_counter] = malloc(sizeof(line));
        tmpLineArray[line_counter] = malloc(sizeof(line));
        strcpy(lineArray[line_counter], line);
        line_counter++;
    }


    //initializing more variables
    int i, k=0, NUM_THREADS = 0;
    deposit_count = 0;
    client_count = 0;
    num_clients = 0;
    account_count = 0;


    //for loop to count how many accounts, depositers and clients there are
     for (i = 0; i < line_counter; i++)
    {
        if(lineArray[i][0] == 'a'){
            account_count++;
        }
        if(lineArray[i][0] == 'd'){
            deposit_count++;
        }
        if(lineArray[i][0] == 'c'){
            client_count++;
        }
    } 
    //num clients will be used for the multi threading
    num_clients = deposit_count + client_count;


    //creating new arrays 
    char *token_array[MAX_SIZE];
    char *account_test = "type";

    clients = malloc(num_clients * sizeof(struct client_struct));
    accounts = malloc(account_count * sizeof(account_object));

    //for loop fills in the client array which contains both depositers and clients as well as fills in the accounts array
    for (i = 0; i < line_counter; i++)
    {
        if (lineArray[i][0] == 'a')
        {
            accounts[i] = account_breakdown(lineArray[i]);
            //next few lines initialzes the transaction count as well as the flag to indicate whether we have reached the max limit
            accounts[i].transaction_count = 0;
            if(accounts[i].transaction_count >= accounts[i].transaction_limit){
                accounts[i].transaction_excess_flag = true; //1 --> fee will need to be applied 
            }
            else{
                accounts[i].transaction_excess_flag = false; //0 --> no fee needed yet
            }
        }
        else if((lineArray[i][0] == 'c') || (lineArray[i][0] =='d'))
        {
            struct client_struct client;
            client.id = k+1;
            strcpy(client.string, lineArray[i]);
            clients[k] = client;
            k++;
        }
    } //end main for loop 



    /*
        THREAD STUFF IS STARTING 
    */
    NUM_THREADS = num_clients;
    pthread_t threads[NUM_THREADS];

    //checking to see if the the mutex has failed
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }  

    i = 0;
    int err_thread;
    int *arg = malloc(sizeof(*arg));

    //FIRST FOR LOOP will run for the amount of depositers first to ensure that all deposits will be made before anything else
    for (i=0; i<(NUM_THREADS-client_count);i++){
    	arg = &clients[i].id;
    	err_thread = pthread_create(&threads[i], NULL, banking_action, arg);
    }
    //JOIN the first set of threads
    for (i=0; i<(NUM_THREADS-client_count);i++){
        pthread_join(threads[i], NULL);
    }

    //SECOND FOR LOOP will run for the remaining entries in clients array (which are all individual clients)
    for (i=deposit_count; i<NUM_THREADS;i++){
    	arg = &clients[i].id;
    	err_thread = pthread_create(&threads[i], NULL, banking_action, arg);
    }
    //JOIN the threads together 
    for (i=deposit_count; i<NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
    }

    /*
        PRINT OUT THE ACCOUNT INFORMATION
    */
    fprintf(output_file, "%s type %s %d \n", accounts[0].account_id, account_type_print(accounts[0].type), accounts[0].balance);
    fprintf(output_file, "%s type %s %d \n", accounts[1].account_id, account_type_print(accounts[1].type), accounts[1].balance);
    fprintf(output_file, "%s type %s %d \n", accounts[2].account_id, account_type_print(accounts[2].type), accounts[2].balance);
    fprintf(output_file, "%s type %s %d \n", accounts[3].account_id, account_type_print(accounts[3].type), accounts[3].balance);


    printf("%s type %s %d \n", accounts[0].account_id, account_type_print(accounts[0].type), accounts[0].balance);
    printf("%s type %s %d \n", accounts[1].account_id, account_type_print(accounts[1].type), accounts[1].balance);
    printf("%s type %s %d \n", accounts[2].account_id, account_type_print(accounts[2].type), accounts[2].balance);
    printf("%s type %s %d \n", accounts[3].account_id, account_type_print(accounts[3].type), accounts[3].balance);
    
    //closing files
    fclose(fp);
    fclose(output_file);

	return 0;
}

