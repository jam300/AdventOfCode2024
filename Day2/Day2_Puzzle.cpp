// Day2_Puzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <future>
#include <numeric>
#include <algorithm>

bool IsAscending(const std::vector<int>& list)
{
    return std::is_sorted(list.begin(), list.end());
}

bool IsDescending(const std::vector<int>& list)
{
    return std::is_sorted(list.rbegin(), list.rend());
}

bool ValidDifferences(const std::vector<int>& list) 
{
    const int MIN_DIFF = 1;
    const int MAX_DIFF = 3;

    return std::adjacent_find(list.begin(), list.end(), 
        [=](int a, int b) 
        {
            int diff = std::abs(a - b);
            return diff < MIN_DIFF || diff > MAX_DIFF;
        }) == list.end();
}

bool IsValidList(const std::vector<int>& list)
{
    return (IsAscending(list) || IsDescending(list)) && ValidDifferences(list);
}

bool Is_Safe_With_Dampener(const std::vector<int>& list)
{
    return std::any_of(list.begin(), list.end(), 
        [&list, index = 0](int) mutable
        {
            std::vector<int> modified_list = list;
            modified_list.erase(modified_list.begin() + index++);
            return IsValidList(modified_list);
        });
}

int Count_Safe_List(const std::vector<std::vector<int>>& lists)
{
    return std::count_if(lists.begin(), lists.end(),
        [](const std::vector<int>& list) 
        {
            return IsValidList(list) || Is_Safe_With_Dampener(list);
        });
}

int Async_IsSaveList(const std::vector<std::vector<int>>& lists, size_t start, size_t end)
{
    return std::count_if(lists.begin() + start, lists.begin() + end,
        [](const std::vector<int>& list)
        {
            return IsValidList(list) || Is_Safe_With_Dampener(list);
        });
}

int Async_Count_Safe_Lists(const std::vector<std::vector<int>>& lists)
{
    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t  block_size = (lists.size() + num_threads - 1 ) / num_threads;

    std::vector<std::future<int>> futures;

    for (size_t index = 0; index < num_threads; ++index)
    {
        size_t start = index * block_size;
        size_t end = std::min(start + block_size, lists.size());

        if (start < end)
        {
            futures.emplace_back(std::async(std::launch::async, Async_IsSaveList, std::ref(lists), start, end));
        }
    }
    int total_safe_count = 0;
    for (auto& future : futures) 
    {
        total_safe_count += future.get();
    }

    return total_safe_count;
}

std::vector<std::vector<int>> ReadFile(const std::string& filename)
{
    std::vector<std::vector<int>> complete_lists;
    try
    {
        std::ifstream file(filename);
        if (!file.is_open()) 
        {
            throw std::ios_base::failure("Error: No se pudo abrir el archivo.");
        }

        std::string line;
        while (std::getline(file, line)) 
        {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::vector<int> list_item;
            int value;
            while (iss >> value)
            {
                list_item.push_back(value);
            }

            if (!list_item.empty()) 
            {
                complete_lists.push_back(list_item);
            }
        }

        file.close();
    }
    catch (const std::ios_base::failure& e) {
        std::cerr << "Excepción de E/S: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Excepción genérica: " << e.what() << std::endl;
    }

    return complete_lists;
}

int main()
{
    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Dia2\\Input_Data.txt"; //replace with your own path
    std::vector<std::vector<int>> complete_lists = ReadFile(filename);

    if (complete_lists.empty())
    {
        std::cerr << "Error: the lists are empty." << std::endl;
        return 1;
    }
    int safe_count = 0;
    int async_safe_count = 0;

    safe_count = Count_Safe_List(complete_lists);
    async_safe_count = Async_Count_Safe_Lists(complete_lists);

    std::cout << "Save List : " << safe_count << std::endl;
    std::cout << "Save List  by async_safe_count : " << async_safe_count << std::endl;
}
