
// set our global length variable
#define ARRAY_LENGTH  4096

// set our global rounds_per_generation
#define ROUNDS_PER_GENERATION  1000000

// set our global generations_count
#define GENERATIONS_COUNT  1000

// set our global/non-shared sub_process_count
#define WORKER_PROCESS_COUNT  6

// set our global inner process communication handler/context
// in our case, it will be the semaphore name
#define SEMAPHORE_NAME "shared_semaphore"

// setup our backing file
#define BACKING_FILE "/shMemEx"


/*
 * This function aims at generating
 * random values and registring it
 * in the freqs_store array.
 *
 * The values are int(s).
 *
 * Notice that the function is agnostic
 * of the values upper and lower bounds,
 * and it is up to the developer/maintainer
 * to change this function to adapt to 
 * diffrent intervals. Just follow the 
 * signature and make sure that your 
 * values gets registered in the freqs_store
 * before returning from this function.
 * 
 */
void gen_and_register_rand(
    int* freqs_store,
    int freqs_store_len, 
    int generation_cycles_count
);

/*
 * This function will calculate the 
 * the max and the min of all the 
 * freq, and then the diffrence of 
 * these two.
 *
 *
 * By projecting the value of this
 * diffrence upon the compareto_base
 * we get the poucentage of the peak
 * deviation.
 *
 *
 * Example: 
 * say that our base is 100
 * and the max freq is 110
 * and the min freq is 98
 *
 * So the diff is 12,
 * and 12 makes 12% of 100.
 *
 * So this function will return .12
*/
float check_values_uniformity(
    float compareto_base,
    int* freqs_store,
    int freq_store_len
);


