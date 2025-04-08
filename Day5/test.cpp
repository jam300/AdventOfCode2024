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
    for (const auto& update : updatesNumbers)
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



// Funcion para construir el grafo a partir de las reglas
std::unordered_map<int, std::vector<int>> buildGraph(const std::vector<std::pair<int, int>>& rules) {
    std::unordered_map<int, std::vector<int>> graph;
    for (const auto& rule : rules) {
        graph[rule.first].push_back(rule.second);
    }
    return graph;
}

// Funcion para calcular los grados de entrada de cada nodo
std::unordered_map<int, int> calculateInDegrees(const std::unordered_map<int, std::vector<int>>& graph) {
    std::unordered_map<int, int> inDegrees;
    for (const auto& [node, neighbors] : graph) {
        if (inDegrees.find(node) == inDegrees.end()) {
            inDegrees[node] = 0; // Inicializar nodos con grado 0
        }
        for (int neighbor : neighbors) {
            inDegrees[neighbor]++;
        }
    }
    return inDegrees;
}

// Funcion para imprimir los grados de entrada
void printInDegrees(const std::unordered_map<int, int>& inDegrees) {
    std::cout << "Grados de entrada por nodo:" << std::endl;
    for (const auto& [node, degree] : inDegrees) {
        std::cout << "Nodo " << node << ": " << degree << std::endl;
    }
}

// Algoritmo de Kahn para calcular el orden topologico
std::vector<int> kahnTopologicalSort(const std::vector<int>& update,
    const std::unordered_map<int, std::vector<int>>& graph) {
    std::unordered_map<int, int> inDegrees = calculateInDegrees(graph);
    std::queue<int> ready;

    // Insertar nodos con grado 0 en la cola
    for (int page : update) {
        if (inDegrees[page] == 0) {
            ready.push(page);
        }
    }

    std::vector<int> sortedOrder;

    while (!ready.empty()) {
        int current = ready.front();
        ready.pop();
        sortedOrder.push_back(current);

        // Reducir el grado de entrada de los vecinos
        for (int neighbor : graph.at(current)) {
            inDegrees[neighbor]--;
            if (inDegrees[neighbor] == 0 && std::find(update.begin(), update.end(), neighbor) != update.end()) {
                ready.push(neighbor);
            }
        }
    }

    // Verificar si el orden incluye todos los nodos del update
    if (sortedOrder.size() != update.size()) {
        throw std::runtime_error("El grafo contiene un ciclo o nodos no procesables.");
    }

    return sortedOrder;
}

int main() {
    // Ejemplo de reglas y actualizaciones
    std::vector<std::pair<int, int>> rules = {
        {47, 53}, {97, 13}, {97, 61}, {97, 47}, {75, 29},
        {61, 13}, {75, 53}, {29, 13}, {97, 29}, {53, 29},
        {61, 53}, {97, 53}, {61, 29}, {47, 13}, {75, 47},
        {97, 75}, {47, 61}, {75, 61}, {47, 29}, {75, 13}, {53, 13}
    };

    std::vector<std::vector<int>> updates = {
        {75, 47, 61, 53, 29},
        {97, 61, 53, 29, 13},
        {75, 29, 13},
        {75, 97, 47, 61, 53},
        {61, 13, 29},
        {97, 13, 75, 29, 47}
    };

    // Construir el grafo
    auto graph = buildGraph(rules);

    // Calcular y mostrar los grados de entrada
    auto inDegrees = calculateInDegrees(graph);
    printInDegrees(inDegrees);

    // Procesar cada actualización
    for (const auto& update : updates) {
        try {
            auto sortedUpdate = kahnTopologicalSort(update, graph);
            std::cout << "Orden corregido: ";
            for (int page : sortedUpdate) {
                std::cout << page << " ";
            }
            std::cout << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Error procesando la actualización: " << e.what() << std::endl;
        }
    }

    return 0;
}

//int main()
//{
//    std::string filename = "C:\\Users\\JMendez\\Documents\\AdventCode\\Day5\\inputdata_Test.txt";
//    std::unordered_map<int, std::vector<int>> graphRules;
//    std::vector<std::vector<int>> updatesNumbers;
//    std::vector<std::vector<int>> invalidUpdates;  // for part 2
//    ReadInputData(filename, graphRules, updatesNumbers);
//    int total = SumMiddlePage(updatesNumbers, graphRules, invalidUpdates);
//    std::cout << "total: " << total << std::endl;
//}
