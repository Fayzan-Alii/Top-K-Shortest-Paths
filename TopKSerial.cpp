#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <unordered_map>
using namespace std;

void findKShortest(int edges[][3], int n, int m, int k, int src, int dest)
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
        int d = (pq.top().first);
        pq.pop();
        if (dis[u][k - 1] < d)
            continue;
        vector<pair<int, int> > v = g[u];

        for (int i = 0; i < v.size(); i++) {
            int dest = v[i].first;
            int cost = v[i].second;

            if (d + cost < dis[dest][k - 1]) {
                dis[dest][k - 1] = d + cost;
                sort(dis[dest].begin(), dis[dest].end());
                pq.push({ (d + cost), dest });
            }
        }
    }

	cout << "The " << k << " shortest paths from " << src << " to " << dest << " are: ";
    for (int i = 0; i < k; i++) {
        if (dis[dest][i] < 1e9) {
            cout << dis[dest][i]<<" ";
        }
        else
        {
            cout<<"No path found!";
            break;
        }
    }
    cout << endl;

}


// int main()
// {
//     clock_t start_ = clock();
//     ifstream file("dataset.txt");
//     if (!file) {
//         cout << "Could not open file\n";
//         return 1;
//     }

//     string buffer;
//     getline(file, buffer);
//     int N, M, K = 10;
//     sscanf(buffer.c_str(), "# Nodes: %d Edges: %d", &N, &M);
//     getline(file, buffer);

//     int edges[M][3];
//     for (int i = 0; i < M; i++) {
//         file >> edges[i][0] >> edges[i][1];
//         edges[i][2] = 1;
//     }

//     file.close();
//     srand(time(0));
//     for(int i=0;i<10;i++)
//     {
// 	int src = rand() % N;
// 	int dest = rand() % N;
//     findKShortest(edges, N, M, K, src, dest);
//     }
//      clock_t end_ = clock();
//    double duration = double(end_ - start_) / CLOCKS_PER_SEC;

//    cout << "Execution time: " << duration << " seconds" << endl;
//     return 0;
// }

int main()
{
    clock_t start_ = clock();
    ifstream file("doctorwho.csv");
    if (!file) {
        cout << "Could not open file\n";
        return 1;
    }

    // Skip the header line
    string line;
    getline(file, line);

    unordered_map<string, int> nodeMap;
    vector<pair<string, string>> edgeList;
    int nodeId = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string source, target, weight, type;

        // Read each field from the CSV line
        getline(ss, source, ',');
        getline(ss, target, ',');
        getline(ss, weight, ',');

        // Map the source and target nodes to integers
        if (nodeMap.count(source) == 0) {
            nodeMap[source] = nodeId++;
        }
        if (nodeMap.count(target) == 0) {
            nodeMap[target] = nodeId++;
        }

        // Add the edge to the edge list
        edgeList.push_back({source, target});
    }

    file.close();

    int N = nodeId, M = edgeList.size(), K = 10;
    int edges[M][3];
    for (int i = 0; i < M; i++) {
        edges[i][0] = nodeMap[edgeList[i].first];
        edges[i][1] = nodeMap[edgeList[i].second];
        edges[i][2] = 1;
    }

    srand(time(0));
    int src[]={1,2,3,4,5,6,7,8,9,10};
    int dest[]={2,3,4,5,6,7,8,9,10,11};
    for(int i=0;i<10;i++)
    {
        //int src = rand() % N;
        //int dest = rand() % N;
        findKShortest(edges, N, M, K, src[i], dest[i]);
    }
    clock_t end_ = clock();
    double duration = double(end_ - start_) / CLOCKS_PER_SEC;

    cout << "Execution time: " << duration << " seconds" << endl;
    return 0;
}


