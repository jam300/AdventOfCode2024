// Day4_Puzzle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

const std::string WORD = "XMAS";
const int WORD_LENGHT = WORD.length();

const std::vector<std::pair<int, int>> directions = {  //[dx, dy]clockwise dy= move through columns , dx = move through rows
    {0, 1},  // Right
    {1, 1},  // Diagonal-right-down
    {1, 0},  // Down
    {1, -1},  // Diagonal-left-down
    {0, -1},  // Left,
    {-1, -1}, // Diagonal-left-up
    {-1, 0},  // Up
    {-1, 1}   // Diagonal-right-up
};

bool IsValidDirection(int row, int columm, int dx, int dy, int row_size, int column_size)
{
    int endRow = row + dx * (WORD_LENGHT - 1);
    int endColumn = columm + dy * (WORD_LENGHT - 1);

    bool isValid = (endRow >= 0 && endRow < row_size && endColumn >=0 && endColumn < column_size);
    return isValid;
}

bool IsTargetWord(const std::string word)
{
    if (WORD == word )
    {
        return true;
    }
    return false;
}

bool CheckWord(const std::vector<std::string>& matrix, int row, int column, int dx, int dy)
{
    std::string formedWord;
    for (int i = 0; i < WORD_LENGHT; ++i)
    {
        int newRow = row + i * dx;
        int newColumn = column + i * dy;
        formedWord += matrix[newRow][newColumn];
    }
    if (IsTargetWord(formedWord))
    {
        return true;
    }
       

    return false;
}

int CountXMAS(const  std::vector<std::string>& matrix)
{
    const int ROWS = matrix.size();
    const int COLUMNS = matrix[0].size();
    int count = 0;

    for (int row = 0; row < ROWS; ++row)
    {
        for (int column = 0; column < COLUMNS; ++ column)
        {
            for (const auto& [dx, dy] : directions) //C++ 17 sintaxis 
            {
              if(IsValidDirection(row, column, dx, dy, ROWS, COLUMNS) && CheckWord(matrix, row, column, dx, dy))
              {
                  count++;
              }
             
            }
        }
    }

    return count;
}

bool IsValidXmasPAtterns(const std::vector<std::string>& matrix, int row, int columm, int row_size, int column_size)
{
    //valid center
    if (matrix[row][columm] != 'A')
    {
        return false;
    }

    //valid digonal limit
    if (row - 1 < 0 || row + 1 >= row_size || columm - 1 < 0 || columm + 1 >= column_size)
    {
        return false;
    }

    std::string firstDiagonal = { matrix[row - 1][columm - 1], matrix[row][columm], matrix[row + 1][columm + 1] };
    std::string secondDiagonal = { matrix[row - 1][columm + 1], matrix[row][columm], matrix[row + 1][columm - 1] };

    return (firstDiagonal == "MAS" || firstDiagonal == "SAM") && (secondDiagonal == "MAS" || secondDiagonal == "SAM");
}

int CountXMASPatterns(const std::vector<std::string>& matrix)
{
    int count = 0;
    const int ROWS = matrix.size();
    const int COLUMNS = matrix[0].size();

    for (int row = 0; row < ROWS; ++row)
    {
        for (int column = 0; column < COLUMNS; ++column)
        {
            if (IsValidXmasPAtterns(matrix, row, column, ROWS, COLUMNS))
            {
                count++;
            }
        }
    }
    return count;
}


std::vector<std::string>ReadFile(const std::string& pathfile)
{
    std::ifstream file(pathfile);
    std::vector<std::string> lines;
    
    try
    {
        if (!file.is_open())
        {
            throw std::ios_base::failure("Error: Unable to open file.");
        }

        std::string line;
        while (std::getline(file, line))
        {
            lines.push_back(line);
        }

        file.close();
        return lines;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main()
{
    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Day4\\Input_Data.txt";
    std::vector<std::string> matrix;

    matrix = ReadFile(filename);

    //std::vector<std::string> matrixTest = {
    //"MMMSXXMASM",
    //"MSAMXMSMSA",
    //"AMXSXMAAMM",
    //"MSAMASMSMX",
    //"XMASAMXAMM",
    //"XXAMMXXAMA",
    //"SMSMSASXSS",
    //"SAXAMASAAA",
    //"MAMMMXMMMM",
    //"MXMXAXMASX"
    //};

    if (matrix.empty())
    {
        std::cerr << "Error: the matrix is empty." << std::endl;
        return 1;
    }

    int xmasCount = CountXMAS(matrix);
    std::cout << "Count of XMAS: " << xmasCount << std::endl;

    int xmasPatternsCount = CountXMASPatterns(matrix);
    std::cout << "xmasPatternsCount: " << xmasPatternsCount << std::endl;

}
