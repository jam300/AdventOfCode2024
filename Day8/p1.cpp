
#include <cmath>
#include <iostream>
#include <optional>
#include <tuple>
#include <unordered_set>

using namespace std;

unordered_map<char, unordered_set<tuple<double, double>, collections::TupleHash>> Parse(const string& fname)
{
    unordered_map<char, unordered_set<tuple<double, double>, collections::TupleHash>> antennas{};

    auto lines = pypp::splitFileLines(fname);
    for (int i{ 0 }; i < lines.size(); ++i) {
        for (int j{ 0 }; j < lines[0].length(); ++j) {
            if (lines[i][j] != '.') {
                antennas[lines[i][j]].insert(std::make_tuple(i, j));
            }
        }
    }

    return antennas;
}

template <typename T>
vector<pair<T, T>> combinations(const unordered_set<T, collections::TupleHash>& s)
{
    vector<pair<T, T>> result;
    for (auto it1 = s.begin(); it1 != s.end(); ++it1)
        for (auto it2 = std::next(it1); it2 != s.end(); ++it2)
            result.emplace_back(*it1, *it2);

    return result;
}

double EuclideanDist(tuple<double, double>& p1, tuple<double, double>& p2)
{
    return sqrt(pow(get<0>(p1) - get<0>(p2), 2) + pow(get<1>(p1) - get<1>(p2), 2));
}

bool IsInBoundaries(tuple<double, double>& p)
{
    return 0 <= get<0>(p) && get<0>(p) < 50 && 0 <= get<1>(p) && get<1>(p) < 50;
}

pair<optional<tuple<double, double>>, optional<tuple<double, double>>> GetAntinodes(
    pair<tuple<double, double>, tuple<double, double>>& pair)
{
    auto p1 = get<0>(pair);
    auto p2 = get<1>(pair);

    // unit vec lying on vec going froom P1 to P2
    const auto dist = EuclideanDist(p1, p2);
    const auto ux = (get<0>(p2) - get<0>(p1)) / dist;
    const auto uy = (get<1>(p2) - get<1>(p1)) / dist;

    optional<tuple<double, double>> antinode1 = nullopt;
    optional<tuple<double, double>> antinode2 = nullopt;

    auto t1 = make_tuple(static_cast<double>(get<0>(p1) - dist * ux), static_cast<double>(get<1>(p1) - dist * uy)); // dist away from P1, in opposite direction wrt u={ux, uy}
    auto t2 = make_tuple(static_cast<double>(get<0>(p2) + dist * ux), static_cast<double>(get<1>(p2) + dist * uy)); // dist away from P2, in same

    if (IsInBoundaries(t1))
        antinode1 = t1;
    if (IsInBoundaries(t2))
        antinode2 = t2;

    return make_pair(antinode1, antinode2);
}

int Compute(const unordered_map<char, unordered_set<tuple<double, double>, collections::TupleHash>>& antennas)
{
    auto result{ 0 };
    collections::Counter<tuple<double, double>, collections::TupleHash> c{};

    for (auto& [_, ant_v] : antennas) {
        if (ant_v.size() < 2)
            continue;

        auto antenna_pairs = combinations(ant_v);
        for (auto pair : antenna_pairs) {
            auto [antinode1, antinode2] = GetAntinodes(pair);

            if (antinode1 != nullopt)
                c[antinode1.value()] += 1;
            if (antinode2 != nullopt)
                c[antinode2.value()] += 1;
        }
    }

    auto [first, last] = c.keys();
    for (; first != last; ++first)
        result += 1;

    return result;
}

int main()
{
    const auto antennas = Parse("C:\\Users\\JMendez\\Documents\\AdventCode\\Day8\\Input_Data.txt");
    std::cout << Compute(antennas) << std::endl;

    return 0;
}