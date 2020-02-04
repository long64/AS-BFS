#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <sstream>

std::vector<std::vector<int> > as_rel;

struct Node
{
  int myASN;
  std::vector<int> parentsASNs;
  std::vector<int> childrenASNs;
  int depth;
  int stage;
};

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
  std::cout << ", depth:" << n.depth << ", stage:" << n.stage << "\n";
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

std::vector<int> GetProvider(int d)
{
  std::vector<int> u;
  u.reserve(100);
  for(std::vector<std::vector<int> >::iterator it = as_rel.begin(); it != as_rel.end(); ++it)
    {
      if((*it)[1] == d && (*it)[2] == -1)
        {
          u.push_back((*it)[0]);
        }
    }
  return u;
}

std::vector<int> GetPeer(int d)
{
  std::vector<int> u;
  u.reserve(1000);
  for(std::vector<std::vector<int> >::iterator it = as_rel.begin(); it != as_rel.end(); ++it)
    {
      if((*it)[0] == d && (*it)[2] == 0)
        {
          u.push_back((*it)[1]);
        }
      else if((*it)[1] == d && (*it)[2] == 0)
        {
          u.push_back((*it)[0]);
        }
    }
  return u;
}

std::vector<int> GetCustomer(int d)
{
  std::vector<int> u;
  u.reserve(10000);
  for(std::vector<std::vector<int> >::iterator it = as_rel.begin(); it != as_rel.end(); ++it)
    {
      if((*it)[0] == d && (*it)[2] == -1)
        {
          u.push_back((*it)[1]);
        }
    }
  return u;
}


std::vector<int> AddChildNodes(std::unordered_map<int, Node> &map, int parentASN, int id)
{
  Node parent = map[parentASN];
  std::vector<int> u;
  std::vector<int> v;
  u.reserve(100);
  v.reserve(100);
  if(id == 1)
    { 
      u = GetProvider(parentASN);
    }
  else if(id == 2)
    {
      u = GetPeer(parentASN);
    }
  else if(id == 3)
    {
      u = GetCustomer(parentASN);
    }
  for(std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
      Node child;
      if(map.find(*it) != map.end())
        {
          if(map[*it].stage != id)
            {
              continue;
            }
          if(map[*it].depth <= parent.depth)
            {
              continue;
            }
          child = map[*it];
          child.parentsASNs.push_back(parentASN);
          parent.childrenASNs.push_back(*it);
          map[*it] = child;
          map[parentASN] = parent;
          //std::cout << *it << "(" << child.parentsASNs.size() << "),";
        }
      else
        {
          child.myASN = *it;
          child.parentsASNs.push_back(parentASN);
          child.depth = parent.depth + 1;
          child.stage = id;
          parent.childrenASNs.push_back(*it);
          map[*it] = child;
          map[parentASN] = parent;
          v.push_back(*it);
          //std::cout << *it << "(" << child.parentsASNs.size() << "),";
        }
    }
  return v; // newly added nodes
}

void BFS(std::unordered_map<int, Node> &map, int destASN, int id)
{
  std::queue<int> que;
  que.push(destASN);
  std::unordered_map<int, bool> flag;
  flag[destASN] = true;
  while(que.size() > 0)
    {
      int asn = que.front();
      que.pop();
      std::vector<int> v = map[asn].childrenASNs;
      for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
          if(flag.find(*it) == flag.end())
            {
              que.push(*it);
              flag[*it] = true;
            }
        }
      v = AddChildNodes(map, asn, id);
      if(id == 1 or id == 3)
        {
          for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
            {
              if(flag.find(*it) == flag.end())
                {
                  que.push(*it);
                  flag[*it] = true;
                }
            }
        }
    }
}

std::unordered_map<int, Node> FirstStage(int d)
{
  std::cout << "* First stage\n";
  std::unordered_map<int, Node> map;
  std::vector<int> empty;
  Node node_d = {d, empty, empty, 0, 0};
  map[d] = node_d;
  BFS(map, d, 1);
  std::cout << "total size:" << map.size() << "\n";
  return map;
}

std::unordered_map<int, Node> SecondStage(std::unordered_map<int, Node> map, int d)
{
  std::cout << "* Second stage\n";
  BFS(map, d, 2);
  std::cout << "total size:" << map.size() << "\n";
  return map;
}

std::unordered_map<int, Node> ThirdStage(std::unordered_map<int, Node> map, int d)
{
  std::cout << "* Third stage\n";
  BFS(map, d, 3);
  std::cout << "total size:" << map.size() << "\n";
  return map;
}

std::vector<int> GetPath(std::unordered_map<int, Node> map, int s, int d)
{
  std::vector<int> path;
  if(map.find(s) == map.end())
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
  std::ifstream ifs(argv[1]);
  std::string str;
  int line = 0;
  while(std::getline(ifs, str) && line < atoi(argv[2]))
    {
      if(str[0] == '#')
        {
          continue;
        }
      std::vector<int> splited = Split(str, '|');
      as_rel.push_back(splited);
      ++line;
    }

  std::cout << "Add new links (press q to finish)\n>";
  while(std::getline(std::cin, str))
    {
      if(str == "q")
        {
          break;
        }
      std::vector<int> splited = Split(str, '|');
      as_rel.push_back(splited);
      std::cout << ">";
    }

  int dest = atoi(argv[3]);

  std::cout << "\nProvider:";
  PrintVector(GetProvider(dest));
  std::cout << "\n";
  
  std::cout << "Peer:";
  PrintVector(GetPeer(dest));
  std::cout << "\n";
  
  std::cout << "Customer:";
  PrintVector(GetCustomer(dest));
  std::cout << "\n";
  
  std::cout << "\n-- Map for AS" << dest << " --\n";

  std::unordered_map<int, Node> map = FirstStage(dest);
  map = SecondStage(map, dest);
  map = ThirdStage(map, dest);
 
  /*
  for(const auto& [key, value] : map)
    {
      PrintNode(value);
      std::cout << "  AS PATH:";
      PrintVector(GetPath(map, key, dest));
    }
  */

  std::cout << "\nSearch the path to the destination\n>";
  while(true)
    {
      int asn;
      std::cin >> asn; 
      std::cout << "Provider:";
      PrintVector(GetProvider(asn));
      std::cout << ", Peer:";
      PrintVector(GetPeer(asn));
      std::cout << ", Customer:";
      PrintVector(GetCustomer(asn));
      std::cout << "\n";
      if(map.find(asn) == map.end())
        {
          std::cout << "cannot reach\n>";
          continue;
        }
      PrintNode(map[asn]);
      std::cout << "AS PATH:";
      PrintVector(GetPath(map, asn, dest));
      std::cout << "\n>";
    }
}
