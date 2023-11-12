#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>       
#include <string.h>

// set our global length variable
#define ARRAY_LENGTH  8

// set our global rounds_per_generation
#define ROUNDS_PER_GENERATION  10

// set our global generations_count
#define GENERATION_COUNT  1

// set our global/non-shared sub_process_count
#define WORKER_PROCESS_COUNT  1

// set our global inner process communication handler/context
// in our case, it will be the semaphore name
#define SEMAPHORE_NAME "shared_semaphore"

// setup our backing file
#define BACKING_FILE "/shMemEx"

int check_values_uniformity(
    int interval_lower_bound,
    int interval_higher_bound,
    int standard_deviation_value,
    int* values
);


