/*
 * CP431/CP631 Assignment 1 - Prime Gaps Finder
 * MPI program to find the largest gap between consecutive primes up to a number n (1 billion or 1 trillion)
 */

#include <mpi.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int id, p;
    unsigned long long n; // for cli input
    unsigned long long low_val, high_val; // range
    unsigned long long local_gap = 0; // local max gap
    unsigned long long local_p1 = 0, local_p2 = 0; // p1,p2 - primes for local max gap
    double elapsed_time; // for bechmarking

    mpz_t curr, next, prev, range_start, range_end, gap_temp; // GMP variables

    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);


    if (argc != 2) { // check for correct cli args
        if (id == 0) {
            printf("try running mpirun -np <num_processes> %s <upper_limit>\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    n = strtoull(argv[1], NULL, 10);

    MPI_Barrier(MPI_COMM_WORLD); // for becnhmarking 
    elapsed_time = -MPI_Wtime();

    low_val = 2 + id * (n - 1) / p;
    high_val = 1 + (id + 1) * (n - 1) / p; // work division

    // Initialize GMP variables
    mpz_init(prev);
    mpz_init(curr);
    mpz_init(next);
    mpz_init(range_start);
    mpz_init(range_end);
    mpz_init(gap_temp);

    mpz_set_ui(range_start, low_val); //lower range boundary
    mpz_set_ui(range_end, high_val); //upper range boundary

    mpz_nextprime(curr, range_start); // first prime >= low_val

    if (mpz_cmp(curr,range_end)<=0){
        while (1){ // can do while true as well...
            mpz_set(prev, curr); //store current primes

            mpz_nextprime(next,curr);
            if (mpz_cmp(next,range_end)>0){ // check if next prime is in range.
                break;
            } 

            mpz_sub(gap_temp,next,prev); // calculate gap
            unsigned long long gap = mpz_get_ui(gap_temp);
            if (gap > local_gap){
                //check if gap is larger than local max gap
                local_gap = gap;
                local_p1 = mpz_get_ui(prev);
                local_p2 = mpz_get_ui(next);
                //update the coresponding primes
            }

            mpz_set(curr,next); // move to next prime

        }
    }

    // Send the last prime of each process to the next process (except the last one)
    if (id < p - 1) {
        unsigned long long last_prime = mpz_get_ui(curr);
        MPI_Send(&last_prime, 1, MPI_UNSIGNED_LONG_LONG, id + 1, 0, MPI_COMM_WORLD);
    }

    // Receive and check gap with prev process's last prime
    if (id> 0){
        unsigned long long prev_lastp;
        MPI_Recv(&prev_lastp, 1, MPI_UNSIGNED_LONG_LONG, id - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        mpz_set_ui(curr, low_val);
        mpz_nextprime(curr,curr);

        if (mpz_cmp(curr,range_end)<=0){   // first prime in current proc's range
            unsigned long long firstp = mpz_get_ui(curr);
            unsigned long long boundary_gap = firstp - prev_lastp;

            if (boundary_gap > local_gap){ // check if gap is larger than local max
                local_gap = boundary_gap;
                local_p1 = prev_lastp;
                local_p2 = firstp;
            }
        }
    }

    //collect reslts to p0
    unsigned long long *col_gaps = NULL;
    unsigned long long *col_p1 = NULL;
    unsigned long long *col_p2 = NULL;

    if (id ==0) {
        col_gaps = (unsigned long long *)malloc(p * sizeof(unsigned long long));
        col_p1 = (unsigned long long *)malloc(p * sizeof(unsigned long long));
        col_p2 = (unsigned long long *)malloc(p * sizeof(unsigned long long));
    }

    MPI_Gather(&local_gap, 1, MPI_UNSIGNED_LONG_LONG, col_gaps, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_p1, 1, MPI_UNSIGNED_LONG_LONG, col_p1, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_p2, 1, MPI_UNSIGNED_LONG_LONG, col_p2, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

    //end benchmarking
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time += MPI_Wtime();

    // Find global max gap and corresponding primes
    if (id == 0) {
        unsigned long long global_gap = 0;
        unsigned long long global_p1 = 0, global_p2 = 0;
        
        for (int i = 0; i < p; i++) {
            if (col_gaps[i] > global_gap) {
                global_gap = col_gaps[i];
                global_p1 = col_p1[i];
                global_p2 = col_p2[i];
            }
        }
        printf("Largest prime gap up to %llu is %llu between primes %llu and %llu\n", n, global_gap, global_p1, global_p2);

        // save results for plotting
        FILE *fp = fopen("prime_gaps_results.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%llu %d %.6f\n", n, p, elapsed_time);
            fclose(fp);
        }
        printf("Time taken: %f seconds\n", elapsed_time);
        
        // Free allocated memory
        free(col_gaps);
        free(col_p1);
        free(col_p2);
    
    }
    // Clear GMP variables
    mpz_clear(curr);
    mpz_clear(prev);
    mpz_clear(next);
    mpz_clear(range_start);
    mpz_clear(range_end);
    mpz_clear(gap_temp);

    MPI_Finalize();
    return 0;
}