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

std::unordered_map<int, int> nViaPath;

int linkEdge1;
int linkEdge2;

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

std::map<int, Node> CastUnorderedMapToMap(std::unordered_map<int, Node> m)
{
  std::map<int, Node> n;
  for(const auto& [key, value] : m)
  {
    n[key] = value;
  }
  return n;
}

int MapSum(std::unordered_map<int, int> m)
{
  int sum = 0;
  for(const auto& [key, value] : m)
  {
    sum = sum + value;
  }
  return sum;
}

void WriteVector(std::ofstream &ofs, std::vector<int> v)
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

void WriteSet(std::ofstream &ofs, std::set<int> set)
{
  for(std::set<int>::iterator it = set.begin(); it != set.end(); ++it)
  {
    ofs << " " << *it;
  }
  ofs << "\n";

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

std::set<int> GetCustomerConeSet(int asn)
{
  std::set<int> set;
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
      if(set.find(*it) == set.end())
      {
        que.push(*it);
      }
    }
  }
  set.erase(asn);
  return set;
}

std::unordered_set<int> MakeSetFromFile(std::string filename)
{
  std::unordered_set<int> set;
  std::string str;
  std::ifstream ifs(filename);
  while(std::getline(ifs, str))
  {
    std::vector<int> v = Split(str, ' ');
    for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
      set.insert(*it);
    }
  }
  ifs.close();
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

  int nPathViaLink = 0;
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

    if(map[asn].stage == stage and FindLink(GetPath(asn, destASN), linkEdge1, linkEdge2))
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
    if(FindLink(GetPath(tmp, destASN), linkEdge1, linkEdge2))
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

  std::ifstream ifs2("cone.txt");
  std::string str2;
  while(std::getline(ifs2, str2))
  {
    std::vector<int> splited = Split(str2, ' ');
    AddConeLink(splited);
  }

  int nAS = 0;
  int nStub = 0;
  int nNonStub = 0;
  int nHasOneProviderNoPeer = 0;
  int nOneProviderStub = 0;
  int nStrictStub = 0;
  int nLimitedStub = 0;
  int nSemiStub = 0;
  int nStubCone = 0;
  int nProvider0 = 0;
  int nProvider1 = 0;
  int nProvider2 = 0;
  int nProvider3 = 0;
  int nProvider4 = 0;
  int nProvider5 = 0;
  int nPeer0 = 0;
  int nPeer1 = 0;
  int nPeer2 = 0;
  int nPeer3 = 0;
  int nPeer4 = 0;
  int nPeer5 = 0;
  int nCustomer0 = 0;
  int nCustomer1 = 0;
  int nCustomer2 = 0;
  int nCustomer3 = 0;
  int nCustomer4 = 0;
  int nCustomer5 = 0;
  int nCustomer6 = 0;
  int nCustomer7 = 0;
  int nCustomer8 = 0;
  int nCone1 = 0;
  int nCone2 = 0;
  int nCone3 = 0;
  int nCone4 = 0;
  int nCone5 = 0;
  int nCone6 = 0;
  int nCone7 = 0;
  int nTier1 = 0;
  int nTier2 = 0;
  int nTier3 = 0;
  int nIXP = 0;
  //std::ofstream ofs1("allAS.txt");
  //std::ofstream ofs2("stub.txt");
  //std::ofstream ofs3("nonstub.txt");
  //std::ofstream ofs4("cone.txt");
  //std::ofstream ofs5("tier1.txt");
  //std::ofstream ofs6("tier2.txt");
  //std::ofstream ofs7("tier3.txt");
  
  std::map<int, Node> oMap = CastUnorderedMapToMap(map);
  //for(const auto& [key, value] : oMap)
  //{
    /*
    ++nAS;
    ofs1 << key;
    if(nAS % 10 == 0)
    {
      ofs1 << "\n";
    }
    else
    {
      ofs1 << " ";
    }
    */

    /*
    if(IsStub(key))
    {
      ++nStub;
      ofs2 << key;
      if(nStub % 10 == 0)
      {
        ofs2 << "\n";
      }
      else
      {
        ofs2 << " ";
      }
    }
    else
    {
      ++nNonStub;
      ofs3 << key;
      if(nNonStub % 10 == 0)
      {
        ofs3 << "\n";
      }
      else
      {
        ofs3 << " ";
      }
    }
    */

    /*
    if(HasOneProvider(key) and HasNoPeer(key))
    {
      ++nHasOneProviderNoPeer;
    }
    if(IsOneProviderStub(key))
    {
      ++nOneProviderStub;
    }
    if(IsStrictStub(key))
    {
      ++nStrictStub;
    }
    if(IsLimitedStub(key))
    {
      ++nLimitedStub;
    }
    if(IsSemiStub(key))
    {
      ++nSemiStub;
    }
    */

    /*
    if(as_cone[key].size() == 0)
    {
      ++nCone1;
    }
    else if(as_cone[key].size() == 1)
    {
      ++nCone2;
    }
    else if(as_cone[key].size() <= 10)
    {
      ++nCone3;
    }
    else if(as_cone[key].size() <= 50)
    {
      ++nCone4;
    }
    else if(as_cone[key].size() <= 100)
    {
      ++nCone5;
    }
    else if(as_cone[key].size() <= 500)
    {
      ++nCone6;
    }
    else
    {
      ++nCone7;
    }
    */

    /*
    if(as_cone[key].size() == 3)
    {
      if(IsStubCone(key))
      {
        ++nStubCone;
        std::cout << key << ",";
      }
    }
    */

    /*
    int size = GetProvider(key).size();
    if(size == 0)
    {
      ++nProvider0;
    }
    else if(size == 1)
    {
      ++nProvider1;
    }
    else if(size <= 10)
    {
      ++nProvider2;
    }
    else if(size <= 20)
    {
      ++nProvider3;
    }
    else if(size <= 50)
    {
      ++nProvider4;
    }
    else
    {
      ++nProvider5;
    }

    int size = GetPeer(key).size();
    if(size == 0)
    {
      ++nPeer0;
    }
    else if(size == 1)
    {
      ++nPeer1;
    }
    else if(size <= 10)
    {
      ++nPeer2;
    }
    else if(size <= 20)
    {
      ++nPeer3;
    }
    else if(size <= 50)
    {
      ++nPeer4;
    }
    else
    {
      ++nPeer5;
    }
    */

    /*
    int size = GetCustomer(key).size();
    if(size == 0)
    {
      ++nCustomer0;
    }
    else if(size == 1)
    {
      ++nCustomer1;
    }
    else if(size <= 5)
    {
      ++nCustomer2;
    }
    else if(size <= 10)
    {
      ++nCustomer3;
    }
    else if(size <= 25)
    {
      ++nCustomer4;
    }
    else if(size <= 50)
    {
      ++nCustomer5;
    }
    else if(size <= 100)
    {
      ++nCustomer6;
    }
    else if(size <= 250)
    {
      ++nCustomer7;
    }
    else
    {
      ++nCustomer8;
    }
    */

    /*
    int size = GetCustomer(key).size();
    if(size > 250)
    {
      ++nTier1;
      ofs5 << key;
      if(nTier1 % 10 == 0)
      {
        ofs5 << "\n";
      }
      else
      {
        ofs5 << " ";
      }
    }
    else if(size > 25)
    {
      ++nTier2;
      ofs6 << key;
      if(nTier2 % 10 == 0)
      {
        ofs6 << "\n";
      }
      else
      {
        ofs6 << " ";
      }
    }
    else if(size >= 1)
    {
      ++nTier3;
      ofs7 << key;
      if(nTier3 % 10 == 0)
      {
        ofs7 << "\n";
      }
      else
      {
        ofs7 << " ";
      }
    }
    */

    /*
    std::set<int> set = GetCustomerConeSet(key);
    ofs4 << key;
    WriteSet(ofs4, set);
    */

    /*
    if(GetProvider(key).size() < 10 and GetPeer(key).size() > 250 and GetCustomer(key).size() < 10)
    {
      ++nIXP;
      std::cout << key << ",";
    }
    */
  //}

  //ofs1.close();
  //ofs2.close();
  //ofs3.close();
  //ofs4.close();
  //ofs5.close();
  //ofs6.close();
  //ofs7.close();
  
  //std::cout << nTier1 << "," << nTier2 << "," << nTier3 << "\n";
  //std::cout << "\n" << nIXP << "\n";

  int n1TotalLink = 0;
  int n11Link = 0;
  int n12Link = 0;
  int n13Link = 0;
  int n1sLink = 0;
  int n2TotalLink = 0;
  int n21Link = 0;
  int n22Link = 0;
  int n23Link = 0;
  int n2sLink = 0;
  int n3TotalLink = 0;
  int n31Link = 0;
  int n32Link = 0;
  int n33Link = 0;
  int n3sLink = 0;
  std::unordered_set<int> tier1Set = MakeSetFromFile("tier1.txt");
  std::unordered_set<int> tier2Set = MakeSetFromFile("tier2.txt");
  std::unordered_set<int> tier3Set = MakeSetFromFile("tier3.txt");
  std::unordered_set<int> stubSet = MakeSetFromFile("stub.txt");

  /*
  for(std::unordered_set<int>::iterator it = tier1Set.begin(); it != tier1Set.end(); ++it)
  {
    //std::vector<int> c1 = GetCustomer(*it);
    std::vector<int> p1 = GetPeer(*it);
    for(std::vector<int>::iterator jt = p1.begin(); jt != p1.end(); ++jt)
    {
      ++n1TotalLink;
      if(tier1Set.find(*jt) != tier1Set.end())
      {
        ++n11Link;
      }
      else if(tier2Set.find(*jt) != tier2Set.end())
      {
        ++n12Link;
      }
      else if(tier3Set.find(*jt) != tier3Set.end())
      {
        ++n13Link;
      }
      else if(stubSet.find(*jt) != stubSet.end())
      {
        ++n1sLink;
      }
    }
  }
  */

  /*
  for(std::unordered_set<int>::iterator it = tier2Set.begin(); it != tier2Set.end(); ++it)
  {
    //std::vector<int> c2 = GetCustomer(*it);
    std::vector<int> p2 = GetPeer(*it);
    for(std::vector<int>::iterator jt = p2.begin(); jt != p2.end(); ++jt)
    {
      ++n2TotalLink;
      if(tier1Set.find(*jt) != tier1Set.end())
      {
        ++n21Link;
      }
      else if(tier2Set.find(*jt) != tier2Set.end())
      {
        ++n22Link;
      }
      else if(tier3Set.find(*jt) != tier3Set.end())
      {
        ++n23Link;
      }
      else if(stubSet.find(*jt) != stubSet.end())
      {
        ++n2sLink;
      }
    }
  }
  */

  for(std::unordered_set<int>::iterator it = tier3Set.begin(); it != tier3Set.end(); ++it)
  {
    //std::vector<int> c3 = GetCustomer(*it);
    std::vector<int> p3 = GetPeer(*it);
    if(p3.size() > 250)
    {
      continue;
    }
    for(std::vector<int>::iterator jt = p3.begin(); jt != p3.end(); ++jt)
    {
      ++n3TotalLink;
      if(tier1Set.find(*jt) != tier1Set.end())
      {
        ++n31Link;
      }
      else if(tier2Set.find(*jt) != tier2Set.end())
      {
        ++n32Link;
      }
      else if(tier3Set.find(*jt) != tier3Set.end())
      {
        ++n33Link;
      }
      else if(stubSet.find(*jt) != stubSet.end())
      {
        ++n3sLink;
      }
    }
  }

  std::cout << n3TotalLink << ", " << n31Link << ", " << n32Link << ", " << n33Link << ", " << n3sLink << "\n";

  //std::unordered_map<int, Node> base_map = map;
}
