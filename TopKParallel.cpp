#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <climits>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <unordered_map>

using namespace std;

void findKShortest(stringstream& ss, int edges[][3], int n, int m, int k, int src, int dest)
{
    vector<vector<pair<int, int> > > g(n + 1);
    for (int i = 0; i < m; i++) {
        g[edges[i][0]].push_back({ edges[i][1], edges[i][2] });
    }

    vector<vector<int> > dis(n + 1, vector<int>(k, 1e9));

    priority_queue<pair<int, int>, vector<pair<int, int> >,
                greater<pair<int, int> > >
        pq;
    pq.push({ 0, src });
    dis[src][0] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();
        if (dis[u][k - 1] < d)
            continue;
        vector<pair<int, int> > v = g[u];

        #pragma omp parallel for
        for (int i = 0; i < v.size(); i++) {
            int dest = v[i].first;
            int cost = v[i].second;

            int new_dist = d + cost;
            if (new_dist < dis[dest][k - 1]) {
                #pragma omp critical
                {
                    if (new_dist < dis[dest][k - 1]) {
                        dis[dest][k - 1] = new_dist;
                        sort(dis[dest].begin(), dis[dest].end());
                        pq.push({ new_dist, dest });
                    }
                }
            }
        }
    }

    int paths[k];
    for (int i = 0; i < k; i++) {
        paths[i] = (dis[dest][i] < 1e9) ? dis[dest][i] : INT_MAX;
    }

    // Write the shortest paths to the stringstream
    ss << "The " << k << " shortest paths from " << src << " to " << dest << " are: ";
    for (int i = 0; i < k; i++) {
        if (paths[i] != INT_MAX) {
            ss << paths[i] << " ";
        } else {
            ss << "No path found";
            break;
        }
    }
    ss << endl;
}

int main(int argc, char** argv)
{
    clock_t start_ = clock();    
    // Initialize MPI and get rank and size
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ifstream file("dataset1.txt");
    if (!file) {
        cout << "Could not open file\n";
        return 1;
    }

    string buffer;
    getline(file, buffer);
    int N, M, K = 10;
    
    sscanf(buffer.c_str(), "# Nodes: %d Edges: %d", &N, &M);
    getline(file, buffer);

    int edges[M][3];
    for (int i = 0; i < M; i++) {
        file >> edges[i][0] >> edges[i][1];
        edges[i][2] = 1;
    }

    file.close();

    // Generate 10 random pairs of nodes
    srand(time(0));
    int pairs[10][2];
    for (int i = 0; i < 10; i++) {
        pairs[i][0] = rand() % N;  // Random source node
        pairs[i][1] = rand() % N;  // Random destination node
    }

    // Divide the pairs of nodes equally among the processes
    int pairs_per_process = 10 / size;
    int start = rank * pairs_per_process;
    int end = start + pairs_per_process;

    // If there are more pairs than processes, randomly assign the remaining pairs
    if (rank < 10 % size) {
        start += rank;
        end += rank + 1;
    } else {
        start += 10 % size;
        end += 10 % size;
    }

    string result;
    #pragma omp parallel for
    for (int i = start; i < end; i++) {
        stringstream ss;
        findKShortest(ss, edges, N, M, K, pairs[i][0], pairs[i][1]);
        #pragma omp critical
        {
            result += ss.str();
        }
    }

    // Gather the results at the root process
    int result_size = result.size();
    vector<int> recvcounts(size);
    vector<int> displs(size);
    MPI_Gather(&result_size, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i - 1] + recvcounts[i - 1];
        }
    }
    vector<char> recvbuf(displs[size - 1] + recvcounts[size - 1]);
    MPI_Gatherv(result.data(), result_size, MPI_CHAR, recvbuf.data(), recvcounts.data(), displs.data(), MPI_CHAR, 0, MPI_COMM_WORLD);

    // The root process prints the results
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            cout << string(recvbuf.data() + displs[i], recvcounts[i]);
        }
    }

   MPI_Finalize();
   clock_t end_ = clock();
   double duration = double(end_ - start_) / CLOCKS_PER_SEC;

   if (rank == 0) {
        cout << "Execution time: " << duration << " seconds" << endl;
    }
   return 0;
}

