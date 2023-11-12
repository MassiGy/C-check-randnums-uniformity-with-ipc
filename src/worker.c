#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>       
#include <string.h>

int main(void){

    int fd = shm_open(
        BACKING_FILE,       /* backing file pathname, */
        O_RDWR,             /* read write operations on backing file, creates it if none */
        0644                /* r/w permisssion for current user only */
    );
    if (fd < 0) {
      fprintf(stderr, "Backing file setup failure.\n");
      exit(-1);
    }

    // access the shared memory
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


    // decalre a local array using the lenght global variable.
    int shared_array_copy_len = ARRAY_LENGTH / sizeof(int);
    int *shared_array_copy = malloc(shared_array_copy_len * sizeof(int));

    int memset_status;
    // loop till counter = generations_count:
    for (int i =0; i < GENERATIONS_COUNT; i++) {
        // zero out the local array
        memset((void*)shared_array_copy, 0, ARRAY_LENGTH);

        // call generation_proc upon the local array
        rand_nums_freq_analyser(shared_array_copy,shared_array_copy_len, ROUNDS_PER_GENERATION);

        // wait the semaphore
        sem_wait(global_semaphore);

        // export the local array content to the shared one
        // we can just use memcpy since they share the same len
        dot not mem copy the current process results, since this means
        that we will only keep the last progression, I should rater 
        increment what was on the share array
        DOTNOTmemcpy(shared_array, shared_array_copy, ARRAY_LENGTH);

        // signal the semaphore
        sem_post(global_semaphore);

        // continue
    }


    // unmap the shared memory
    munmap(shared_array, ARRAY_LENGTH);

    // unlink the backing file
    shm_unlink(BACKING_FILE);

    // destroy/close the semaphore
    sem_close(global_semaphore);

    // return and exit
    return 0;
}



void rand_nums_freq_analyser(int* freqs_store,int freqs_store_len, int generation_cycles_count) {
    for (int i = 0; i < generation_cycles_count; i++) {
        int guess = rand() % freqs_store_len;
        freqs_store[guess]++;

        printf("guess:%d\n", guess);
    }
    return;
}

