#pragma once

#include "node.hpp"
#include <unordered_map>
#include <vector>

using namespace std;

extern unordered_map<int, unordered_map<int, vector<int>>> asRel;

extern unordered_map<int, vector<int>> asCone;

extern unordered_map<int, Node> asMap;

extern unordered_map<int, int> nViaPath;

extern int linkEdge1;
extern int linkEdge2;

void AddLink(vector<int> v);

void AddLink(const int as1, const int as2, const int type);

void DelLink(const int as1, const int as2, const int type);

int DelLink(const int as1, const int as2);

void AddConeLink(vector<int> v);

void InitializeAsRel();

void InitializeAsMap();

void ResetAsMap();

void InitializeAsCone();