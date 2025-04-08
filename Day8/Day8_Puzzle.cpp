// Day8_Puzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <unordered_set>
#include <mutex>
#include <optional>

struct AntennaPosition
{
    int x;
    int y;

    bool operator==(const AntennaPosition& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct AntennaHash
{
    std::size_t operator()(const AntennaPosition& position)const
    {
        return std::hash<int>()(position.x) ^ (std::hash<int>()(position.y) << 1);
    }
};

std::vector<std::string> mappedArea;
using AntennaMap = std::unordered_map<char, std::vector<AntennaPosition>>;
using AntinodeSet = std::unordered_set<AntennaPosition, AntennaHash>;

double EuclideanDist(const AntennaPosition& p1, const AntennaPosition& p2)
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

bool IsInBoundaries(const AntennaPosition& position)
{
    return position.x >= 0 && position.x < mappedArea.size() && position.y >= 0 && position.y < mappedArea[0].size();
}

std::pair<std::optional<AntennaPosition>, std::optional<AntennaPosition>> GetAntinodes(const AntennaPosition& p1, const AntennaPosition& p2)
{
    const auto dist = EuclideanDist(p1, p2);
    const auto ux = (p2.x - p1.x) / dist;
    const auto uy = (p2.y - p1.y) / dist;

    std::optional<AntennaPosition> antinode1 = std::nullopt;
    std::optional<AntennaPosition> antinode2 = std::nullopt;

    AntennaPosition t1 = { p1.x - dist * ux, p1.y - dist * uy };
    AntennaPosition t2 = { p2.x + dist * ux, p2.y + dist * uy };

    if (IsInBoundaries(t1))
    {
        antinode1 = t1;
    }
    if (IsInBoundaries(t2))
    {
        antinode2 = t2;
    }

    return { antinode1, antinode2 };
 
}

void ProcessSignal(char signal, const std::vector<AntennaPosition>& positions, AntinodeSet& antinodes, std::mutex& antinodeMutex)
{
    for (size_t i = 0; i < positions.size(); ++i)
    {
        for (size_t j = i +1; j < positions.size(); ++j)
        {
            const auto& p1 = positions[i];
            const auto& p2 = positions[j];

            auto [antinode1, antinode2] = GetAntinodes(p1, p2);

            std::lock_guard<std::mutex> lock(antinodeMutex);
            if (antinode1) {
                antinodes.insert(*antinode1);
            }
            if (antinode2) {
                antinodes.insert(*antinode2);
            }

        }
    }
}

int CalculateAntinodes(const AntennaMap& antennaPositions)
{
    std::mutex antinodeMutex;
    std::vector<std::thread> threads;
    AntinodeSet antinodes;

    for (const auto& [signal, positions] : antennaPositions) 
    {
        threads.emplace_back(ProcessSignal, signal, std::cref(positions), std::ref(antinodes), std::ref(antinodeMutex));

    }

    for (auto& thread : threads) 
    {
        thread.join();
    }

    for (const auto& node: antinodes)
    {
        std::cout << "X: " << node.x << " Y: " << node.y << std::endl;
    }

    return antinodes.size();
}

void GetAllSignals(const std::vector<std::string>& mappedArea, AntennaMap& antennaPositions)
{
    for (int x = 0; x < mappedArea.size(); ++x)
    {
        for (int y = 0; y < mappedArea[0].size(); ++y)
        {
            char cell = mappedArea[x][y];
            if (cell != '.')
            {
                antennaPositions[cell].push_back({ x, y });
            }
        }
    }
}

void ReadInputDataFile(const std::string& pathfile, AntennaMap& antennaPositions)
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
            mappedArea.push_back(line);
        }

        file.close();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    GetAllSignals(mappedArea, antennaPositions);
}

int main()
{
    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Day8\\Input_Data.txt";    
    AntennaMap antennaPositions;
    ReadInputDataFile(filename, antennaPositions);

    int totalAntinodes = CalculateAntinodes(antennaPositions);
    std::cout << "Total unique antinodes: " << totalAntinodes << std::endl;
}

