// Puzzle_Day_3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <numeric>

int calculateTotal(const std::string& content)
{
    std::regex pattern(R"(mul\((\d{1,3}),(\d{1,3})\)|do\(\)|don\'t\(\))");
    std::sregex_iterator begin(content.begin(), content.end(), pattern);
    std::sregex_iterator end;

    bool enabled = true;
    int total = 0;

    for (auto it = begin; it != end; ++it) 
    {
        std::string match = it->str();

        if (match == "do()")
        {
            enabled = true; 
        }
        else if (match == "don't()") 
        {
            enabled = false;
        }
        else if (enabled && (*it)[1].matched && (*it)[2].matched)
        {
            int num1 = std::stoi((*it)[1].str());
            int num2 = std::stoi((*it)[2].str());
            total += num1 * num2;
        }
    }
    return total;
}

std::string ReadFile(const std::string& filePath)
{
    std::string stringFile;
    try
    {
        
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            throw std::ios_base::failure("Error: No se pudo abrir el archivo.");
        }

        stringFile = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    }
    catch (const std::ios_base::failure& e)
    {
        std::cerr << "Excepci�n de E/S: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Excepci�n gen�rica: " << e.what() << std::endl;
    }

    return stringFile;
    
}

int main()
{
    std::string filePath = "C:\\Users\\JMendez\\Documents\\AdventCode\\Dia3\\InputData.txt";
    std::string content = ReadFile(filePath);

    int result = calculateTotal(content);
    std::cout << "result: " << result << std::endl;
}
