#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>       
#include <string.h>

// @main
int main(void){

    int fd = shm_open(
        BACKING_FILE,       /* backing file pathname, */
        O_RDWR | O_CREAT,   /* read write operations on backing file, creates it if none */
        0644                /* r/w permisssion for current user only */
    );
    if (fd < 0) {
      fprintf(stderr, "Backing file setup failure.\n");
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
      fprintf(stderr, "Shared memory setup failure.\n");
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
      fprintf(stderr, "Shared semaphore setup failure.\n");
      exit(-1);
    }

    sem_wait(global_semaphore);
    memset(shared_array, 0, ARRAY_LENGTH);
    sem_post(global_semaphore);

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
                fprintf(stderr, "Worker exec after fork failure.\n");
                exit(-1);
            }
        }

        // @parent_process:
        // continue to fork the next sub_process
    }


    int standard_deviation_value;
    int interval_lower_bound;
    int interval_higher_bound;

    // init our standard deviation value
    // call uniformity_check upon the global array
    // int status = check_values_uniformity(
    //     interval_lower_bound,
    //     interval_higher_bound,
    //     standard_deviation_value,
    //     (int*)shared_array
    // );

    // print out the results
    // printf("uniformity_check_status is :%d\n", status);

    sleep(5);
    // print the shared array;
    sem_wait(global_semaphore);
    for (int i = 0; i < ARRAY_LENGTH/sizeof(int); i++) {
        printf("%i ",((int*)shared_array)[i]);
    }
    printf("\n");
    sem_post(global_semaphore);


    // unmap the shared memory
    munmap(shared_array, ARRAY_LENGTH);

    // unlink the backing file
    shm_unlink(BACKING_FILE);

    // free & destroy the semaphore
    sem_close(global_semaphore);

    // return and quit
    return 0;
}
        
