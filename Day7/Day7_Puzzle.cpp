// Day7_Puzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <set>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <cstdio>

struct Equation
{
    long long targetValue;
    std::vector<int> numbers;
};

struct Key
{
    int index;
    long long currentResult;
    
    bool operator==(const Key& other) const
    {
        return index == other.index && currentResult == other.currentResult;
    }
};

struct KeyHash
{
    std::size_t operator()(const Key& k) const
    {
        return std::hash<int>()(k.index) ^ std::hash<long long>()(k.currentResult );
    }
};

bool FindRightOperation(const std::vector<int>& numbers, long long target, int index, long long currentResult, std::unordered_map<Key, bool, KeyHash>& memo)
{
    // if it bigger thant target it's pointless to continue 
    if (currentResult > target)
    {
        return false;
    }

    //base case
    if ( index == numbers.size())
    {
        return currentResult == target;
    }
    
    //memoization
    Key key = { index, currentResult };
    if (memo.find(key) != memo.end())
    {
        return memo[key];
    }

    bool result = false;

    // Add +
    if (FindRightOperation(numbers, target, index + 1, currentResult + numbers[index], memo)) 
    {
        result = true;
    }

    // multiplication *
    if (!result && FindRightOperation(numbers, target, index + 1, currentResult * numbers[index], memo)) 
    {
        result = true;
    }
    //part 2
    if (!result)
    {
        long long concatenatedValue = std::stoll(std::to_string(currentResult) + std::to_string(numbers[index]));
        if (FindRightOperation(numbers, target, index + 1, concatenatedValue, memo))
        {
            result = true;
        }
    }

    memo[key] = result;
    return result;
}

void ReadInputDataFile(const std::string& pathfile, std::vector<Equation>& equations)
{
    std::ifstream file(pathfile);

    try
    {
        if (!file.is_open())
        {
            throw std::ios_base::failure("Error: Unable to open file.");
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string part;

            std::getline(iss, part, ':');
            long long target = std::stoll(part); 

            std::vector<int> numbers;
            int num;
            while (iss >> num)
            {
                numbers.push_back(num);
            }

            equations.push_back({ target, numbers });
        }

        file.close();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main()
{
    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Day7\\Input_Data.txt";
    std::vector<Equation> equations;
    ReadInputDataFile(filename, equations);

    long long totalCount = 0;
    for (const auto& eq : equations)
    {   
        std::unordered_map<Key, bool, KeyHash> memo;
        if (FindRightOperation(eq.numbers, eq.targetValue, 1, eq.numbers[0], memo))
        {
            totalCount += eq.targetValue;
        }
    }

    std::cout << "Totalcount: " << totalCount << std::endl;
}