// int main(int argc, char** argv)
// {
//     clock_t start_ = clock();
//     // Initialize MPI and get rank and size
//     MPI_Init(&argc, &argv);
//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     ifstream file("doctorwho.csv");
//     if (!file) {
//         cout << "Could not open file\n";
//         return 1;
//     }

//     // Skip the header line
//     string line;
//     getline(file, line);

//     unordered_map<string, int> nodeMap;
//     vector<pair<string, string>> edgeList;
//     int nodeId = 0;

//     while (getline(file, line)) {
//         stringstream ss(line);
//         string source, target, weight;

//         // Read each field from the CSV line
//         getline(ss, source, ',');
//         getline(ss, target, ',');
//         getline(ss, weight, ',');

//         // Map the source and target nodes to integers
//         if (nodeMap.count(source) == 0) {
//             nodeMap[source] = nodeId++;
//         }
//         if (nodeMap.count(target) == 0) {
//             nodeMap[target] = nodeId++;
//         }

//         // Add the edge to the edge list
//         edgeList.push_back({source, target});
//     }

//     file.close();

//     int N = nodeId, M = edgeList.size(), K = 10;
//     int edges[M][3];
//     for (int i = 0; i < M; i++) {
//         edges[i][0] = nodeMap[edgeList[i].first];
//         edges[i][1] = nodeMap[edgeList[i].second];
//         edges[i][2] = 1;
//     }

//     // Generate 10 random pairs of nodes
//     srand(time(0));
//     int pairs[10][2];
//     for (int i = 0; i < 10; i++) {
//         pairs[i][0] = i+1;  // Random source node
//         pairs[i][1] = i+2;  // Random destination node
//     }

//     // Divide the pairs of nodes equally among the processes
//     int pairs_per_process = 10 / size;
//     int start = rank * pairs_per_process;
//     int end = start + pairs_per_process;

//     // If there are more pairs than processes, randomly assign the remaining pairs
//     if (rank < 10 % size) {
//         start += rank;
//         end += rank + 1;
//     } else {
//         start += 10 % size;
//         end += 10 % size;
//     }

//     string result;
//     #pragma omp parallel for
//     for (int i = start; i < end; i++) {
//         stringstream ss;
//         findKShortest(ss, edges, N, M, K, pairs[i][0], pairs[i][1]);
//         #pragma omp critical
//         {
//             result += ss.str();
//         }
//     }

//     // Gather the results at the root process
//     int result_size = result.size();
//     vector<int> recvcounts(size);
//     vector<int> displs(size);
//     MPI_Gather(&result_size, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
//     if (rank == 0) {
//         displs[0] = 0;
//         for (int i = 1; i < size; i++) {
//             displs[i] = displs[i - 1] + recvcounts[i - 1];
//         }
//     }
//     vector<char> recvbuf(displs[size - 1] + recvcounts[size - 1]);
//     MPI_Gatherv(result.data(), result_size, MPI_CHAR, recvbuf.data(), recvcounts.data(), displs.data(), MPI_CHAR, 0, MPI_COMM_WORLD);

//     // The root process prints the results
//     if (rank == 0) {
//         for (int i = 0; i < size; i++) {
//             cout << string(recvbuf.data() + displs[i], recvcounts[i]);
//         }
//     }

//     MPI_Finalize();
//     clock_t end_ = clock();
//     double duration = double(end_ - start_) / CLOCKS_PER_SEC;

//     if (rank == 0) {
//         cout << "Execution time: " << duration << " seconds" << endl;
//     }
//     return 0;
// }