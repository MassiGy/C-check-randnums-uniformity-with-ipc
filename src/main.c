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
    sem_wait(global_semaphore);
    for (int i = 0; i < ARRAY_LENGTH/sizeof(int); i++) {
        printf("%i ",((int*)shared_array)[i]);
    }
    sem_post(global_semaphore);
    printf("]\n");


    // init our standard deviation value
    int standard_deviation_value;

    // call uniformity_check upon the global array
    float uniformity_percentage = check_values_uniformity(
        .005,
        .01,
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
    float sd,       
    float compareto_base,
    int* freqs_store,
    int freq_store_len
){

    // calc the sum of freqs
    int freqs_sum = 0;
    for (int i = 0; i < freq_store_len; i++) {
        freqs_sum+= freqs_store[i];
    }

    // calc the ratio of all vals freq by freqs_sum
    float* values_freq_ratio = malloc(freq_store_len * sizeof(float));
    for (int i = 0; i < freq_store_len; i++) {
        // cast to float by adding 0.0
        values_freq_ratio[i] = freqs_store[i]/(freqs_sum + 0.00);
        printf("values_freq_ratio[%d]=%f\n", i, values_freq_ratio[i]);
    }

    // check if the ration is between base-sd, base+sd
    int success_counter = 0;
    for (int i = 0; i < freq_store_len; i++) {
        if (values_freq_ratio[i] >= compareto_base - sd &&
            values_freq_ratio[i] <= compareto_base + sd) {

            // inc counter
            success_counter++;
        }
    }

    // free the allocated ressources
    free(values_freq_ratio);
    values_freq_ratio = NULL;

    // return the pourcentage of success
    return success_counter * 100 / (freq_store_len + 0.0); 
}



