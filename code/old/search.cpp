#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <sstream>

std::unordered_map<int, std::unordered_map<int, std::vector<int> > > as_rel;

std::unordered_map<int, std::vector<int> > as_cone;

struct Node
{
  int myASN;
  std::vector<int> parentsASNs;
  std::vector<int> childrenASNs;
  int depth;
  int stage;
  int searched;
};

std::unordered_map<int, Node> map;

void PrintVector(std::vector<int> v)
{
  std::cout << "{";
  for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
      std::cout << *it;
      if(it != v.end() - 1)
        {
          std:: cout << ',';
        }
    }
  std::cout << "}";
}

void PrintNode(Node n)
{
  std::cout << "AS" << n.myASN << " = Parents:";
  PrintVector(n.parentsASNs);
  std::cout << ", Children:";
  PrintVector(n.childrenASNs);
  std::cout << ", depth:" << n.depth << ", stage:" << n.stage << ", searched:" << n.searched << "\n";
}

bool Find(std::vector<int> v, int n)
{
  std::vector<int>::iterator it = std::find(v.begin(), v.end(), n);
  if(it == v.end())
    {
      return false;
    }
  else
    {
      return true;
    }
}

std::vector<int> Split(std::string str, char sep)
{
  std::vector<int> v;
  std::stringstream ss(str);
  std::string buffer;
  while(std::getline(ss, buffer, sep))
    {
      v.push_back(std::stoi(buffer));
    }
  return v;
}

void AddLink(std::vector<int> v)
{
  if(v[2] == 0)
    {
      as_rel[v[0]][0].push_back(v[1]); // peer
      as_rel[v[1]][0].push_back(v[0]); // peer
    }
  else if(v[2] == -1)
    {
      as_rel[v[0]][-1].push_back(v[1]); // customer
      as_rel[v[1]][1].push_back(v[0]); // provider
    }
}

void AddConeLink(std::vector<int> v)
{
  as_cone[v[0]].insert(as_cone[v[0]].end(), v.begin() + 1, v.end());
}

std::vector<int> GetProvider(int d)
{
  return as_rel[d][1];
}

std::vector<int> GetPeer(int d)
{
  return as_rel[d][0];
}

std::vector<int> GetCustomer(int d)
{
  return as_rel[d][-1];
}

