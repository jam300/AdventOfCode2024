// Puzzle1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <algorithm>
#include <chrono>
#include <cstdio> 
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <thread>
#include <vector>

template <typename Iterator>
int Calculate_Total_Distance(Iterator first_1, Iterator last_1, Iterator first_2)
{
    return std::accumulate(first_1, last_1, 0, 
        [first_2](int sum, int current_value) mutable
        {
            return sum + std::abs(current_value - *first_2++);
        });
}

template<typename Iterator>
int Parallel_Total_Distance(Iterator start_1, Iterator end_1, Iterator start_2)
{
    std::ptrdiff_t total_elements = std::distance(start_1, end_1);
    unsigned allowed_threads = std::thread::hardware_concurrency();
    if (allowed_threads == 0) allowed_threads = 2;

    std::ptrdiff_t block_size = (total_elements + allowed_threads - 1) / allowed_threads;

    std::vector<int> results(allowed_threads);
    std::vector<std::thread> threads(allowed_threads - 1);

    Iterator block_last_1;
    Iterator block_last_2;
    for (unsigned index = 0; index < allowed_threads - 1; index++)
    {
        block_last_1 = start_1;
        block_last_2 = start_2;

        std::advance(block_last_1, block_size);
        std::advance(block_last_2, block_size);
        
        threads[index] = std::thread([start_1, block_last_1, start_2, &results, index]() mutable
            {
                results[index] = Calculate_Total_Distance<Iterator>(start_1, block_last_1, start_2);
            });

        start_1 = block_last_1;
        start_2 = block_last_2;
    }

    results[allowed_threads - 1] = Calculate_Total_Distance<Iterator>(start_1, end_1, start_2);
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return std::accumulate(results.begin(), results.end(), 0);
}

template<typename Iterator>
int Async_Total_Distance(Iterator start_1, Iterator end_1, Iterator start_2)
{
    std::ptrdiff_t total_elements = std::distance(start_1, end_1);
    unsigned allowed_threads = std::thread::hardware_concurrency();
    if (allowed_threads == 0) allowed_threads = 2;

    std::ptrdiff_t block_size = (total_elements + allowed_threads - 1) / allowed_threads;

    std::vector<std::future<int>> futures;

    while ( start_1 != end_1)
    {
        Iterator block_end_1 = start_1;
        Iterator block_end_2 = start_2;

        std::ptrdiff_t remaining = std::distance(start_1, end_1);
        std::ptrdiff_t current_block_size = std::min<std::ptrdiff_t>(block_size, remaining);
        std::advance(block_end_1, current_block_size);
        std::advance(block_end_2, current_block_size);

        futures.push_back(std::async(std::launch::async, Calculate_Total_Distance<Iterator>, start_1, block_end_1, start_2));

        start_1 = block_end_1;
        start_2 = block_end_2;
    }

    int total_distance = 0;
    for( auto& fut : futures)
    {
        total_distance += fut.get();
    }
    return total_distance;
}

int Simple_calculate_total_distance(const std::vector<int>& left_list, const std::vector<int>& right_list)
{
    int total_distance = 0;
    for (size_t index = 0; index < left_list.size(); ++index)
    {
        total_distance += std::abs(left_list[index] - right_list[index]);
    }
    return total_distance;
}

void ReadFile(const std::string& filename, std::vector<int>& vector1, std::vector<int>& vector2)
{
    try
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::ios_base::failure("Error: can't open the file.");
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            int leftValue, rightValue;

            // Validar que se puedan leer ambos valores en la línea
            if (!(iss >> leftValue >> rightValue))
            {
                throw std::invalid_argument("Error: invalit format.");
            }

            // Guardar los valores en los vectores
            vector1.push_back(leftValue);
            vector2.push_back(rightValue);
        }

        file.close();
    }
    catch (const std::ios_base::failure& e)
    {
        std::cerr << "Exception of E/S: " << e.what() << std::endl;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Exception invalit argument: " << e.what() << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Generig exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknow error traying to read the file." << std::endl;
    }
}

int main()
{
    std::vector<int> left_sorted;
    std::vector<int> right_sorted;

    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Dia1\\Input_Data.txt"; //replace with your own path
    ReadFile(filename, left_sorted, right_sorted);

    if (left_sorted.empty() || right_sorted.empty())
    {
        std::cerr << "Error: the lists are empty." << std::endl;
        return 1;
    }

    if (left_sorted.size() != right_sorted.size())
    {
        std::cerr << "Error: the lists has differences sizes." << std::endl;
        std::cerr << "List 1 size: " << left_sorted.size() << ", List 2 size: " << right_sorted.size() << std::endl;
        return 1;
    }

    std::sort(left_sorted.begin(), left_sorted.end());
    std::sort(right_sorted.begin(), right_sorted.end());

    int total_distance = Simple_calculate_total_distance(left_sorted, right_sorted);
    int result_SimpleLambda = Calculate_Total_Distance(left_sorted.begin(), left_sorted.end(), right_sorted.begin());
    int result = Parallel_Total_Distance(left_sorted.begin(), left_sorted.end(), right_sorted.begin());
    int async_result = Async_Total_Distance(left_sorted.begin(), left_sorted.end(), right_sorted.begin());

    printf("Single-thread Total distance: %d \n", total_distance);
    printf("SimpleLambda Total distance: %d \n", result_SimpleLambda);
    printf("multi-thread total distance: %d \n", result);
    printf("async total distance: %d \n", async_result);


    //just for testing
    //std::vector<int> list1(2000000, 1);  // 2000 elements with value 1
    //std::vector<int> list2(2000000, 2);  // 2000 elements with value 2

    //auto start_single = std::chrono::high_resolution_clock::now();
    //int total_distance = Simple_calculate_total_distance(list1, list2);
    //auto end_single = std::chrono::high_resolution_clock::now();
    //auto duration_single = std::chrono::duration_cast<std::chrono::microseconds>(end_single - start_single).count();

    //auto start_SimpleLambda = std::chrono::high_resolution_clock::now();
    //int result_SimpleLambda = Calculate_Total_Distance(list1.begin(), list1.end(), list2.begin());
    //auto end_SimpleLambda = std::chrono::high_resolution_clock::now();
    //auto duration_SimpleLambda = std::chrono::duration_cast<std::chrono::microseconds>(end_SimpleLambda - start_SimpleLambda).count();

    //auto start_multi = std::chrono::high_resolution_clock::now();
    //int result = Parallel_Total_Distance(list1.begin(), list1.end(), list2.begin());
    //auto end_multi = std::chrono::high_resolution_clock::now();
    //auto duration_multi = std::chrono::duration_cast<std::chrono::microseconds>(end_multi - start_multi).count();

    //auto start_async = std::chrono::high_resolution_clock::now();
    //int async_result = Async_Total_Distance(list1.begin(), list1.end(), list2.begin());
    //auto end_async = std::chrono::high_resolution_clock::now();
    //auto duration_async = std::chrono::duration_cast<std::chrono::microseconds>(end_async - start_async).count();

    //printf("Single-thread Total distance: %d, Time: %lld microseconds\n", total_distance, duration_single);
    //printf("SimpleLambda Total distance: %d, Time: %lld microseconds\n", result_SimpleLambda, duration_SimpleLambda);
    //printf("multi-thread total distance: %d, time: %lld microseconds\n", result, duration_multi);
    //printf("async total distance: %d, time: %lld microseconds\n", async_result, duration_async);
}
