#include "config.h"
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

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
    //
    // loop till counter = generations_count:
    //
    //     zero out the local array
    //     call generation_proc upon the local array
    //
    //     post the semaphore
    //     export the array content to the share one
    //     signal the semaphore
    //
    //     continue
    


    // unmap the shared memory
    munmap(shared_array, ARRAY_LENGTH);

    // unlink the backing file
    shm_unlink(BACKING_FILE);

    // destroy/close the semaphore
    sem_close(global_semaphore);

    // return and exit
    return 0;
}





