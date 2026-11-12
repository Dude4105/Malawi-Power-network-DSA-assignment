// Dijkstra's algorithm on the Malawi Power Network (adjacency-matrix version)
// Nodes 1..21, undirected graph, edge weight = electrical resistance.

// Build:  g++ -std=c++17 -O2 -o dijkstra_malawi dijkstra_malawi.cpp
// Run:    ./dijkstra_malawi   (then type the starting node and the ending node)

#include <algorithm>
#include <climits>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

const int V   = 21;        // number of nodes
const int INF = INT_MAX;   // "infinity" for distances
const int NO_EDGE = 0;     // 0 in the matrix means "no direct connection"

const int W_16_17 = 1;   // unlabeled in the figure (placeholder)
const int W_18_21 = 1;   // unlabeled in the figure (placeholder)

int graph[V][V];         // adjacency matrix, graph[u][v] = resistance of u-v

// Add an undirected edge using the node numbers from the drawing (1-based).
void addEdge(int u, int v, int w) {
    graph[u - 1][v - 1] = w;
    graph[v - 1][u - 1] = w;
}

void buildGraph() {
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            graph[i][j] = NO_EDGE;

    // Southern generation core
    addEdge(1, 2, 2);
    addEdge(1, 3, 2);
    addEdge(1, 4, 3);
    addEdge(2, 3, 1);
    addEdge(2, 6, 5);
    addEdge(3, 5, 1);
    addEdge(4, 5, 5);

    // Hub node 5 and the east side
    addEdge(5, 6, 8);
    addEdge(5, 7, 9);
    addEdge(5, 11, 6);
    addEdge(6, 7, 5);
    addEdge(7, 8, 4);
    addEdge(8, 9, 15);
    addEdge(8, 10, 10);
    addEdge(8, 14, 5);
    addEdge(9, 15, 9);

    // Middle band
    addEdge(10, 11, 4);
    addEdge(10, 13, 10);
    addEdge(10, 14, 2);
    addEdge(11, 12, 12);
    addEdge(11, 13, 3);

    // Northern ring
    addEdge(12, 19, 5);
    addEdge(13, 17, 11);
    addEdge(13, 18, 5);
    addEdge(14, 16, 17);
    addEdge(14, 17, 5);
    addEdge(15, 16, 1);
    addEdge(16, 17, W_16_17);
    addEdge(16, 21, 3);
    addEdge(17, 21, 2);
    addEdge(18, 19, 8);
    addEdge(18, 21, W_18_21);
    addEdge(19, 20, 4);
    addEdge(20, 21, 6);
}

// Pick the unvisited node with the smallest tentative distance.
int minDistanceNode(const vector<int>& dist, const vector<bool>& visited) {
    int best = INF, bestNode = -1;
    for (int v = 0; v < V; v++) {
        if (!visited[v] && dist[v] < best) {
            best = dist[v];
            bestNode = v;
        }
    }
    return bestNode; //the node to e selected
}

// Dijkstra with an adjacency matrix: O(V^2).
// Fills dist[] (shortest resistance from src) and prev[] (predecessor on path).
void dijkstra(int src, vector<int>& dist, vector<int>& prev) {
    dist.assign(V, INF);
    prev.assign(V, -1);
    vector<bool> visited(V, false);

    dist[src] = 0;                                   // Step 1

    for (int count = 0; count < V; count++) {
        int u = minDistanceNode(dist, visited);      // Step 3
        if (u == -1) break;                          // remaining nodes unreachable
        visited[u] = true;

        for (int v = 0; v < V; v++) {                // Steps 2 and 4: relax
            if (graph[u][v] != NO_EDGE && !visited[v]) {
                int newDist = dist[u] + graph[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                }
            }
        }
    }
}

// Step 6: trace back through prev[] to rebuild the path src -> target.
string buildPath(int target, const vector<int>& prev) {
    vector<int> path;
    for (int at = target; at != -1; at = prev[at]) path.push_back(at + 1);
    reverse(path.begin(), path.end());
    string s;
    for (size_t i = 0; i < path.size(); i++) {
        if (i) s += " -> ";
        s += to_string(path[i]);
    }
    return s;
}

void printAdjacencyMatrix() {
    cout << "Adjacency matrix (0 = no direct line):\n    ";
    for (int j = 1; j <= V; j++) cout << setw(3) << j;
    cout << "\n";
    for (int i = 0; i < V; i++) {
        cout << setw(3) << i + 1 << " ";
        for (int j = 0; j < V; j++) cout << setw(3) << graph[i][j];
        cout << "\n";
    }
    cout << "\n";
}

// Print the shortest path and resistance from one source node to ALL other nodes.
void printAllPaths(int source) {
    vector<int> dist, prev;
    dijkstra(source - 1, dist, prev);

    cout << "Shortest (lowest-resistance) paths from node " << source << "\n";
    cout << left << setw(6) << "Node" << setw(12) << "Resistance" << "Path\n";
    cout << string(50, '-') << "\n";

    for (int v = 0; v < V; v++) {
        cout << left << setw(6) << v + 1;
        if (dist[v] == INF) {
            cout << setw(12) << "INF" << "unreachable\n";
        } else {
            cout << setw(12) << dist[v] << buildPath(v, prev) << "\n";
        }
    }
    cout << "\n";
}

// Read an integer node number from the user (1..V). Returns false on bad input/EOF.
bool readNode(const string& prompt, int& node) {
    cout << prompt;
    if (!(cin >> node)) return false;
    return true;
}

int main() {
    buildGraph();
    printAdjacencyMatrix();

    // Default output: shortest paths from node 1 to every node
    printAllPaths(1);

    cout << "Malawi Power Network - lowest-resistance route finder\n";
    cout << "Nodes are numbered 1 to " << V << ". Enter 0 as the starting node to quit.\n\n";

    while (true) {
        int start, end;

        if (!readNode("Enter starting node (1-" + to_string(V) + "): ", start)) break;
        if (start == 0) break;
        if (start < 1 || start > V) {
            cout << "Invalid node. Please enter a number from 1 to " << V << ".\n\n";
            continue;
        }

        if (!readNode("Enter ending node   (1-" + to_string(V) + "): ", end)) break;
        if (end < 1 || end > V) {
            cout << "Invalid node. Please enter a number from 1 to " << V << ".\n\n";
            continue;
        }

        vector<int> dist, prev;
        dijkstra(start - 1, dist, prev);

        cout << "\n";
        if (dist[end - 1] == INF) {
            cout << "No route exists from node " << start << " to node " << end << ".\n\n";
        } else {
            cout << "Shortest path     : " << buildPath(end - 1, prev) << "\n";
            cout << "Total resistance  : " << dist[end - 1] << "\n\n";
        }
    }

    cout << "Goodbye.\n";
    return 0;
}

/*
GROUP NAMES
1. BESTER LEMSON - BIS/24/SS/027
2. EMMA MANKHAMBIRA - BIT/24/SSO24
3. KHUMBO MSONGA - BIT/23/SS/031
*/
