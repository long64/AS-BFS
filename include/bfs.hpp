#pragma once

#include "utility.hpp"
#include "as_util.hpp"
#include "node.hpp"
#include "global.hpp"
#include <vector>

using namespace std;

vector<int> AddChildNodes(int parentASN, int stage);

void BFS(int destASN, int stage);

void FirstStage(int d);

void SecondStage(int d);

void ThirdStage(int d);