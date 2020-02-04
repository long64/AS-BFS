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
    InitializeAsMap();
    InitializeAsCone();

    int numLoop = atoi(argv[1]);

    // linkEdge1 = atoi(argv[2]);
    // linkEdge2 = atoi(argv[3]);

    // unordered_set<int> allASSet = MakeSetFromFile("allAS.txt");
    // unordered_set<int> tier1Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier1.txt");
    unordered_set<int> tier2Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier2.txt");
    // unordered_set<int> tier3Set = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/tier3.txt");
    unordered_set<int> stubSet = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/stub.txt");

    vector<double> providerPathDisapp;
    vector<double> peerPathDisapp;
    vector<double> customerPathDisapp;

    unordered_map<int, int> totalAvailablePathBefore;
    totalAvailablePathBefore[1] = 0;
    totalAvailablePathBefore[0] = 0;
    totalAvailablePathBefore[-1] = 0;

    unordered_map<int, int> totalAvailablePathAfter;
    totalAvailablePathAfter[1] = 0;
    totalAvailablePathAfter[0] = 0;
    totalAvailablePathAfter[-1] = 0;

    int nNoAvailableProviderPath = 0;
    int nNoAvailablePeerPath = 0;
    int nNoAvailableCustomerPath = 0;
    int nNoAvailablePath = 0;

    int count = 0;
    while (++count <= numLoop)
    {
        linkEdge1 = GetRandomIntFromSet(tier2Set);
        linkEdge2 = GetRandomIntFromSet(stubSet);

        if (IsConnected(linkEdge1, linkEdge2))
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

        int nAvailableProviderPath1 = CountAvailablePath(linkEdge1, linkEdge2, 1);
        int nAvailablePeerPath1 = CountAvailablePath(linkEdge1, linkEdge2, 0);
        int nAvailableCustomerPath1 = CountAvailablePath(linkEdge1, linkEdge2, -1);

        totalAvailablePathBefore[1] += nAvailableProviderPath1;
        totalAvailablePathBefore[0] += nAvailablePeerPath1;
        totalAvailablePathBefore[-1] += nAvailableCustomerPath1;

        ResetAsMap();
        FirstStage(linkEdge1);
        SecondStage(linkEdge1);
        ThirdStage(linkEdge1);

        if (not HasRoute(linkEdge2, linkEdge1))
        {
            continue;
        }

        /*
        int nAvailableProviderPath2 = CountAvailablePath(linkEdge2, linkEdge1, 1);
        int nAvailablePeerPath2 = CountAvailablePath(linkEdge2, linkEdge1, 0);
        int nAvailableCustomerPath2 = CountAvailablePath(linkEdge2, linkEdge1, -1);
        */

        cout << "["
             << linkEdge1
             << ","
             << linkEdge2
             << "]";

        int ca1 = 0;
        int ca2 = 0;
        int ca3 = 0;

        AddLink(linkEdge1, linkEdge2, -1);

        ResetAsMap();
        FirstStage(linkEdge2);
        SecondStage(linkEdge2);
        ThirdStage(linkEdge2);

        if (nAvailableProviderPath1 != 0)
        {
            ca1 = CountAvailablePath(linkEdge1, linkEdge2, 1);
            providerPathDisapp.push_back((double)ca1 / nAvailableProviderPath1);
            totalAvailablePathAfter[1] += ca1;
            if (ca1 == 0)
            {
                ++nNoAvailableProviderPath;
            }
        }
        if (nAvailablePeerPath1 != 0)
        {
            ca2 = CountAvailablePath(linkEdge1, linkEdge2, 0);
            peerPathDisapp.push_back((double)ca2 / nAvailablePeerPath1);
            totalAvailablePathAfter[0] += ca2;
            if (ca2 == 0)
            {
                ++nNoAvailablePeerPath;
            }
        }
        if (nAvailableCustomerPath1 != 0)
        {
            ca3 = CountAvailablePath(linkEdge1, linkEdge2, -1);
            customerPathDisapp.push_back((double)ca3 / nAvailableCustomerPath1);
            totalAvailablePathAfter[-1] += ca3;
            if (ca3 == 0)
            {
                ++nNoAvailableCustomerPath;
            }
        }

        if (ca1 == 0 and ca2 == 0 and ca3 == 0)
        {
            ++nNoAvailablePath;
            cout << "(N)";
        }
        cout << ",";

        // delete tunnel
        DelLink(linkEdge1, linkEdge2, -1);
    }

    cout << "\n"
         << Average(providerPathDisapp)
         << ":"
         << Variance(providerPathDisapp)
         << ", "
         << Average(peerPathDisapp)
         << ":"
         << Variance(peerPathDisapp)
         << ", "
         << Average(customerPathDisapp)
         << ":"
         << Variance(customerPathDisapp)
         << "\n";

    cout << (double)totalAvailablePathAfter[1] / totalAvailablePathBefore[1]
         << ", "
         << (double)totalAvailablePathAfter[0] / totalAvailablePathBefore[0]
         << ", "
         << (double)totalAvailablePathAfter[-1] / totalAvailablePathBefore[-1]
         << "\n";

    cout << nNoAvailableProviderPath
         << ", "
         << nNoAvailablePeerPath
         << ", "
         << nNoAvailableCustomerPath
         << ", "
         << nNoAvailablePath
         << "\n";

    return 0;
}