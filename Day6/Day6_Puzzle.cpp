// Day6_Puzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
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


enum class Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

enum class MoveResult 
{
    SUCCESS,
    OBSTACLE,
    OUT_OF_BOUNDS
};

const  std::map<Direction, std::pair<int, int>> DirectionToDelta = {
    {Direction::UP, {-1, 0}},
    {Direction::RIGHT, {0, 1}},
    {Direction::DOWN, {1, 0}},
    {Direction::LEFT, {0, -1}}
};

const std::map<char, Direction> CharToDirection = {
    {'^', Direction::UP},
    {'>', Direction::RIGHT},
    {'v', Direction::DOWN},
    {'<', Direction::LEFT}
};

const std::map< Direction, char> DirectionToChar = {
   {Direction::UP, '^'},
   {Direction::RIGHT,'>'},
   {Direction::DOWN, 'v'},
   {Direction::LEFT, '<'}
};

struct PathNode
{
    int x; // for columns, be positive to right
    int y; // for rows, be positive to down 
    Direction direction;
    PathNode(int x, int y, Direction dir) : x(x), y(y), direction(dir) {}

    bool operator==(const PathNode& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator<(const PathNode& other) const
    {
        if (x != other.x) return x < other.x; 
        if (y != other.y) return y < other.y; 
        return direction < other.direction;
    }

    friend std::ostream& operator<<(std::ostream& os, const PathNode& node) 
    {
        os << "Posicion: (" << node.x << ", " << node.y << "), Direction: " << DirectionToChar.at(node.direction);
        return os;
    }
    
};

struct MapHandler
{
    std::vector<std::string> mapArea;

    MapHandler(const std::vector<std::string>& inputMap) : mapArea(inputMap){}

    int getMaxRows() const
    {
        return mapArea.size();
    }

    int getMaxCols() const
    {
        return mapArea.empty() ? 0 : mapArea[0].size();
    }

    bool IsOutOfBounds(int x, int y) const
    {
        return x < 0 || x >= getMaxRows() || y < 0 || y >= getMaxCols();
    }

    bool IsObstacle(int x, int y) const
    {
        return mapArea[x][y] == '#';
    }

    bool IsGuard(int x, int y) const
    {
        char cell = mapArea[x][y];

        if ((cell == '^' || cell == '>' || cell == 'v' || cell == '<'))
        {
            return true;
        }
        return false;
    }

    char GetCellAt(int x, int y) const
    {
        if (IsOutOfBounds(x, y))
        {
            throw std::out_of_range("Cell position out of bounds");
        }
        return mapArea[x][y];
    }

    PathNode FindGuard() const
    {
        for (int x = 0; x < getMaxRows(); ++x)
        {
            for (int y = 0; y < getMaxCols(); ++y)
            {
                if (IsGuard(x, y))
                {
                    return PathNode(x, y, CharToDirection.at(GetCellAt(x, y)));
                }
            }
        }

        throw std::runtime_error("Guard not found on the map!");
    }

    bool  AddObstacle(int x, int y)
    {
        char cell = mapArea[x][y];

        if (!IsGuard(x, y) && !IsOutOfBounds(x, y) && cell != '#')
        {
            mapArea[x][y] = '#';
            return true;
        }

        return false;
    }

    void PrintMap(int x, int y, char orientation)
    {
        std::vector<std::string> mapCopy = mapArea;
        auto initGuardPosition = FindGuard();
        mapCopy[initGuardPosition.x][initGuardPosition.y] = '.';

        if (!IsOutOfBounds(x, y)) 
        {
            mapCopy[x][y] = orientation;
        }

        std::cout << "\033[2J\033[H";

        for (const auto& row : mapCopy)
        {
            std::cout << row << "\n";
        }    
    }
};

struct Guard
{
    PathNode currentState;
    std::pair<int, int> delta;
    std::vector<PathNode> path;

    Guard(PathNode startNode)
        : currentState(startNode)
    {
        delta = DirectionToDelta.at(currentState.direction);
        path.push_back(currentState);
    }

    char GetCurrentDirection()
    {
        return DirectionToChar.at(currentState.direction);
    }

    std::vector<PathNode> GetCompletePath()
    {
        return path;
    }

