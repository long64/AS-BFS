#include "node.hpp"
#include "utility.hpp"
#include "global.hpp"
#include "as_util.hpp"
#include "bfs.hpp"
#include <iostream>
#include <vector>
// #include <set>
#include <unordered_set>
// #include <map>
#include <unordered_map>

using namespace std;

int main(int argc, char *argv[])
{
    InitializeAsRel();
    InitializeAsCone();
    InitializeAsMap();

    int numLoop = atoi(argv[1]);

    // linkEdge1 = atoi(argv[2]);
    // linkEdge2 = atoi(argv[3]);

    // unordered_set<int> allASSet = MakeSetFromFile("allAS.txt");
    unordered_set<int> tier1Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier1.txt");
    // unordered_set<int> tier2Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier2.txt");
    // unordered_set<int> tier3Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier3.txt");
    unordered_set<int> stubSet = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/stub.txt");

    int nTunnelDisapp = 0;

    int count = 0;
    while (++count <= numLoop)
    {
        linkEdge1 = GetRandomIntFromSet(tier1Set);
        linkEdge2 = GetRandomIntFromSet(stubSet);
        if (IsConnected(linkEdge1, linkEdge2))
        {
            continue;
        }

        ResetAsMap();
        FirstStage(linkEdge1);
        SecondStage(linkEdge1);
        ThirdStage(linkEdge1);
        if (not HasRoute(linkEdge2, linkEdge1))
        {
            continue;
        }

        ResetAsMap();
        FirstStage(linkEdge2);
        SecondStage(linkEdge2);
        ThirdStage(linkEdge2);
        if (not HasRoute(linkEdge1, linkEdge2))
        {
            continue;
        }
        int neighbor = GetNeighborForDest(linkEdge1, linkEdge2);

        cout << count
             << ":["
             << linkEdge1
             << ","
             << linkEdge2
             << "]";
        PrintVector(GetPath(linkEdge1, linkEdge2));
        cout << "\n";

        AddLink(linkEdge1, linkEdge2, -1);
        int linkType = DelLink(linkEdge1, neighbor);

        ResetAsMap();
        FirstStage(linkEdge2);
        SecondStage(linkEdge2);
        ThirdStage(linkEdge2);

        vector<int> path = GetPath(neighbor, linkEdge2);
        if (Find(path, linkEdge1))
        {
            ++nTunnelDisapp;
            PrintVector(path);
            cout << "\n";
        }

        AddLink(linkEdge1, neighbor, linkType);
        DelLink(linkEdge1, linkEdge2, -1);
    }

    cout
        << "\n"
        << nTunnelDisapp
        << ", "
        << (double)nTunnelDisapp / numLoop
        << "\n";

    return 0;
}