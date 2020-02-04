#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <sstream>
#include <iterator>

std::unordered_map<int, std::unordered_map<int, std::vector<int> > > as_rel;

std::unordered_map<int, std::vector<int> > as_cone;

std::unordered_map<int, int> nViaPath1;
std::unordered_map<int, int> nViaPath2;
std::unordered_map<int, int> nViaPath3;
std::unordered_map<int, int> nViaPath4;

// std::ofstream ofs("path.txt");

int linkEdge1, linkEdge2;

struct Node
{
  int myASN;
  std::vector<int> parentsASNs;
  std::vector<int> childrenASNs;
  int depth;
  int stage;
  int searched;
  int flag;
};

std::unordered_map<int, Node> map;

int MapSum(std::unordered_map<int, int> m)
{
  int sum = 0;
  for(const auto& [key, value] : m)
  {
    sum = sum + value;
  }
  return sum;
}

/*
   void WriteVector(std::vector<int> v)
   {
   ofs << "{";
   for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
   {
   ofs << *it;
   if(it != v.end() - 1)
   {
   ofs << ',';
   }
   }
   ofs << "}\n";
   }
   */

void PrintVector(std::vector<int> v)
{
  std::cout << "{";
  for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
  {
    std::cout << *it;
    if(it != v.end() - 1)
    {
      std::cout << ',';
    }
  }
  std::cout << "}";
}

void PrintSet(std::unordered_set<int> set)
{
  std::unordered_set<int>::iterator jt = set.end();
  advance(jt, -1);
  std::cout << "{";
  for(std::unordered_set<int>::iterator it = set.begin(); it != set.end(); ++it)
  {
    std::cout << *it;
    if(it != jt)
    {
      std::cout << ',';
    }
  }
  std::cout << "}";
}