    void Rotate90Degrees()
    {
        switch (currentState.direction)
        {
            case Direction::UP:
                currentState.direction = Direction::RIGHT;
                break;
            case Direction::RIGHT:
                currentState.direction = Direction::DOWN;
                break;
            case Direction::DOWN:
                currentState.direction = Direction::LEFT;
                break;
            case Direction::LEFT:
                currentState.direction = Direction::UP;
                break;
        }
        delta = DirectionToDelta.at(currentState.direction);
    }

    MoveResult  TryMove(MapHandler& mapHandler)
    {
        int nx = currentState.x + delta.first;
        int ny = currentState.y + delta.second;

        if (mapHandler.IsOutOfBounds(nx, ny))
        {
            return MoveResult::OUT_OF_BOUNDS;
        }

        if (mapHandler.IsObstacle(nx, ny)) 
        {
            return MoveResult::OBSTACLE;
        }

        currentState.x = nx;
        currentState.y = ny;

        path.push_back(currentState);

        return MoveResult::SUCCESS;
    }

    friend std::ostream& operator<<(std::ostream& os, const Guard& guard)
    {
        os << " posicion: (" << guard.currentState.x << ", " << guard.currentState.y << "), Direction: " << DirectionToChar.at(guard.currentState.direction);
        return os;
    }

};

int FindLoops(const std::vector<std::string>& originalMap,  const std::vector<PathNode>& path)
{
    std::vector<std::string> localFieldMap = originalMap;
    std::vector<PathNode> loops;

    for (const auto& node : path)
    {        
        MapHandler mapHandler(localFieldMap);
        Guard guard(mapHandler.FindGuard());

        if (mapHandler.AddObstacle(node.x, node.y))
        {
            std::set<PathNode> visited;
            visited.insert(guard.currentState);

            while (true)
            {
                //mapHandler.PrintMap(guard.currentState.x, guard.currentState.y, guard.GetCurrentDirection());
                //std::this_thread::sleep_for(std::chrono::milliseconds(200));

                MoveResult result = guard.TryMove(mapHandler);

                if (result == MoveResult::OUT_OF_BOUNDS)
                {
                    break;
                }

                if (result == MoveResult::OBSTACLE)
                {
                    guard.Rotate90Degrees();
                }

                if (visited.find(guard.currentState) != visited.end())
                {
                    loops.push_back(node);
                    break;
                }

                if (result == MoveResult::SUCCESS)
                {
                    visited.insert(guard.currentState);
                }
            }
        }
    }
    std::set<std::pair<int, int>> uniqueCoordinates;
    std::for_each(loops.begin(), loops.end(), [&uniqueCoordinates](const PathNode& node) {
        uniqueCoordinates.insert({ node.x, node.y });
        });

    return uniqueCoordinates.size();
}

void Simulate(MapHandler& mapHandler, Guard& guard, std::set<std::pair<int, int>>& visited)
{
    while (true)
    {
        //mapHandler.PrintMap(guard.currentState.x, guard.currentState.y, guard.GetCurrentDirection());
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
        MoveResult result = guard.TryMove(mapHandler);
        if (result == MoveResult::OUT_OF_BOUNDS)
        {
            break;
        }
        else if (result == MoveResult::OBSTACLE)
        {
            guard.Rotate90Degrees();
        }
        else if (result == MoveResult::SUCCESS)
        {
            visited.insert({ guard.currentState.x, guard.currentState.y });
        }
    }

    //mapHandler.PrintMap(guard.currentState.x, guard.currentState.y, ' ');
    //std::cout << "Guard exited the map!\n";
}

void ReadInputDataFile(const std::string& pathfile, std::vector<std::string>& mappedArea)
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
}

int main()
{
    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Day6\\Input_Data.txt";
    std::vector<std::string> mappedArea;

    ReadInputDataFile(filename, mappedArea);

    MapHandler mapHandler(mappedArea);
    Guard guard(mapHandler.FindGuard());

    std::set<std::pair<int, int>> visited;
    visited.insert({ guard.currentState.x, guard.currentState.y }); // insert initial position

    std::cout << "Initial posicion state: "<< guard << std::endl;

    Simulate(mapHandler, guard, visited);
    std::cout << "The guard will visit: " << visited.size() << " positions." << std::endl;

    auto completePath = guard.GetCompletePath();
    int loops = FindLoops(mappedArea, completePath);


    std::cout << "loops: " << loops << std::endl;
}

