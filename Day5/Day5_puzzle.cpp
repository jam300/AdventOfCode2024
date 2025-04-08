// Day5_puzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <future>
#include <iostream>
#include <regex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include "Day5_puzzle.h"

std::pair<int, int> ParserRules(const std::string& line)
{
    std::regex pattern(R"(\d{2})");
    std::sregex_iterator begin(line.begin(), line.end(), pattern);
    std::sregex_iterator end;
    int first = 0;
    int second = 0;
    int count = 0;

    for (auto it = begin; it != end && count < 2; ++it, ++count)
    {
        int value = std::stoi(it->str());
        if (count == 0)
        {
            first = value;
        }
        else 
        {
            second = value;
        }

    }
    return { first, second };
}

std::vector<int> ParseUpdates(const std::string& line)
{
    std::regex pattern(R"(\d{2})");
    std::sregex_iterator begin(line.begin(), line.end(), pattern);
    std::sregex_iterator end;
    std::vector<int> tmp_vec;
    tmp_vec.reserve(std::distance(begin, end));

    for (auto it = begin; it != end; ++it)
    {
        tmp_vec.push_back(std::stoi(it->str()));
    }
    return tmp_vec;
}

void ReadInputData(const std::string& filename, std::unordered_map<int, std::vector<int>>& graphRules, std::vector<std::vector<int>>& updatesNumbers)
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
            if (line.empty() || line.find_first_not_of(" \t") == std::string::npos)
            {
                continue;
            }

            if (line.find('|') != std::string::npos)
            {
                auto rule = ParserRules(line);
                graphRules[rule.first].push_back(rule.second);
            }
            else
            {
                updatesNumbers.push_back(ParseUpdates(line));
            }
        }
        file.close();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Generig exception: " << e.what() << std::endl;
    }
}

bool IsValidUpdate(const std::vector<int>& update, const std::unordered_map<int, std::vector<int>>& graph)
{
    std::unordered_set<int> processed;
    for (int page : update)
    {
        if (graph.find(page) != graph.end())
        {
            for (int dependecy : graph.at(page))
            {
                if (processed.find(dependecy) != processed.end())
                {
                    return false;
                }
            }
        }
        processed.insert(page);
    }

    return true;
}

int SumMiddlePage(std::vector<std::vector<int>>& updatesNumbers, std::unordered_map<int, std::vector<int>>& graphRules, std::vector<std::vector<int>>& invalidUpdates)
{
    int totalMiddlePages = 0;
    for (const auto& update: updatesNumbers)
    {
        if (IsValidUpdate(update, graphRules))
        {
            int middleIndex = update.size() / 2;
            totalMiddlePages += update[middleIndex];
        }
        else
        {
            invalidUpdates.push_back(update);
        }
    }
    return totalMiddlePages;
}

int AsycIsValidUpdated(std::vector<std::vector<int>>& updatesNumbers, std::unordered_map<int, std::vector<int>>& graphRules, size_t start, size_t end)
{
    int localTotalMiddlePages = 0;
    std::mutex mutex;

    for (std::size_t index = start; index < end; ++index)
    {
        if (IsValidUpdate(updatesNumbers[index], graphRules))
        {
            int middleIndex = updatesNumbers[index].size() / 2;
            localTotalMiddlePages += updatesNumbers[index][middleIndex];
        }
        else
        {

        }
    }
    return localTotalMiddlePages;
}

int Async_SumMiddlePage(std::vector<std::vector<int>>& updatesNumbers, std::unordered_map<int, std::vector<int>>& graphRules)
{
    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t  block_size = (updatesNumbers.size() + num_threads - 1) / num_threads;

    std::vector<std::future<int>> futures;

    for (size_t index = 0; index < num_threads; ++index)
    {
        size_t start = index * block_size;
        size_t end = std::min(start + block_size, updatesNumbers.size()); 

        if (start < end)
        {
            futures.emplace_back(std::async(std::launch::async, AsycIsValidUpdated, std::ref(updatesNumbers), std::ref(graphRules), start, end));
        }
    }
    int totalMiddlePages = 0;
    for (auto& future : futures) 
    {
        totalMiddlePages += future.get();
    }

    return totalMiddlePages;
}

std::unordered_map<int, int> calculateGraphDegrees(std::unordered_map<int, std::vector<int>>& graphRules)
{
    std::unordered_map<int, int> graphDegrees;

    for (const auto& [node, neighbors] : graphRules) // C++17 syntax 
    {

        if (graphDegrees.find(node) == graphDegrees.end())
        {
            graphDegrees[node] = 0;
        }
        for (int neighbor : neighbors)
        {
            graphDegrees[neighbor]++;
        }
    }
    return graphDegrees;
}

std::vector<int> KahnTopologicalSort(const std::vector<int>& update, const std::unordered_map<int, std::vector<int>>& graphRules, std::unordered_map<int, int>& inDegreesOriginal)
{
    std::unordered_map<int, int> inDegrees = inDegreesOriginal; // Copy to avoid modifying the original map
    std::queue<int> readyQueue;
    std::vector<int> sortedOrder;

    std::unordered_set<int> updateSet(update.begin(), update.end());

    for (int page : update) 
    {
        if (inDegrees.find(page) != inDegrees.end() && inDegrees[page] == 0)
        {
            readyQueue.push(page);
        }
    }

    while (!readyQueue.empty())
    {
        int current = readyQueue.front();
        readyQueue.pop();
        sortedOrder.push_back(current);

        if (graphRules.find(current) != graphRules.end())
        {
            for (int neighbor : graphRules.at(current))
            {
                if (inDegrees.find(neighbor) != inDegrees.end())
                {
                    inDegrees[neighbor]--;
                    if (inDegrees[neighbor] == 0 && updateSet.find(neighbor) != updateSet.end())
                    {
                        readyQueue.push(neighbor);
                    }
                }
                
            }
        }
    }
    return sortedOrder;
}

int main()
{
    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Day5\\inputdata_Test.txt";
    std::unordered_map<int, std::vector<int>> graphRules;
    std::vector<std::vector<int>> updatesNumbers;
    std::vector<std::vector<int>> invalidUpdates;  // for part 2
    ReadInputData(filename, graphRules, updatesNumbers);
    int total = SumMiddlePage(updatesNumbers, graphRules, invalidUpdates);
    std::unordered_map<int, int> inDegrees = calculateGraphDegrees(graphRules);


    for (const auto& update : invalidUpdates)
    {
        try {
            auto sortedUpdate = KahnTopologicalSort(update, graphRules, inDegrees);
            std::cout << "Orden corregido: ";
            for (int page : sortedUpdate) 
            {
                std::cout << page << " ";
            }
            std::cout << std::endl;
        }
        catch (const std::exception& e) 
        {
            std::cout << "Error procesando la actualización: " << e.what() << std::endl;
        }
    }

 }
