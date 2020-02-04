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

std::unordered_map<int, std::unordered_map<int, std::vector<int>>> as_rel;

std::unordered_map<int, std::vector<int>> as_cone;

std::unordered_map<int, int> nViaPath;

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

std::unordered_set<int> stubSet;

std::unordered_set<int> nonStubSet;

int MapSum(std::unordered_map<int, int> m)
{
  int sum = 0;
  for (const auto &[key, value] : m)
  {
    sum = sum + value;
  }
  return sum;
}

int GetRandomASN()
{
  std::unordered_map<int, Node>::iterator it = map.begin();
  advance(it, rand() % map.size());
  return it->first;
}

int GetRandomStubASN()
{
  std::unordered_set<int>::iterator it = stubSet.begin();
  advance(it, rand() % stubSet.size());
  return *it;
}

int GetRandomNonStubASN()
{
  std::unordered_set<int>::iterator it = nonStubSet.begin();
  advance(it, rand() % nonStubSet.size());
  return *it;
}

int GetRandomASNFromSet(std::unordered_set<int> set)
{
  std::unordered_set<int>::iterator it = set.begin();
  advance(it, rand() % set.size());
  return *it;
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
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
  {
    std::cout << *it;
    if (it != v.end() - 1)
    {
      std::cout << ',';
    }
  }
  std::cout << "}";
}

