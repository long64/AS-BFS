#include "global.hpp"
#include "node.hpp"
#include "utility.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <iterator>

using namespace std;

unordered_map<int, unordered_map<int, vector<int>>> asRel;

unordered_map<int, vector<int>> asCone;

unordered_map<int, Node> asMap;

static unordered_map<int, Node> baseAsMap;

unordered_map<int, int> nViaPath;

int linkEdge1;
int linkEdge2;

void AddLink(vector<int> v)
{
    if (v[2] == 0)
    {
        asRel[v[0]][0].push_back(v[1]); // peer
        asRel[v[1]][0].push_back(v[0]); // peer
    }
    else if (v[2] == -1)
    {
        asRel[v[0]][-1].push_back(v[1]); // customer
        asRel[v[1]][1].push_back(v[0]);  // provider
    }
}

void AddLink(const int as1, const int as2, const int type)
{
    asRel[as1][type].push_back(as2);
    asRel[as2][-type].push_back(as1);
}

void DelLink(const int as1, const int as2, const int type)
{
    vector<int>::iterator it = find(asRel[as1][type].begin(), asRel[as1][type].end(), as2);
    asRel[as1][type].erase(it);

    vector<int>::iterator jt = find(asRel[as2][-type].begin(), asRel[as2][-type].end(), as1);
    asRel[as2][-type].erase(jt);
}

int DelLink(const int as1, const int as2)
{
    vector<int>::iterator it = find(asRel[as1][1].begin(), asRel[as1][1].end(), as2);
    if (it != asRel[as1][1].end())
    {
        asRel[as1][1].erase(it);
        vector<int>::iterator jt = find(asRel[as2][-1].begin(), asRel[as2][-1].end(), as1);
        asRel[as2][-1].erase(jt);
        return 1;
    }

    it = find(asRel[as1][0].begin(), asRel[as1][0].end(), as2);
    if (it != asRel[as1][0].end())
    {
        asRel[as1][0].erase(it);
        vector<int>::iterator jt = find(asRel[as2][0].begin(), asRel[as2][0].end(), as1);
        asRel[as2][0].erase(jt);
        return 0;
    }

    it = find(asRel[as1][-1].begin(), asRel[as1][-1].end(), as2);
    if (it != asRel[as1][-1].end())
    {
        asRel[as1][-1].erase(it);
        vector<int>::iterator jt = find(asRel[as2][1].begin(), asRel[as2][1].end(), as1);
        asRel[as2][1].erase(jt);
        return -1;
    }

    cout << "\n\nDelLink : Not Found\n\n";
    return -2;
}

void AddConeLink(vector<int> v)
{
    asCone[v[0]].insert(asCone[v[0]].end(), v.begin() + 1, v.end());
}

void InitializeAsRel()
{
    ifstream ifs("/Users/aki/Documents/M1/AS_data/data/20190701.as-rel.txt");
    string str;
    while (getline(ifs, str))
    {
        if (str[0] == '#')
        {
            continue;
        }
        AddLink(Split(str, '|'));
    }
}

void InitializeAsMap()
{
    unordered_set<int> allASSet = MakeSetFromFile("/Users/aki/Documents/M1/AS_data/data/allAS.txt");
    baseAsMap = InitializeMapFromSet(allASSet);
    asMap = baseAsMap;
}

void ResetAsMap()
{
    asMap = baseAsMap;
}

void InitializeAsCone()
{
    ifstream ifs("/Users/aki/Documents/M1/AS_data/data/cone.txt");
    string str;
    while (getline(ifs, str))
    {
        vector<int> splited = Split(str, ' ');
        AddConeLink(splited);
    }
}