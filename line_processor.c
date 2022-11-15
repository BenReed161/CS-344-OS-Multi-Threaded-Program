/* ******************************
 * Program Name: line_processor.c
 * By: Benjamin Reed
 * ONID: 933 889 437
 * Email: reedbe@oregonstate.edu
 * Desc: lineprocessor.c creates 4 seperate threads and follows this:
 * stdin --> input --> buffer1 --> line_processor --> buffer2 --> plus_sign --> buffer3  --> output -->stdout
 * The program follows the consumer-producer outlined in class
 * There are shared buffers between each of the threads that use the mutex locks to stop any race cases
 * The mutex locks also implement semaphores so that deadlocks do not occur in the programs and all the functions
 * wait for the locks to lift before attempting to call any get functions.
 ****************************** */ 

// SOURCES: Use of scripts given on assigment page

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int num_chars = 0;

//line input for user input option
char line_input[1000];

//pthreads
pthread_t input_id, output_id, plus_sign_id, line_separator_id;

// Buffer 1, shared resource between input thread and line-seperator thread
char buffer1[50000];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the line-seperator thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

// Buffer 2, shared resource between line-seperator thread and plus-sign thread
char buffer2[50000];
// Number of items in the buffer
int count_2 = 0;
// Index where the line-seperator will put the next item
int prod_idx_2 = 0;
// Index where the plus-sign thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 2, shared resource between plus-sign thread and output thread
char buffer3[50000];
// Number of items in the buffer
int count_3 = 0;
// Index where the plus-sign will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

/* ******************************
 * Function Name: get_buf_1
 * Input: NA
 * Output: int
 * Desc: Gets the next char from the buffer1
 ****************************** */ 
int get_buf_1(){
    pthread_mutex_lock(&mutex_1);
    while(count_1 == 0)
    pthread_cond_wait(&full_1, &mutex_1);
    char line_char = buffer1[con_idx_1];
    con_idx_1 = con_idx_1 + 1;
    count_1--;
    pthread_mutex_unlock(&mutex_1);
    return line_char;
}
/* ******************************
 * Function Name: set_buf_1
 * Input: char
 * Output: NA
 * Desc: Sets the buffer1 at the current
 * position to the char passed
 ****************************** */ 
void set_buf_1(char line_char){
    pthread_mutex_lock(&mutex_1);
    buffer1[prod_idx_1] = line_char;
    prod_idx_1 = prod_idx_1 + 1;
    count_1++;
    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
}
/* ******************************
 * Function Name: get_buf_2
 * Input: NA
 * Output: int
 * Desc: Gets the next char from the buffer2
 ****************************** */ 
int get_buf_2(){
    pthread_mutex_lock(&mutex_2);
    while(count_2 == 0)
    pthread_cond_wait(&full_2, &mutex_2);
    char line_char = buffer2[con_idx_2];
    con_idx_2 = con_idx_2 + 1;
    count_2--;
    pthread_mutex_unlock(&mutex_2);
    return line_char;
}
/* ******************************
 * Function Name: set_buf_2
 * Input: char
 * Output: NA
 * Desc: Sets the buffer2 at the current
 * position to the char passed
 ****************************** */ 
void set_buf_2(char line_char){
    pthread_mutex_lock(&mutex_2);
    buffer2[prod_idx_2] = line_char;
    prod_idx_2 = prod_idx_2 + 1;
    count_2++;
    pthread_cond_signal(&full_2);
    pthread_mutex_unlock(&mutex_2);
}
/* ******************************
 * Function Name: get_buf_3
 * Input: NA
 * Output: int
 * Desc: Gets the next char from the buffer3
 ****************************** */ 
int get_buf_3(){
    pthread_mutex_lock(&mutex_3);
    while(count_3 == 0)
    pthread_cond_wait(&full_3, &mutex_3);
    char line_char = buffer3[con_idx_3];
    con_idx_3 = con_idx_3 + 1;
    count_3--;
    pthread_mutex_unlock(&mutex_3);
    return line_char;
}
/* ******************************
 * Function Name: set_buf_3
 * Input: char
 * Output: NA
 * Desc: Sets the buffer3 at the current
 * position to the char passed
 ****************************** */ 
