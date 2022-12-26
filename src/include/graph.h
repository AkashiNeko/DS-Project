#pragma once

#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <utility>
#include <functional>

#include <cstdio>
#include <cassert>

#define _TITLE_ "city num,city name,x,y,neighbor,"

typedef std::pair<std::vector<int>, std::vector<std::vector<size_t>>> DijkResult;

struct NeighborCity {
    size_t cityNum;
    int distance = 1;
};

struct Coordinate {
    int x = 0;
    int y = 0;
};

typedef std::list<NeighborCity> Neighbor;

struct city {
    std::string cityName;
    Neighbor neighbors;
    Coordinate coordinate = { 0,0 };
public:
    void clear() {
        cityName.clear();
        neighbors.clear();
        coordinate.x = 0;
        coordinate.y = 0;
    }
};

class Graph : public std::vector<city> {
private:
    DijkResult* dijkResult;
    size_t dijkstraSrc;
    size_t cityCount;
public:
    Graph();
    ~Graph();
    // city node
    void addCity(size_t num, Neighbor nbs = {}, std::string name = "NULL", Coordinate cd = { 0,0 });
    void delCity(size_t num);
    bool emptyCity(size_t cityNum);
    size_t minVacancy();
    // city path
    int getLine(size_t src, size_t dest);
    void setLine(size_t src, size_t dest, int newDistance);
    void removeLine(size_t src, size_t dest);
    // file
    bool saveToCSV(std::string path);
    bool readFromCSV(std::string path);
    // traversal
    std::vector<size_t> dfs(size_t source);
    std::vector<size_t> bfs(size_t source);
    // shortest path
    void dijkstra(size_t src);
    std::vector<size_t>& getShortestPath(size_t dest);
    int getShortestDistance(size_t dest);
    inline size_t getCurDijkstraSrc() { return dijkstraSrc; }
    inline std::vector<int>& getDijkResultDistance() { return dijkResult->first; }
    inline std::vector<std::vector<size_t>>& getDijkResultPath() { return dijkResult->second; }
};

#endif // GRAPH_H