bool IsStrictStub(int d)
{
  if(as_rel[d][-1].size() == 0 and as_rel[d][0].size() == 0 and as_rel[d][1].size() == 1)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool IsSemiStub(int d)
{
  std::vector<int> u = as_rel[d][-1];
  if(u.size() > 0 and as_rel[d][0].size() == 0 and as_rel[d][1].size() == 1)
    {
      for(std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
        {
          if(not IsStrictStub(*it))
            {
              return false;
            }
        }
      return true;
    }
  else
    {
      return false;
    }
}

std::vector<int> AddChildNodes(int parentASN, int stage)
{
  Node parent = map[parentASN];
  std::vector<int> u;
  std::vector<int> v;
  u.reserve(100);
  v.reserve(100);
  if(stage == 1)
    { 
      u = GetProvider(parentASN);
    }
  else if(stage == 2)
    {
      u = GetPeer(parentASN);
    }
  else if(stage == 3)
    {
      u = GetCustomer(parentASN);
    }
  for(std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
      Node child = map[*it];
      if(child.stage == stage and child.depth == parent.depth + 1)
        {
          child.parentsASNs.push_back(parentASN);
          parent.childrenASNs.push_back(*it);
          map[*it] = child;
          map[parentASN] = parent;
          v.push_back(*it);
          // std::cout << *it << "(" << child.parentsASNs.size() << "),";
        }
      else if(child.stage == -1)
        {
          child.myASN = *it;
          child.parentsASNs.push_back(parentASN);
          child.depth = parent.depth + 1;
          child.stage = stage;
          parent.childrenASNs.push_back(*it);
          map[*it] = child;
          map[parentASN] = parent;
          v.push_back(*it);
          // std::cout << *it << "(" << child.parentsASNs.size() << "),";
        }
    }
  return v; // added child nodes
}

void BFS(int destASN, int stage)
{
  std::queue<int> que;
  que.push(destASN);
  Node node_d = map[destASN];
  node_d.searched = stage;
  map[destASN] = node_d;
  while(que.size() > 0)
    {
      int asn = que.front();
      que.pop();
      std::vector<int> v = map[asn].childrenASNs;
      for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
          if(map[*it].searched < stage)
            {
              que.push(*it);
              Node child = map[*it];
              child.searched = stage;
              map[*it] = child;
            }
        }
      v = AddChildNodes(asn, stage);
      if(stage == 1 or stage == 3)
        {
          for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
            {
              if(map[*it].searched < stage)
                {
                  que.push(*it);
                  Node child = map[*it];
                  child.searched = stage;
                  map[*it] = child;
                }
            }
        }
    }
}

void FirstStage(int d)
{
  // std::cout << "* First stage\n";
  Node node_d;
  node_d.myASN = d;
  node_d.depth = 0;
  node_d.stage = 0;
  node_d.searched = 0;
  map[d] = node_d;
  BFS(d, 1);
  // std::cout << "total size:" << map.size() << "\n";
}

void SecondStage(int d)
{
  // std::cout << "* Second stage\n";
  BFS(d, 2);
  // std::cout << "total size:" << map.size() << "\n";
}

void ThirdStage(int d)
{
  // std::cout << "* Third stage\n";
  BFS(d, 3);
  // std::cout << "total size:" << map.size() << "\n";
}

std::vector<int> GetPath(int s, int d)
{
  std::vector<int> path;
  if(map.find(s) == map.end())
    {
      return path;
    }
  if(map[s].stage == -1)
    {
      return path;
    }
  int n = s;
  while(n != d)
    {
      std::vector<int> v = map[n].parentsASNs;
      // tie break
      n = *std::min_element(v.begin(), v.end());
      path.push_back(n);
    }
  return path;
}

int main(int argc, char* argv[])
{
  std::ifstream ifs("20190701.as-rel.txt");
  std::string str;
  while(std::getline(ifs, str))
    {
      if(str[0] == '#')
        {
          continue;
        }
      std::vector<int> splited = Split(str, '|');
      AddLink(splited);
      Node node0;
      node0.myASN = splited[0];
      node0.depth = -1;
      node0.stage = -1;
      node0.searched = 0;
      Node node1;
      node1.myASN = splited[1];
      node1.depth = -1;
      node1.stage = -1;
      node1.searched = 0;
      map[splited[0]] = node0;
      map[splited[1]] = node1;
    }

  std::ifstream ifs2("20190701.ppdc-ases.txt");
  std::string str2;
  while(std::getline(ifs2, str2))
    {
      if(str2[0] == '#')
        {
          continue;
        }
      std::vector<int> splited = Split(str2, ' ');
      AddConeLink(splited);
    }

  std::cout << "Add new links (press q to finish)\n>";
  while(std::getline(std::cin, str))
    {
      if(str == "q")
        {
          break;
        }
      std::vector<int> splited = Split(str, '|');
      AddLink(splited);
      Node node0;
      node0.myASN = splited[0];
      node0.stage = -1;
      node0.searched = 0;
      Node node1;
      node1.myASN = splited[1];
      node1.stage = -1;
      node1.searched = 0;
      map[splited[0]] = node0;
      map[splited[1]] = node1;
      std::cout << ">";
    }

  std::cout << map.size() << " ASes exist (";

  /*
  int nStrictStub = 0;
  int nSemiStub = 0;
  for(const auto& [key, value] : map)
    {
      if(IsStrictStub(key))
        {
          ++nStrictStub;
        }
      if(IsSemiStub(key))
        {
          ++nSemiStub;
        }
    }
  std::cout << nStrictStub << " Strict stubs, " << nSemiStub << " Semi stubs)\n";
  */

  std::cout << "23358 Strict stubs, 412 Semi stubs)\n";

  std::unordered_map<int, Node> base_map = map;

  std::cout << "\nSearch the path (>s d)\n>";

  while(true)
    {
      map = base_map;

      int as1, as2;
      std::cin >> as1 >> as2; 

      if(map.find(as1) == map.end() or map.find(as2) == map.end())
        {
          std::cout << "Not Found\n>";
          continue;
        }

      std::vector<int> ases = GetProvider(as1);
      std::cout << "\n*Source\nProvider(" << ases.size() << ") ";
      PrintVector(ases);
      ases = GetPeer(as1);
      std::cout << "\nPeer(" << ases.size() << ") ";
      PrintVector(ases);
      ases = GetCustomer(as1);
      std::cout << "\nCustomer(" << ases.size() << ") ";
      PrintVector(ases);
      ases = as_cone[as1];
      std::cout << "\nCustomer cone(" << ases.size() << ") ";
      PrintVector(ases);

      ases = GetProvider(as2);
      std::cout << "\n\n*Destination\nProvider(" << ases.size() << ") ";
      PrintVector(ases);
      ases = GetPeer(as2);
      std::cout << "\nPeer(" << ases.size() << ") ";
      PrintVector(ases);
      ases = GetCustomer(as2);
      std::cout << "\nCustomer(" << ases.size() << ") ";
      PrintVector(ases);
      ases = as_cone[as2];
      std::cout << "\nCustomer cone(" << ases.size() << ") ";
      PrintVector(ases);
      std::cout << "\n\n";

      FirstStage(as2);
      SecondStage(as2);
      ThirdStage(as2);

      std::cout << "AS PATH from AS" << as1 << " to AS" << as2 << ":";
      PrintVector(GetPath(as1, as2));

      int nViaAS1 = 0;
      for(const auto& [key, value] : map)
        {
          if(Find(GetPath(key, as2), as1))
            {
              ++nViaAS1;
            }
        }
      std::cout << "\n" << nViaAS1 << " ASes route via AS" << as1 << " to AS" << as2;

      std::cout << "\n\n>";
    }
}
