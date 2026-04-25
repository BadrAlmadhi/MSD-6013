#include <thread>
#include <chrono> // used for time 
#include <iostream>
#include <vector>

#include <omp.h> // OpenMP

/**
 * functions returns two things 
 * return {total_sum, elapsed_ms};
 * the sum and the time 
 */
// struct Data
template<typename T> 
struct Data {
    T sum;  // the result of the sum
    double time_ms;  // how long it took (in milliseconds)
};


// serial_sum
// generic functionn 
template<typename T>
// sum array , number of elements in the array
// function takes in array and return the sum
T serial_sum(T a[], size_t N) {
    T sum = 0;

    for (size_t i = 0; i < N; i++) {
        sum += a[i];
    }

    return sum;
}

// parallel_sum_std (part 1) C++ thread paralled reduction
template<typename T> // generic
Data<T> parallel_sum_std(T a[], size_t N, size_t num_threads) {
    // start timer
    auto start_time = std::chrono::high_resolution_clock::now(); 

    // each thread gets its own sum slot
    std::vector<T> partial_sumes(num_threads, 0);

    // store threads objects in vector array
    std::vector<std::thread> threads;

    // number if array elements per thread
    size_t chunk_size = N / num_threads;

    for (size_t t = 0; t < num_threads; t++) {
        size_t start_index = t * chunk_size;

        // last thread handles any leftover elements. 
        size_t end_index;
        if (t == num_threads - 1) {
            end_index = N;
        } else {
            end_index = start_index + chunk_size;
        }

        // create one thread
        threads.push_back(std::thread([&, t, start_index, end_index]() {
            T local_sum = 0;

            // this thread only sums its assigned part. 
            for (size_t i = start_index; i < end_index; i++) {
                local_sum += a[i];
            }

            // save this thread result
            partial_sumes[t] = local_sum;
        }));
    }

    // wait for all threads tto finish.
    for (size_t t = 0; t < threads.size(); t++) {
        threads[t].join();
    }

    // combine all partial sums 
    T total_sun = 0;
    for (size_t t = 0; t < num_threads; t++) {
        total_sun += partial_sumes[t];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();  

    return {total_sun, elapsed_ms};
}


// parallel_sum_omp1 (part 2): OpenMP custom parallel reduction
// This does not use OpenMP's built-in reduction.
// We manually create partial sums, like Part 1.
template<typename T>
Data<T> parallel_sum_omp1(T a[], size_t N, size_t num_threads) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // One sum slot for each OpenMP thread.
    std::vector<T> partial_sums(num_threads, 0);

    size_t chunk_size = N / num_threads;

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        size_t start_index = tid * chunk_size;

        size_t end_index;
        if ((size_t)tid == num_threads - 1) {
            end_index = N;
        } else {
            end_index = start_index + chunk_size;
        }

        T local_sum = 0;

        // This OpenMP thread sums only its assigned part.
        for (size_t i = start_index; i < end_index; i++) {
            local_sum += a[i];
        }

        // Safe because each thread writes to a different slot.
        partial_sums[tid] = local_sum;
    }

    // Combine all partial sums after the parallel region ends.
    T total_sum = 0;
    for (size_t t = 0; t < num_threads; t++) {
        total_sum += partial_sums[t];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    double elapsed_ms =
        std::chrono::duration<double, std::milli>(end_time - start_time).count();

    return {total_sum, elapsed_ms};
}

// parallel_sum_omp_build (part 3)
// parallel_sum_omp_builtin (part 3): OpenMP built-in reduction
// This version uses OpenMP's reduction feature.
// OpenMP automatically creates private sums and combines them safely.
template<typename T>
Data<T> parallel_sum_omp_builtin(T a[], size_t N, size_t num_threads) {
    auto start_time = std::chrono::high_resolution_clock::now();

    T total_sum = 0;

    #pragma omp parallel for num_threads(num_threads) reduction(+:total_sum)
    for (size_t i = 0; i < N; i++) {
        total_sum += a[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    double elapsed_ms =
        std::chrono::duration<double, std::milli>(end_time - start_time).count();

    return {total_sum, elapsed_ms};
}


void run_strong_scaling() {
    std::vector<size_t> thread_counts = {1, 2, 4, 8};
    std::vector<size_t> sizes = {1000000, 5000000, 10000000};

    std::cout << "\n===== STRONG SCALING TEST =====\n";

    for (size_t N : sizes) {
        std::vector<int> a(N, 1);
        int correct = serial_sum(a.data(), N);

        std::cout << "\nN = " << N << "\n";
        std::cout << "threads,std_thread_ms,omp_custom_ms,omp_builtin_ms\n";

        for (size_t threads : thread_counts) {
            Data<int> r1 = parallel_sum_std(a.data(), N, threads);
            Data<int> r2 = parallel_sum_omp1(a.data(), N, threads);
            Data<int> r3 = parallel_sum_omp_builtin(a.data(), N, threads);

            if (r1.sum != correct || r2.sum != correct || r3.sum != correct) {
                std::cout << "ERROR: incorrect sum\n";
            }

            std::cout << threads << ","
                      << r1.time_ms << ","
                      << r2.time_ms << ","
                      << r3.time_ms << "\n";
        }
    }
}

void run_weak_scaling() {
    std::vector<size_t> thread_counts = {1, 2, 4, 8};
    size_t work_per_thread = 1000000;

    std::cout << "\n===== WEAK SCALING TEST =====\n";
    std::cout << "threads,N,std_thread_ms,omp_custom_ms,omp_builtin_ms\n";

    for (size_t threads : thread_counts) {
        size_t N = work_per_thread * threads;

        std::vector<int> a(N, 1);
        int correct = serial_sum(a.data(), N);

        Data<int> r1 = parallel_sum_std(a.data(), N, threads);
        Data<int> r2 = parallel_sum_omp1(a.data(), N, threads);
        Data<int> r3 = parallel_sum_omp_builtin(a.data(), N, threads);

        if (r1.sum != correct || r2.sum != correct || r3.sum != correct) {
            std::cout << "ERROR: incorrect sum\n";
        }

        std::cout << threads << ","
                  << N << ","
                  << r1.time_ms << ","
                  << r2.time_ms << ","
                  << r3.time_ms << "\n";
    }
}



int main() {
    run_strong_scaling();
    run_weak_scaling();
    return 0;
}