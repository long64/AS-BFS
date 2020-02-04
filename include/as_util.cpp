#pragma once

#include "as_util.hpp"
#include "global.hpp"
#include "utility.hpp"
#include <vector>
#include <set>
#include <queue>

using namespace std;

vector<int> GetProvider(const int d)
{
    return asRel[d][1];
}

vector<int> GetPeer(const int d)
{
    return asRel[d][0];
}

vector<int> GetCustomer(const int d)
{
    return asRel[d][-1];
}

int GetLinkType(const int as1, const int as2)
{
    if (Find(asRel[as1][1], as2))
    {
        return 1;
    }
    else if (Find(asRel[as1][0], as2))
    {
        return 0;
    }
    else if (Find(asRel[as1][-1], as2))
    {
        return -1;
    }
    else
    {
        cout << "\n\nGetLinkType : Not Found\n\n";
        return -2;
    }
}

set<int> GetCustomerConeSet(const int asn)
{
    // recuresively adding customers

    set<int> s;
    queue<int> que;
    que.push(asn);
    while (not que.empty())
    {
        int n = que.front();
        que.pop();

        s.insert(n);

        vector<int> u = GetCustomer(n);
        for (vector<int>::iterator it = u.begin(); it != u.end(); ++it)
        {
            if (s.find(*it) == s.end())
            {
                que.push(*it);
            }
        }
    }
    s.erase(asn);
    return s;
}

vector<int> GetPath(const int s, const int d)
{
    vector<int> path;
    if (asMap.find(s) == asMap.end())
    {
        return path;
    }
    if (asMap[s].GetStage() == -1)
    {
        return path;
    }
    int n = s;
    while (n != d)
    {
        vector<int> v = asMap[n].GetParent();
        // tie break
        n = *min_element(v.begin(), v.end());
        path.push_back(n);
    }
    return path;
}

int GetNeighborForDest(const int s, const int d)
{
    vector<int> path = GetPath(s, d);
    if (path.size() == 0)
    {
        return -1;
    }
    else
    {
        return path[0];
    }
}

bool HasRoute(const int s, const int d)
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

bool HasOneProvider(const int d)
{
    if (asRel[d][1].size() == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool HasNoPeer(const int d)
{
    if (asRel[d][0].size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool IsStub(const int d)
{
    if (asRel[d][-1].size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool IsOneProviderStub(const int d)
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

bool IsStrictStub(const int d)
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

bool IsLimitedStub(const int d)
{
    vector<int> u = asRel[d][0];
    if (IsOneProviderStub(d) and not HasNoPeer(d))
    {
        for (vector<int>::iterator it = u.begin(); it != u.end(); ++it)
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

bool IsSemiStub(const int d)
{
    vector<int> u = asRel[d][-1];
    if (not IsStub(d) and HasNoPeer(d) and HasOneProvider(d))
    {
        for (vector<int>::iterator it = u.begin(); it != u.end(); ++it)
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

bool IsStubCone(const int d)
{
    vector<int> u = asCone[d];
    for (vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
        if (*it == d)
        {
            continue;
        }
        vector<int> v = GetProvider(*it);
        for (vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
        {
            if (not Find(u, *jt))
            {
                return false;
            }
        }
        v = GetPeer(*it);
        for (vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
        {
            if (not Find(u, *jt))
            {
                return false;
            }
        }
        v = GetCustomer(*it);
        for (vector<int>::iterator jt = v.begin(); jt != v.end(); ++jt)
        {
            if (not Find(u, *jt))
            {
                return false;
            }
        }
    }
    return true;
}

bool IsProvider(const int m, const int n)
{
    return Find(GetProvider(n), m);
}

bool IsPeer(const int m, const int n)
{
    return Find(GetPeer(n), m);
}

bool IsCustomer(const int m, const int n)
{
    return Find(GetCustomer(n), m);
}

bool IsConnected(const int m, const int n)
{
    return IsProvider(m, n) or IsPeer(m, n) or IsCustomer(m, n);
}

bool IsPathFromProvider(const int s, const vector<int> v)
{
    return Find(GetProvider(s), v[0]);
}

bool IsPathFromPeer(const int s, const vector<int> v)
{
    return Find(GetPeer(s), v[0]);
}

bool IsPathFromCustomer(const int s, const vector<int> v)
{
    return Find(GetCustomer(s), v[0]);
}

bool MayNeighborAnnounce(const int s, const int d, const int neighbor)
{
    vector<int> v = GetPath(neighbor, d);
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

int CountAvailablePath(const int s, const int d, const int type)
{
    int n = 0;
    vector<int> v;

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

    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        if (MayNeighborAnnounce(s, d, *it))
        {
            ++n;
        }
    }

    return n;
}

vector<int> GetAvailableNeighbor(const int s, const int d, const int type)
{
    vector<int> v;

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

    vector<int> neighbors;
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    {
        if (MayNeighborAnnounce(s, d, *it))
        {
            neighbors.push_back(*it);
        }
    }

    return neighbors;
}

void PrintAsInfo(const int asn)
{
    vector<int> provider = GetProvider(asn);
    vector<int> peer = GetPeer(asn);
    vector<int> customer = GetCustomer(asn);
    vector<int> cone = asCone[asn];

    cout << "Provider(" << provider.size() << "):";
    PrintVector(provider);
    cout << "\nPeer(" << peer.size() << "):";
    PrintVector(peer);
    cout << "\nCustomer(" << customer.size() << "):";
    PrintVector(customer);
    cout << "\nCustomer Cone(" << cone.size() << "):";
    PrintVector(cone);
    cout << "\n\n";
}

int CountPathViaAs(const int viaAs, const int d)
{
    int c = 0;
    for (const auto &[key, value] : asMap)
    {
        if (Find(GetPath(key, d), viaAs))
        {
            ++c;
        }
    }
    return c;
}