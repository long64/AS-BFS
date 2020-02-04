#pragma once

#include "node.hpp"
#include "global.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

using namespace std;

map<int, Node> CastUnorderedMapToMap(unordered_map<int, Node> um);

int MapSum(unordered_map<int, int> um);

void WriteVector(ofstream &ofs, vector<int> v);

void PrintVector(vector<int> v);

void WriteSet(ofstream &ofs, set<int> s);

void PrintSet(unordered_set<int> s);

bool Find(vector<int> v, int n);

bool FindLink(vector<int> v, int m, int n);

void PrintNode(Node n);

vector<int> Split(string str, char sep);

unordered_set<int> MakeSetFromFile(string filename);

int GetRandomIntFromSet(unordered_set<int> s);

unordered_map<int, Node> InitializeMapFromSet(unordered_set<int> s);

double Average(vector<int> v);

double Average(vector<double> v);

double Variance(vector<int> v);

double Variance(vector<double> v);