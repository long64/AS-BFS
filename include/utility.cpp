#include "node.hpp"
#include "global.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

using namespace std;

map<int, Node> CastUnorderedMapToMap(unordered_map<int, Node> um)
{
    map<int, Node> m;
    for (const auto &[key, value] : um)
    {
        m[key] = value;
    }
    return m;
}

int MapSum(unordered_map<int, int> um)
{
    int sum = 0;
    for (const auto &[key, value] : um)
    {
        sum = sum + value;
    }
    return sum;
}

void WriteVector(ofstream &ofs, vector<int> v)
{
    ofs << "{";
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        ofs << *it;
        if (it != v.end() - 1)
        {
            ofs << ',';
        }
    }
    ofs << "}\n";
}

void PrintVector(vector<int> v)
{
    cout << "{";
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        cout << *it;
        if (it != v.end() - 1)
        {
            cout << ',';
        }
    }
    cout << "}";
}

void WriteSet(ofstream &ofs, set<int> s)
{
    for (set<int>::iterator it = s.begin(); it != s.end(); ++it)
    {
        ofs << " " << *it;
    }
    ofs << "\n";
}

void PrintSet(unordered_set<int> s)
{
    unordered_set<int>::iterator jt = s.end();
    advance(jt, -1);
    cout << "{";
    for (unordered_set<int>::iterator it = s.begin(); it != s.end(); ++it)
    {
        cout << *it;
        if (it != jt)
        {
            cout << ',';
        }
    }
    cout << "}";
}

bool Find(vector<int> v, int n)
{
    vector<int>::iterator it = find(v.begin(), v.end(), n);
    if (it == v.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool FindLink(vector<int> v, int m, int n)
{
    if (m == n)
    {
        return Find(v, m);
    }
    else
    {
        if (Find(v, m) and Find(v, n))
        {
            vector<int>::iterator it = std::find(v.begin(), v.end(), m);
            vector<int>::iterator jt = std::find(v.begin(), v.end(), n);
            if (it + 1 == jt or it - 1 == jt)
            {
                //PrintVector(v);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}

void PrintNode(Node n)
{
    cout << "AS" << n.GetAsn() << " = Parents:";
    PrintVector(n.GetParent());
    cout << ", Children:";
    PrintVector(n.GetChild());
    cout << ", depth:" << n.GetDepth() << ", stage:" << n.GetStage() << ", searched:" << n.GetSearched() << ", flag:" << n.GetFlag() << "\n";
}

vector<int> Split(string str, char sep)
{
    vector<int> v;
    stringstream ss(str);
    string buffer;
    while (getline(ss, buffer, sep))
    {
        v.push_back(std::stoi(buffer));
    }
    return v;
}

unordered_set<int> MakeSetFromFile(string filename)
{
    unordered_set<int> s;
    string str;
    ifstream ifs(filename);
    while (getline(ifs, str))
    {
        vector<int> v = Split(str, ' ');
        for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
            s.insert(*it);
        }
    }
    ifs.close();
    return s;
}

int GetRandomIntFromSet(unordered_set<int> s)
{
    unordered_set<int>::iterator it = s.begin();
    advance(it, rand() % s.size());
    return *it;
}

unordered_map<int, Node> InitializeMapFromSet(unordered_set<int> s)
{
    unordered_map<int, Node> m;
    for (unordered_set<int>::iterator it = s.begin(); it != s.end(); ++it)
    {
        Node n(*it);
        m[*it] = n;
    }
    return m;
}

double Average(vector<int> v)
{
    double sum = 0.0;
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        sum += *it;
    }
    return sum / v.size();
}

double Average(vector<double> v)
{
    double sum = 0.0;
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
    {
        sum += *it;
    }
    return sum / v.size();
}

double Variance(vector<int> v)
{
    double ave = Average(v);
    double sum = 0.0;
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        sum += (*it - ave) * (*it - ave);
    }
    return sum / v.size();
}

double Variance(vector<double> v)
{
    double ave = Average(v);
    double sum = 0.0;
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
    {
        sum += (*it - ave) * (*it - ave);
    }
    return sum / v.size();
}