void InitializeMap(std::unordered_map<int, int> &m)
{
  for(const auto& [key, value] : m)
  {
    m[key] = 0;
  }
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

bool FindLink(std::vector<int> v, int m, int n)
{
  if(m == n)
  {
    return Find(v, m);
  }
  else
  {
    if(Find(v, m) and Find(v, n))
    {
      std::vector<int>::iterator it = std::find(v.begin(), v.end(), m);
      std::vector<int>::iterator jt = std::find(v.begin(), v.end(), n);
      if(it + 1 == jt or it - 1 == jt)
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
  std::cout << "AS" << n.myASN << " = Parents:";
  PrintVector(n.parentsASNs);
  std::cout << ", Children:";
  PrintVector(n.childrenASNs);
  std::cout << ", depth:" << n.depth << ", stage:" << n.stage << ", searched:" << n.searched << ", flag:" << n.flag << "\n";
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

std::unordered_set<int> GetCustomerConeSet(int asn)
{
  std::unordered_set<int> set;
  std::unordered_map<int, int> tmpMap;
  std::queue<int> que;
  que.push(asn);
  while(not que.empty())
  {
    int n = que.front();
    que.pop();

    set.insert(n);

    std::vector<int> u = GetCustomer(n);
    for(std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
      //que.push(*it);
      if(set.find(*it) == set.end())
      {
        que.push(*it);
      }
    }
  }
  return set;
}

bool HasOneProvider(int d)
{
  if(as_rel[d][1].size() == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool HasNoPeer(int d)
{
  if(as_rel[d][0].size() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool IsStub(int d)
{
  if(as_rel[d][-1].size() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool IsOneProviderStub(int d)
{
  if(IsStub(d) and HasOneProvider(d))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool IsStrictStub(int d)
{
  if(IsOneProviderStub(d) and HasNoPeer(d))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool IsLimitedStub(int d)
{
  std::vector<int> u = as_rel[d][0];
  if(IsOneProviderStub(d) and not HasNoPeer(d))
  {
    for(std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
      if(not IsStub(*it))
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

bool IsSemiStub(int d)
{
  std::vector<int> u = as_rel[d][-1];
  if(not IsStub(d) and HasNoPeer(d) and HasOneProvider(d))
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

bool IsStubCone(int d)
{
  std::vector<int> u = as_cone[d];
  for(std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
  {
    if(*it == d)
    {
      continue;
    }
    std::vector<int> v = GetProvider(*it);
    for(std::vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
    {
      if(not Find(u, *jt))
      {
        return false;
      }
    }
    v = GetPeer(*it);
    for(std::vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
    {
      if(not Find(u, *jt))
      {
        return false;
      }
    }
    v = GetCustomer(*it);
    for(std::vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
    {
      if(not Find(u, *jt))
      {
        return false;
      }
    }
  }
  return true;
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

std::vector<int> AddChildNodes(int parentASN, int stage)
{
  Node parent = map[parentASN];
  std::vector<int> u;
  std::vector<int> v;
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

  int nPathViaLink1 = 0;
  int nPathViaLink2 = 0;
  int nPathViaLink3 = 0;
  int nPathViaLink4 = 0;
  std::queue<int> pathQueue;

  while(que.size() > 0)
  {
    int asn = que.front();
    que.pop();

    /* huge file
       if(map[asn].stage == stage)
       {
       ofs << asn << " " << destASN << " ";
       WriteVector(GetPath(asn, destASN));
       }
       */

    if(map[asn].stage == stage)
    {
      std::vector<int> path = GetPath(asn, destASN);
      if(FindLink(path, linkEdge1, linkEdge2))
      {
        ++nPathViaLink1;
      }
      if(Find(path, linkEdge1))
      {
        ++nPathViaLink2;
        if(Find(path, linkEdge2))
        {
          ++nPathViaLink3;
          ++nPathViaLink4;
        }
      }
      else
      {
        if(Find(path, linkEdge2))
        {
          ++nPathViaLink3;
        }
      }
    }

    /*
       if(IsStub(asn) and asn != destASN)
       {
       continue;
       }
       */

    std::vector<int> v = map[asn].childrenASNs;
    for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
      if(map[*it].searched < stage)
      {
        /*
           if(not IsStub(*it))
           {
           que.push(*it);
           }
           */
        que.push(*it);
        Node child = map[*it];
        child.searched = stage;
        map[*it] = child;
      }
    }

    v = AddChildNodes(asn, stage);

    if(stage == 1 or stage == 3) // peer not queued
    {
      for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
      {
        if(map[*it].searched < stage)
        {
          if(IsStub(*it))
          {
            pathQueue.push(*it);
          }
          else
          {
            que.push(*it);
          }
          //que.push(*it);
          Node child = map[*it];
          child.searched = stage;
          map[*it] = child;
        }
      }
    }
    else if(stage == 2)
    {
      for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
      {
        if(map[*it].searched < stage)
        {
          pathQueue.push(*it);
          Node child = map[*it];
          child.searched = stage;
          map[*it] = child;
        }
      }
    }
  }
  while(pathQueue.size() > 0)
  {
    int tmp = pathQueue.front();
    pathQueue.pop();

    std::vector<int> path = GetPath(tmp, destASN);
    if(FindLink(path, linkEdge1, linkEdge2))
    {
      ++nPathViaLink1;
    }
    if(Find(path, linkEdge1))
    {
      ++nPathViaLink2;
      if(Find(path, linkEdge2))
      {
        ++nPathViaLink3;
        ++nPathViaLink4;
      }
    }
    else
    {
      if(Find(path, linkEdge2))
      {
        ++nPathViaLink3;
      }
    }
  }
  nViaPath1[destASN] += nPathViaLink1;
  nViaPath2[destASN] += nPathViaLink2;
  nViaPath3[destASN] += nPathViaLink3;
  nViaPath4[destASN] += nPathViaLink4;
}

void FirstStage(int d)
{
  // std::cout << "* First stage\n";
  Node node_d;
  node_d.myASN = d;
  node_d.depth = 0;
  node_d.stage = 0;
  node_d.searched = 0;
  node_d.flag = -1;
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

int main(int argc, char* argv[])
{
  linkEdge1 = atoi(argv[1]);
  linkEdge2 = atoi(argv[2]);

  //std::ifstream ifs("sample1.txt");
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
    node0.flag = -1;
    Node node1;
    node1.myASN = splited[1];
    node1.depth = -1;
    node1.stage = -1;
    node1.searched = 0;
    node1.flag = -1;
    map[splited[0]] = node0;
    map[splited[1]] = node1;
  }

  std::cout << "Add new links (enter q to finish)\n>";
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
    node0.depth = -1;
    node0.stage = -1;
    node0.searched = 0;
    node0.flag = -1;
    Node node1;
    node1.myASN = splited[1];
    node1.depth = -1;
    node1.stage = -1;
    node1.searched = 0;
    node1.flag = -1;
    map[splited[0]] = node0;
    map[splited[1]] = node1;
    std::cout << ">";
  }

  std::ifstream ifs2("cone.txt");
  std::string str2;
  while(std::getline(ifs2, str2))
  {
    std::vector<int> splited = Split(str2, ' ');
    AddConeLink(splited);
  }

  std::unordered_map<int, Node> base_map = map;

  std::queue<int> waitingStub;

  std::vector<int> v1 = as_cone[linkEdge1];
  std::vector<int> v2 = as_cone[linkEdge2];
  std::cout << "\n(" << v1.size() << ")";
  PrintVector(v1);
  std::cout << "\n(" << v2.size() << ")";
  PrintVector(v2);
  std::cout << "\n";

  v1.push_back(linkEdge1);
  v2.push_back(linkEdge2);
  for(std::vector<int>::iterator it = v1.begin(); it != v1.end(); ++it)
  {
    std::cout << *it;

    if(IsStrictStub(*it))
    {
      std::cout << "(SS),";
      waitingStub.push(*it);
      continue;
    }

    /*
    nViaPath1[*it] = 0;
    nViaPath2[*it] = 0;
    nViaPath3[*it] = 0;
    nViaPath4[*it] = 0;
    */

    map = base_map;
    FirstStage(*it);
    SecondStage(*it);
    ThirdStage(*it);

    std::cout << ",";
  }

  for(std::vector<int>::iterator it = v2.begin(); it != v2.end(); ++it)
  {
    std::cout << *it;

    if(IsStrictStub(*it))
    {
      std::cout << "(SS),";
      waitingStub.push(*it);
      continue;
    }

    /*
    nViaPath1[*it] = 0;
    nViaPath2[*it] = 0;
    nViaPath3[*it] = 0;
    nViaPath4[*it] = 0;
    */

    map = base_map;
    FirstStage(*it);
    SecondStage(*it);
    ThirdStage(*it);

    std::cout << ",";
  }

  while(waitingStub.size() > 0)
  {
    int ss = waitingStub.front();
    waitingStub.pop();

    nViaPath1[ss] = nViaPath1[GetProvider(ss)[0]];
    nViaPath2[ss] = nViaPath1[GetProvider(ss)[0]];
    nViaPath3[ss] = nViaPath1[GetProvider(ss)[0]];
    nViaPath4[ss] = nViaPath1[GetProvider(ss)[0]];
  }

  std::cout << "\n\n" << MapSum(nViaPath1) << " routes via link [AS" << linkEdge1 << " " << "AS" << linkEdge2 << "]\n";
  std::cout << MapSum(nViaPath2) << " routes via AS" << linkEdge1 << "\n";
  std::cout << MapSum(nViaPath3) << " routes via AS" << linkEdge2 << "\n";
  std::cout << MapSum(nViaPath4) << " routes via AS" << linkEdge1 << " and AS" << linkEdge2 << "\n";
  std::cout << "the net routes through AS" << linkEdge1 << " or AS" << linkEdge2 << " : " << MapSum(nViaPath2) + MapSum(nViaPath3) - MapSum(nViaPath4) << "\n";

  // ofs.close();
}
