
// set our global length variable
#define ARRAY_LENGTH  4096

// set our global rounds_per_generation
#define ROUNDS_PER_GENERATION  100000

// set our global generations_count
#define GENERATIONS_COUNT  10

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
* DISCLAIMER: This is not backed by any real math
* just out of the top of my head, but seams fairly 
* right!
*
* Steps:
* check_values_uniformity will help us check if all 
* our frequences are uniformly spread.
*
* The idea is simple, take all the frequencies, calc
* the sum of all freqs which equals to how many vales
* are generated.
*
* Then, for each value, devide its freq by the sum of 
* frequencies.
*
* Check if the devision result is between compareto_base
* - sd and compareto_base + sd for each value freq.
* 
* if in, inc counter. Then at the end, return conter*100/len
* to get the pourcentage of uniformity.
*
*
* Notice that this function is agnostic of the random 
* generation process and do not care at all of the lower
* and upper bound of the generated values interval. That 
* is up to you the developer/maintainer to change the 
* generation function and this one to adapt them. Keep 
* in mind that the soul purpose of the project is to learn
* IPC following the POSIX standards.
*
*/
float check_values_uniformity(
    float standard_deviation_value,       
    float compareto_base,
    int* freqs_store,
    int freq_store_len
);


