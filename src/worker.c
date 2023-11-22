#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>       
#include <string.h>
#include <stdint.h>


// declare a global state var
static unsigned __int128 state;

/* The state must be seeded with an odd value. */
void seed(unsigned __int128 seed)
{
	state = seed << 1 | 1;
}

uint64_t next(void)
{
	// GCC cannot write 128-bit literals, so we use an expression
	const unsigned __int128 mult =
		(unsigned __int128)0x12e15e35b500f16e << 64 |
		0x2e714eb2b37916a5;
	state *= mult;
	return state >> 64;
}

int main(void){

    int fd = shm_open(
        BACKING_FILE,               /* backing file pathname, */
        O_RDWR,                     /* read write operations on backing file, creates it if none */
        S_IRUSR | S_IWUSR | S_IXUSR /* r/w permisssion for current user only */
    );
    if (fd < 0) {
        fprintf(stderr, "@Worker: Backing file setup failure.\n");
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
      fprintf(stderr, "@Worker: Shared memory setup failure.\n");
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
      fprintf(stderr, "@Worker: Shared semaphore setup failure.\n");
      exit(-1);
    }

    // seed our Lehmer PRNG, with an arbitrary value
    seed(1191921817);


    // decalre a local array using the lenght global variable.
    int shared_array_copy_len = ARRAY_LENGTH / sizeof(int);
    int *shared_array_copy = malloc(shared_array_copy_len * sizeof(int));

    int memset_status;
    // loop till counter = generations_count:
    for (int i =0; i < GENERATIONS_COUNT; i++) {
        // zero out the local array
        memset((void*)shared_array_copy, 0, ARRAY_LENGTH);

        // call generation_proc upon the local array
        gen_and_register_rand(shared_array_copy,shared_array_copy_len, ROUNDS_PER_GENERATION);

        // wait the semaphore
        sem_wait(global_semaphore);

        // export the local array content to the shared one
        for (int i = 0; i < shared_array_copy_len; i++) {
            ((int *)shared_array)[i] += shared_array_copy[i];
        }

        // signal the semaphore
        sem_post(global_semaphore);

        // continue
    }

    // unmap the shared memory
    munmap(shared_array, ARRAY_LENGTH);

    // free the allocated ressources
    free(shared_array_copy);
    shared_array_copy = NULL;
    
    // close i/o to the backing file
    close(fd);

    // close the semaphore
    sem_close(global_semaphore);

    // return and exit
    return 0;
}



void gen_and_register_rand(int* freqs_store,int freqs_store_len, int generation_cycles_count) {
    /* here you can change the values generation */
    int guess;
    for (int i = 0; i < generation_cycles_count; i++) {
        guess = next() % freqs_store_len;
        freqs_store[guess]++;

        // printf("guess:%d\n", guess);
    }
    return;
}

