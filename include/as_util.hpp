#pragma once

#include <vector>
#include <set>

using namespace std;

vector<int> GetProvider(const int d);

vector<int> GetPeer(const int d);

vector<int> GetCustomer(const int d);

int GetLinkType(const int as1, const int as2);

set<int> GetCustomerConeSet(const int asn);

vector<int> GetPath(const int s, const int d);

int GetNeighborForDest(const int s, const int d);

bool HasRoute(const int s, const int d);

bool HasOneProvider(const int d);

bool HasNoPeer(const int d);

bool IsStub(const int d);

bool IsOneProviderStub(const int d);

bool IsStrictStub(const int d);

bool IsLimitedStub(const int d);

bool IsSemiStub(const int d);

bool IsStubCone(const int d);

bool IsProvider(const int m, const int n);

bool IsPeer(const int m, const int n);

bool IsCustomer(const int m, const int n);

bool IsConnected(const int m, const int n);

bool IsPathFromProvider(const int s, const vector<int> v);

bool IsPathFromPeer(const int s, const vector<int> v);

bool IsPathFromCustomer(const int s, const vector<int> v);

bool MayNeighborAnnounce(const int s, const int d, const int neighbor);

int CountAvailablePath(const int s, const int d, const int type);

vector<int> GetAvailableNeighbor(const int s, const int d, const int type);

void PrintAsInfo(const int asn);

int CountPathViaAs(const int viaAs, const int d);