void set_buf_3(char line_char){
    pthread_mutex_lock(&mutex_3);
    buffer3[prod_idx_3] = line_char;
    prod_idx_3 = prod_idx_3 + 1;
    count_3++;
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
}

int char_num;
int line_nums;
int stopped = 0;
/* ******************************
 * Function Name: input
 * Input: void *
 * Output: void *
 * Desc: Takes the input from the user until a stop is reached
 * splits the input into whether a file is open or not
 ****************************** */ 
void *input(void *args){
    num_chars = 0;
    int i = 0;
    if(isatty(0)){
        while(i != 50){
            fgets(line_input, 1000, stdin);
            if(!strcmp(line_input, "STOP\n")){
                stopped = 1;
                //Concatitnates an invisible character to indicicate the end of buffer
                set_buf_1(27);
                _Exit(1);
                break;
            }
            for(int x = 0; x < strlen(line_input); x++){
                set_buf_1(line_input[x]);
                //num_chars++;
            }
            i++;
            //line_nums++;
        }
    }
    else{
        while(fgets(line_input, 1000, stdin) != NULL && i != 50) {    
            if(!strcmp(line_input, "STOP\n")){
                stopped = 1;
                set_buf_1(27);
                break;
            }
            for(int x = 0; x < strlen(line_input); x++){
                set_buf_1(line_input[x]);
                //num_chars++;
            }
            i++;
            //line_nums++;
        }
    }
    return NULL;
}
/* ******************************
 * Function Name: line_separator
 * Input: void * 
 * Output: void *
 * Desc: When the line seperator gets the input from the buffer
 * it sets the newlines to a space.
 ****************************** */ 
void *line_separator(void *args){
    char tmp;
    for(int i = 0; i < 50000; i++){
        tmp = get_buf_1();
        if(tmp == 27){
            set_buf_2(27);
            break;
        }
        if(tmp == '\n'){
            line_nums++;
            set_buf_2(' ');
            continue;
        }
        set_buf_2(tmp);
    }
    return NULL;
}
/* ******************************
 * Function Name: plus_sign
 * Input: void * 
 * Output: void * 
 * Desc: When the plus_sign function gets the input 
 * from the buffer it sets the plus signs in groups 
 * of two to a carrot. 
 ****************************** */ 
void *plus_sign(void *args){
    int plus_num = 0;
    char tmp;
    for(int j = 0; j < 50000; j++){
        tmp = get_buf_2();
        if(tmp == 27){
            set_buf_3(27);
            break;
        } 
        if(tmp == '+'){
            plus_num++;
            if(plus_num % 2 == 0){
                set_buf_3('^');
            }
        }
        else{
            set_buf_3(tmp);
            plus_num=0;
        }
        num_chars++;
    }
    return NULL;
}
/* ******************************
 * Function Name: output
 * Input: void *
 * Output: void * 
 * Desc: prints the output from the buffer 3
 * when it's recieved, every 80 lines it prints a newline
 * The function will also only print when 80 lines is reached
 ****************************** */ 
void *output(void *args){
    int j = 0;
    char tmp;
    for(int x = 0; x < 50; x++){
        for(int i = 0; i < 80; i++){
            tmp = get_buf_3();
            fputc(tmp, stdout);
        }
        fputc('\n', stdout);
    }
    return NULL;
}
/* ******************************
 * Function Name: main
 * Input: NA
 * Output: NA
 * Desc: creates the 4 seperate threads for each of the functions
 ****************************** */ 
int main(){
    
    pthread_create(&input_id, NULL, input, NULL);
    pthread_create(&line_separator_id, NULL, line_separator, NULL);
    pthread_create(&plus_sign_id, NULL, plus_sign, NULL);
    pthread_create(&output_id, NULL, output, NULL);

    pthread_join(input_id, NULL);
    pthread_join(line_separator_id, NULL);
    pthread_join(plus_sign_id, NULL);
    pthread_join(output_id, NULL);

    exit(1);
}