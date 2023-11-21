#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/wait.h>       
#include <fcntl.h>       
#include <string.h>

// @main
int main(void){

    int fd = shm_open(
        BACKING_FILE,               /* backing file pathname, */
        O_RDWR | O_CREAT,           /* read write operations on backing file, creates it if none */
        S_IRUSR | S_IWUSR | S_IXUSR /* r/w permisssion for current user only */
    );

    if (fd < 0) {
      fprintf(stderr, "@Main: Backing file setup failure.\n");
      exit(-1);
    }

    /* allocate ARRAY_LENGTH bytes into shared memory */
    ftruncate(fd, ARRAY_LENGTH);

    // setup our shared memory
    void* shared_array = mmap(
        NULL,                   /* let the system figure out where to pu the shared mem*/
        ARRAY_LENGTH,           /* our shared meme will be an array of that many int  */
        PROT_READ | PROT_WRITE, /* read write protections */
        MAP_SHARED,             /* this make it a shared mem between diffrent processes */
        fd,                     /* no backing file, but need a file descriptor */
        0                       /* backing file offset. */
    );

    if (shared_array == NULL || shared_array == (void *)-1) {
      fprintf(stderr, "@Main: Shared memory setup failure.\n");
      exit(-1);
    }

    // setup up our semaphore using its name
    sem_t * global_semaphore = sem_open(
        SEMAPHORE_NAME,        /* this will be the name/id of our global semaphore */
        O_CREAT,               /* this will create the semaphore if needed */
        0644,                  /* this is the access parameters, r/w for current user only */
        1                      /* this is the initial value */
    );

    if (global_semaphore == NULL || (void*) global_semaphore == (void *)-1) {
      fprintf(stderr, "@Main: Shared semaphore setup failure.\n");
      exit(-1);
    }

    memset(shared_array, 0, ARRAY_LENGTH);

    // loop till counter = sub_process_count:
    int pid;
    for (int i = 0; i < WORKER_PROCESS_COUNT; i++) {

        // spawn a sub process
        pid = fork();

        // @sub_procss:
        if (pid == 0) {

            // exec/replace on top of child the ./worker program
            int status = execlp("./worker", "worker", NULL);
            if(status < 0){
                fprintf(stderr, "@Main: Worker exec after fork failure.\n");
                exit(-1);
            }
        }

        // @parent_process:
        // continue to fork the next sub_process
    }

    for (int i = 0; i < WORKER_PROCESS_COUNT; i++) {
        // if chld exit with error, ignore and continue
        wait(NULL);
    }
    // print the shared array;
    printf("\n[");
    for (int i = 0; i < ARRAY_LENGTH/sizeof(int); i++) {
        printf("%i ",((int*)shared_array)[i]);
    }
    printf("]\n");


    // init our standard deviation value
    int standard_deviation_value;

    // call uniformity_check upon the global array
    float uniformity_percentage = check_values_uniformity(
        (WORKER_PROCESS_COUNT * 1.0 / ARRAY_LENGTH) * ROUNDS_PER_GENERATION * GENERATIONS_COUNT,
        (int*)shared_array,
        ARRAY_LENGTH/sizeof(int)
    );

    // print out the results
    printf("uniformity_check_status is :%.2f\n", uniformity_percentage);


    // unmap the shared memory
    munmap(shared_array, ARRAY_LENGTH);

    // close i/o to the backing file
    close(fd);

    // unlink the backing file
    shm_unlink(BACKING_FILE);

    // free & destroy the semaphore
    sem_close(global_semaphore);
    sem_destroy(global_semaphore);

    // return and quit
    return 0;
}
        

float check_values_uniformity(
    float compareto_base,
    int* freqs_store,
    int freq_store_len
){

    // find the max and min of freqs_store
    int max = freqs_store[0];
    int min = freqs_store[0];

    for (int i = 1; i < freq_store_len; i++) {
        if (freqs_store[i] > max) {
            max = freqs_store[i];
        }
        if(freqs_store[i] < min){
            min = freqs_store[i];
        }
    }

    // calc the diffrence of the two
    int amplitude = max - min;

    // calc the poucentage that the amplitude 
    // makes regarding the values of the base
    float amp_pourcentage_of_base = (amplitude + 0.0)/ compareto_base;

    // return that pourcentage
    return amp_pourcentage_of_base;
}



