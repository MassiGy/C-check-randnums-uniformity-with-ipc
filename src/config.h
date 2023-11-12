
// set our global length variable
#define ARRAY_LENGTH  8

// set our global rounds_per_generation
#define ROUNDS_PER_GENERATION  10

// set our global generations_count
#define GENERATIONS_COUNT  1

// set our global/non-shared sub_process_count
#define WORKER_PROCESS_COUNT  2

// set our global inner process communication handler/context
// in our case, it will be the semaphore name
#define SEMAPHORE_NAME "shared_semaphore"

// setup our backing file
#define BACKING_FILE "/shMemEx"


// int get_random(int seed, int uppper_bound, int lower_bound);

void rand_nums_freq_analyser(int* freqs_store,int freqs_store_len, int generation_cycles_count);


int check_values_uniformity(
    int interval_lower_bound,
    int interval_higher_bound,
    int standard_deviation_value,
    int* values
);


