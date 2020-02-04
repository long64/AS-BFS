#include "node.hpp"
#include "utility.hpp"
#include "global.hpp"
#include "as_util.hpp"
#include "bfs.hpp"
#include <iostream>
#include <vector>
// #include <set>
// #include <unordered_set>
// #include <map>
// #include <unordered_map>

using namespace std;

int main(int argc, char *argv[])
{
    InitializeAsRel();
    InitializeAsMap();
    InitializeAsCone();

    // linkEdge1 = atoi(argv[1]);
    // linkEdge2 = atoi(argv[2]);

    /*
    unordered_set<int> allASSet = MakeSetFromFile("allAS.txt");
    unordered_set<int> tier1Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier1.txt");
    unordered_set<int> tier2Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier2.txt");
    unordered_set<int> tier3Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier3.txt");
    unordered_set<int> stubSet = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/stub.txt");
    */

    unordered_map<int, Node> baseMap = asMap;

    cout << "\nSearch the path (>s d)\n>";

    while (true)
    {
        asMap = baseMap;

        int as1, as2;
        cin >> as1 >> as2;

        if (asMap.find(as1) == asMap.end() or asMap.find(as2) == asMap.end())
        {
            cout << "Not Found\n>";
            continue;
        }

        PrintAsInfo(as1);
        PrintAsInfo(as2);

        FirstStage(as2);
        SecondStage(as2);
        ThirdStage(as2);

        cout << "AS PATH from AS"
             << as1
             << " to AS"
             << as2
             << ":";
        PrintVector(GetPath(as1, as2));

        cout << "\n"
             << CountPathViaAs(as1, as2)
             << " ASes having path via AS"
             << as1
             << " to AS"
             << as2
             << "\n\n>";
    }

    return 0;
}