#include "include/graph.h"

Graph::Graph() :dijkstraSrc((size_t)-1), cityCount(0) {
    this->dijkResult = new DijkResult();
}

Graph::~Graph() {
    delete dijkResult;
}

void Graph::addCity(size_t num, Neighbor nbs, std::string name, Coordinate cd) {
    assert(!name.empty());
    if (num >= this->size()) this->resize(num * 2 + 1);
    auto& city = (*this)[num];
    city.cityName = name;
    city.neighbors = nbs;
    city.coordinate = cd;
    ++cityCount;
}

void Graph::delCity(size_t num) {
    assert(num < this->size());
    (*this)[num].clear();
    auto sz = this->size();
    for (size_t i = 0; i < sz; ++i) {
        if (!emptyCity(i))
            removeLine(i, num);
    }
    --cityCount;
}

int Graph::getLine(size_t src, size_t dest) {
    if (this->emptyCity(src) || this->emptyCity(dest)) return INT_MAX;
    for (auto& e : (*this)[src].neighbors) {
        if (e.cityNum == dest)
            return e.distance;
    }
    return INT_MAX;
}

void Graph::setLine(size_t src, size_t dest, int newDistance) {
    assert(!this->emptyCity(src));
    assert(!this->emptyCity(dest));
    for (auto& e : (*this)[src].neighbors) {
        if (e.cityNum == dest) {
            e.distance = newDistance;
            return;
        }
    }
    (*this)[src].neighbors.push_back({dest, newDistance});
}

void Graph::removeLine(size_t src, size_t dest) {
    auto& nbs = (*this)[src].neighbors;
    for (auto it = nbs.begin(); it != nbs.end(); ++it) {
        if (it->cityNum == dest) {
            nbs.erase(it);
            return;
        }
    }
}

bool Graph::emptyCity(size_t cityNum) {
    return cityNum >= this->size() || (*this)[cityNum].cityName.empty();
}

size_t Graph::minVacancy() {
    auto sz = this->size();
    for (size_t i = 0; i < sz; ++i) {
        if (this->emptyCity(i)) return i;
    }
    return sz;
}

bool Graph::saveToCSV(std::string path) {
    // graph save to csv file.
    std::ofstream file;
    file.open(path.c_str(), std::ios::out);
    if (!file.is_open()) return false;
    file << _TITLE_ << std::endl;
    // write node data
    auto sz = this->size();
    for (size_t i = 0; i < sz; ++i) {
        // if the city name is empty : continue
        if ((*this)[i].cityName.empty()) continue;
        file << i << ',' << (*this)[i].cityName << ','
            << (*this)[i].coordinate.x << ','
            << (*this)[i].coordinate.y << ',';
        // write neighbor city
        for (auto& e : (*this)[i].neighbors) {
            file << e.cityNum << 'd' << e.distance << ',';
        }
        file << std::endl;
    }
    file.close();
    return true;
}

bool Graph::readFromCSV(std::string path) {
    // read a graph from a csv file.
    std::ifstream file(path.c_str(), std::ios::in);
    if (!file.is_open())
        return false;
    clear();
    std::string line;
    // title
    getline(file, line);
    if (line.substr(0, 32) != _TITLE_) {
        file.close();
        return false;
    }
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string str;
        // city number
        getline(ss, str, ',');
        size_t num = stoi(str);
        if (num >= this->size()) resize(num * 2 + 1);
        // city name
        getline(ss, str, ',');
        (*this)[num].cityName = str;
        // coordinate
        getline(ss, str, ',');
        (*this)[num].coordinate.x = stoi(str);
        getline(ss, str, ',');
        (*this)[num].coordinate.y = stoi(str);
        // neighbor city
        while (getline(ss, str, ',')) {
            if (str.empty()) break;
            NeighborCity nb;
            sscanf(str.c_str(), "%zud%d", &nb.cityNum, &nb.distance);
            (*this)[num].neighbors.push_back(nb);
        }
        ++cityCount;
    }
    file.close();
    return true;
}

std::vector<size_t> Graph::dfs(size_t source) {
    assert(!emptyCity(source));
    std::vector<bool> visit(this->size(), false);
    std::vector<size_t> ans;
    std::function<void(const size_t cur)> f;
    f = [&](const size_t cur) {
        if (!visit[cur]) {
            visit[cur] = true;
            ans.push_back(cur);
            for (auto& nb : (*this)[cur].neighbors) {
                f(nb.cityNum);
            }
        }
    };
    f(source);
    return ans;
}

std::vector<size_t> Graph::bfs(size_t source) {
    assert(!emptyCity(source));
    std::vector<bool> visit(this->size(), false);
    std::vector<size_t> ans;
    std::queue<size_t> q;
    q.push(source);
    visit[source] = true;
    while (!q.empty()) {
        auto sz = q.size();
        while (sz--) {
            size_t cur = q.front();
            q.pop();
            ans.push_back(cur);
            for (auto& nb : (*this)[cur].neighbors) {
                if (!visit[nb.cityNum]) {
                    q.push(nb.cityNum);
                    visit[nb.cityNum] = true;
                }
            }
        }
    }
    return ans;
}

void Graph::dijkstra(size_t src) {
    assert(!this->emptyCity(src));
    this->dijkstraSrc = src;
    size_t sz = this->size();
    assert(sz);
    // visited citys
    std::vector<bool> visit(sz, true);
    for (size_t i = 0; i < sz; ++i) {
        if (this->emptyCity(src))
            visit[i] = false;
    }
    // result (distance and path)
    auto& vd = this->dijkResult->first;
    vd.clear();
    vd.resize(sz, INT_MAX);
    vd[src] = 0;
    auto& path = this->dijkResult->second;
    path.clear();
    path.resize(sz, { src });
    auto cur = src;
    for (size_t i = 0; i < sz; ++i) {
        // calculate the distance between the current city and neighbors
        if (visit[cur]) {
            visit[cur] = false;
            // traverse neighbors
            for (auto& e : (*this)[cur].neighbors) {
                if (visit[e.cityNum]) {
                    // if the distance is shorter, replace
                    auto tmp = vd[cur] + e.distance;
                    if (tmp < vd[e.cityNum]) {
                        vd[e.cityNum] = tmp;
                        path[e.cityNum] = path[cur];
                        path[e.cityNum].push_back(e.cityNum);
                    }
                }
            }
        }
        // select the index of the minimum value
        bool end = true;
        int min = INT_MAX;
        for (size_t j = 0; j < sz; ++j) {
            if (visit[j]) {
                if (vd[j] < min) {
                    min = vd[j];
                    cur = j;
                }
                end = false;
            }
        }
        if (end) break;
    }
}

std::vector<size_t>& Graph::getShortestPath(size_t dest) {
    assert(!this->emptyCity(dest));
    return this->dijkResult->second[dest];
}

int Graph::getShortestDistance(size_t dest) {
    assert(!this->emptyCity(dest));
    return this->dijkResult->first[dest];
}