bool Find(std::vector<int> v, int n)
{
  std::vector<int>::iterator it = std::find(v.begin(), v.end(), n);
  if (it == v.end())
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
  if (m == n)
  {
    return Find(v, m);
  }
  else
  {
    if (Find(v, m) and Find(v, n))
    {
      std::vector<int>::iterator it = std::find(v.begin(), v.end(), m);
      std::vector<int>::iterator jt = std::find(v.begin(), v.end(), n);
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
  while (std::getline(ss, buffer, sep))
  {
    v.push_back(std::stoi(buffer));
  }
  return v;
}

void AddLink(std::vector<int> v)
{
  if (v[2] == 0)
  {
    as_rel[v[0]][0].push_back(v[1]); // peer
    as_rel[v[1]][0].push_back(v[0]); // peer
  }
  else if (v[2] == -1)
  {
    as_rel[v[0]][-1].push_back(v[1]); // customer
    as_rel[v[1]][1].push_back(v[0]);  // provider
  }
}

void AddConeLink(std::vector<int> v)
{
  as_cone[v[0]].insert(as_cone[v[0]].end(), v.begin() + 1, v.end());
}

std::unordered_set<int> MakeSetFromFile(std::string filename)
{
  std::unordered_set<int> set;
  std::string str;
  std::ifstream ifs(filename);
  while (std::getline(ifs, str))
  {
    std::vector<int> v = Split(str, ' ');
    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
      set.insert(*it);
    }
  }
  ifs.close();
  return set;
}

std::unordered_map<int, Node> InitializeMapFromSet(std::unordered_set<int> s)
{
  std::unordered_map<int, Node> m;
  Node n;
  n.depth = -1;
  n.stage = -1;
  n.searched = 0;
  n.flag = -1;
  for (std::unordered_set<int>::iterator it = s.begin(); it != s.end(); ++it)
  {
    n.myASN = *it;
    m[*it] = n;
  }
  return m;
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

bool IsProvider(int m, int n)
{
  return Find(GetProvider(n), m);
}

bool IsPeer(int m, int n)
{
  return Find(GetPeer(n), m);
}

bool IsCustomer(int m, int n)
{
  return Find(GetCustomer(n), m);
}

bool IsConnected(int m, int n)
{
  return IsProvider(m, n) or IsPeer(m, n) or IsCustomer(m, n);
}

bool HasOneProvider(int d)
{
  if (as_rel[d][1].size() == 1)
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
  if (as_rel[d][0].size() == 0)
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
  if (as_rel[d][-1].size() == 0)
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
  if (IsStub(d) and HasOneProvider(d))
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
  if (IsOneProviderStub(d) and HasNoPeer(d))
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
  if (IsOneProviderStub(d) and not HasNoPeer(d))
  {
    for (std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
      if (not IsStub(*it))
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
  if (not IsStub(d) and HasNoPeer(d) and HasOneProvider(d))
  {
    for (std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
      if (not IsStrictStub(*it))
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
  for (std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
  {
    if (*it == d)
    {
      continue;
    }
    std::vector<int> v = GetProvider(*it);
    for (std::vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
    {
      if (not Find(u, *jt))
      {
        return false;
      }
    }
    v = GetPeer(*it);
    for (std::vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
    {
      if (not Find(u, *jt))
      {
        return false;
      }
    }
    v = GetCustomer(*it);
    for (std::vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
    {
      if (not Find(u, *jt))
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
  if (map.find(s) == map.end())
  {
    return path;
  }
  if (map[s].stage == -1)
  {
    return path;
  }
  int n = s;
  while (n != d)
  {
    std::vector<int> v = map[n].parentsASNs;
    // tie break
    n = *std::min_element(v.begin(), v.end());
    path.push_back(n);
  }
  return path;
}

bool HasRoute(int s, int d)
{
  if (GetPath(s, d).size() > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool IsPathFromProvider(int s, std::vector<int> v)
{
  return Find(GetProvider(s), v[0]);
}

bool IsPathFromPeer(int s, std::vector<int> v)
{
  return Find(GetPeer(s), v[0]);
}

bool IsPathFromCustomer(int s, std::vector<int> v)
{
  return Find(GetCustomer(s), v[0]);
}

bool MayNeighborAnnounce(int s, int d, int neighbor)
{
  std::vector<int> v = GetPath(neighbor, d);
  if (v.size() == 0 or Find(v, s))
  {
    return false;
  }

  if (Find(GetProvider(s), neighbor))
  {
    return true;
  }
  else if (IsPathFromCustomer(neighbor, v))
  {
    return true;
  }
  else
  {
    return false;
  }
}

int CountAvailablePath(int s, int d, int type)
{
  int n = 0;
  std::vector<int> v;

  if (type == 1)
  {
    v = GetProvider(s);
  }
  else if (type == 0)
  {
    v = GetPeer(s);
  }
  else if (type == -1)
  {
    v = GetCustomer(s);
  }

  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
  {
    if (MayNeighborAnnounce(s, d, *it))
    {
      ++n;
    }
  }

  return n;
}

std::vector<int> AddChildNodes(int parentASN, int stage)
{
  Node parent = map[parentASN];
  std::vector<int> u;
  std::vector<int> v;
  if (stage == 1)
  {
    u = GetProvider(parentASN);
  }
  else if (stage == 2)
  {
    u = GetPeer(parentASN);
  }
  else if (stage == 3)
  {
    u = GetCustomer(parentASN);
  }
  for (std::vector<int>::iterator it = u.begin(); it != u.end(); ++it)
  {
    Node child = map[*it];
    if (child.stage == stage and child.depth == parent.depth + 1)
    {
      child.parentsASNs.push_back(parentASN);
      parent.childrenASNs.push_back(*it);
      map[*it] = child;
      map[parentASN] = parent;
      v.push_back(*it);
      // std::cout << *it << "(" << child.parentsASNs.size() << "),";
    }
    else if (child.stage == -1)
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

  int nPathViaLink = 0;
  std::queue<int> pathQueue;

  while (que.size() > 0)
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

    if (map[asn].stage == stage and FindLink(GetPath(asn, destASN), linkEdge1, linkEdge2))
    {
      ++nPathViaLink;
    }

    /*
       if(IsStub(asn) and asn != destASN)
       {
       continue;
       }
       */

    std::vector<int> v = map[asn].childrenASNs;
    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
      if (map[*it].searched < stage)
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

    if (stage == 1 or stage == 3) // peer not queued
    {
      for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
      {
        if (map[*it].searched < stage)
        {
          if (IsStub(*it))
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
    else if (stage == 2)
    {
      for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
      {
        if (map[*it].searched < stage)
        {
          pathQueue.push(*it);
          Node child = map[*it];
          child.searched = stage;
          map[*it] = child;
        }
      }
    }
  }
  while (pathQueue.size() > 0)
  {
    int tmp = pathQueue.front();
    pathQueue.pop();
    if (FindLink(GetPath(tmp, destASN), linkEdge1, linkEdge2))
    {
      ++nPathViaLink;
    }
  }
  nViaPath[destASN] += nPathViaLink;
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

int main(int argc, char *argv[])
{
  //linkEdge1 = atoi(argv[1]);
  //linkEdge2 = atoi(argv[2]);

  //std::ifstream ifs("sample1.txt");
  std::ifstream ifs("20190701.as-rel.txt");

  std::string str;
  while (std::getline(ifs, str))
  {
    if (str[0] == '#')
    {
      continue;
    }
    std::vector<int> splited = Split(str, '|');
    AddLink(splited);
    /*
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
    */
  }

  /*
  std::ifstream ifs2("cone.txt");
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
  */

  std::unordered_set<int> allASSet = MakeSetFromFile("allAS.txt");
  map = InitializeMapFromSet(allASSet);

  std::unordered_map<int, Node> base_map = map;

  double nAveProviderPathDisapp = 0;
  double nAvePeerPathDisapp = 0;
  double nAveCustomerPathDisapp = 0;
  int num = atoi(argv[1]);
  int count = 0;
  int n1, n2, n3 = 0;
  int nNoAvailableProviderPath = 0;
  int nNoAvailablePeerPath = 0;
  int nNoAvailableCustomerPath = 0;
  int nNoAvailablePath = 0;
  //std::unordered_set<int> tier1Set = MakeSetFromFile("tier1.txt");
  std::unordered_set<int> tier2Set = MakeSetFromFile("tier2.txt");
  //std::unordered_set<int> tier3Set = MakeSetFromFile("tier3.txt");
  std::unordered_set<int> stubSet = MakeSetFromFile("stub.txt");
  while (count < num)
  {
    linkEdge1 = GetRandomASNFromSet(tier2Set);
    linkEdge2 = GetRandomASNFromSet(stubSet);

    if (IsConnected(linkEdge1, linkEdge2))
    {
      continue;
    }

    map = base_map;
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

    map = base_map;
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

    std::cout << "["
              << linkEdge1
              << ","
              << linkEdge2
              << "]";

    int ca1 = 0;
    int ca2 = 0;
    int ca3 = 0;

    std::vector<int> newLink;
    newLink.push_back(linkEdge1);
    newLink.push_back(linkEdge2);
    newLink.push_back(-1);
    AddLink(newLink);

    map = base_map;
    FirstStage(linkEdge2);
    SecondStage(linkEdge2);
    ThirdStage(linkEdge2);

    if (nAvailableProviderPath1 != 0)
    {
      ca1 = CountAvailablePath(linkEdge1, linkEdge2, 1);
      nAveProviderPathDisapp += (double)ca1 / nAvailableProviderPath1;
      ++n1;
      if (ca1 == 0)
      {
        ++nNoAvailableProviderPath;
      }
    }
    if (nAvailablePeerPath1 != 0)
    {
      ca2 = CountAvailablePath(linkEdge1, linkEdge2, 0);
      nAvePeerPathDisapp += (double)ca2 / nAvailablePeerPath1;
      ++n2;
      if (ca2 == 0)
      {
        ++nNoAvailablePeerPath;
      }
    }
    if (nAvailableCustomerPath1 != 0)
    {
      ca3 = CountAvailablePath(linkEdge1, linkEdge2, -1);
      nAveCustomerPathDisapp += (double)ca3 / nAvailableCustomerPath1;
      ++n3;
      if (ca3 == 0)
      {
        ++nNoAvailableCustomerPath;
      }
    }

    if (ca1 == 0 and ca2 == 0 and ca3 == 0)
    {
      ++nNoAvailablePath;
      std::cout << "(N)";
    }
    std::cout << ",";

    ++count;
  }

  nAveProviderPathDisapp = nAveProviderPathDisapp / n1;
  nAvePeerPathDisapp = nAvePeerPathDisapp / n2;
  nAveCustomerPathDisapp = nAveCustomerPathDisapp / n3;

  std::cout << "\n\n"
            << n1 << ", " << n2 << ", " << n3;

  std::cout << "\n"
            << nAveProviderPathDisapp << ", " << nAvePeerPathDisapp << ", " << nAveCustomerPathDisapp << "\n";

  std::cout << nNoAvailableProviderPath << ", " << nNoAvailablePeerPath << ", " << nNoAvailableCustomerPath << ", " << nNoAvailablePath << "\n";
}
