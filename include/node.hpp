#pragma once

#include <vector>

class Node
{
private:
  int m_asn;
  std::vector<int> m_parent;
  std::vector<int> m_child;
  int m_depth;
  int m_stage;
  int m_searched;
  int m_flag;

public:
  Node(const int asn = 0);
  int GetAsn();
  std::vector<int> GetParent();
  void AddParent(const int asn);
  std::vector<int> GetChild();
  void AddChild(const int asn);
  int GetDepth();
  void SetDepth(const int depth);
  int GetStage();
  void SetStage(const int stage);
  int GetSearched();
  void SetSearched(const int searched);
  int GetFlag();
  void SetFlag(const int flag);
};