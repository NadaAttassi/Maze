#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <nlohmann/json.hpp>
#include "httplib.hpp"

using json = nlohmann::json;
using namespace std;

struct Node {
    int x, y, cost;
    bool operator>(const Node& other) const {
        return cost > other.cost;
    }
};

vector<pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

vector<pair<int, int>> dijkstra(vector<vector<int>>& maze, pair<int, int> start, pair<int, int> end) {
    int rows = maze.size(), cols = maze[0].size();
    vector<vector<int>> dist(rows, vector<int>(cols, INT_MAX));
    vector<vector<pair<int, int>>> prev(rows, vector<pair<int, int>>(cols, { -1, -1 }));
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    pq.push({ start.first, start.second, 0 });
    dist[start.first][start.second] = 0;

    while (!pq.empty()) {
        Node node = pq.top(); pq.pop();
        int x = node.x, y = node.y, cost = node.cost;

        if (x == end.first && y == end.second) break;

        for (auto& dir : directions) {
            int dx = dir.first, dy = dir.second;
            int nx = x + dx, ny = y + dy;

            if (nx >= 0 && ny >= 0 && nx < rows && ny < cols && maze[nx][ny] == 0) {
                int new_cost = cost + 1;
                if (new_cost < dist[nx][ny]) {
                    dist[nx][ny] = new_cost;
                    pq.push({ nx, ny, new_cost });
                    prev[nx][ny] = { x, y };
                }
            }
        }
    }

    vector<pair<int, int>> path;
    for (pair<int, int> at = end; at != make_pair(-1, -1); at = prev[at.first][at.second]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());
    return path;
}

void handle_request(const httplib::Request& req, httplib::Response& res) {
    // Ajouter les en-têtes CORS
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    try {
        json request = json::parse(req.body);
        vector<vector<int>> maze = request["maze"].get<vector<vector<int>>>();
        pair<int, int> start = { request["start"][0], request["start"][1] };
        pair<int, int> end = { request["end"][0], request["end"][1] };

        vector<pair<int, int>> path = dijkstra(maze, start, end);

        json response;
        for (auto& coord : path) {
            int x = coord.first, y = coord.second;
            response["path"].push_back({ x, y });
        }

        // Log pour vérifier la réponse
        std::cout << "Path found: " << response.dump() << std::endl;

        res.set_content(response.dump(), "application/json");
    } catch (const json::parse_error& e) {
        res.status = 400; // Bad Request
        res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
    }
}

int main() {
    httplib::Server svr;

    // Gestion des requêtes OPTIONS pour CORS
    svr.Options("/solve", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    svr.Post("/solve", handle_request);
    cout << "Server running on http://localhost:8080" << endl;
    svr.listen("localhost", 8080);
}