#pragma once

#include "utility.hpp"
#include "as_util.hpp"
#include "node.hpp"
#include "global.hpp"
#include <vector>
#include <queue>

using namespace std;

vector<int> AddChildNodes(int parentASN, int stage)
{
    Node parent = asMap[parentASN];
    vector<int> u;
    vector<int> v;
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
    for (vector<int>::iterator it = u.begin(); it != u.end(); ++it)
    {
        Node child = asMap[*it];
        if (child.GetStage() == stage and child.GetDepth() == parent.GetDepth() + 1)
        {
            child.AddParent(parentASN);
            parent.AddChild(*it);
            asMap[*it] = child;
            asMap[parentASN] = parent;
            v.push_back(*it);
            // std::cout << *it << "(" << child.parentsASNs.size() << "),";
        }
        else if (child.GetStage() == -1)
        {
            // child.myASN = *it;
            child.AddParent(parentASN);
            child.SetDepth(parent.GetDepth() + 1);
            child.SetStage(stage);
            parent.AddChild(*it);
            asMap[*it] = child;
            asMap[parentASN] = parent;
            v.push_back(*it);
            // std::cout << *it << "(" << child.parentsASNs.size() << "),";
        }
    }
    return v; // added child nodes
}

void BFS(int destASN, int stage)
{
    queue<int> que;
    que.push(destASN);
    Node node_d = asMap[destASN];
    node_d.SetSearched(stage);
    asMap[destASN] = node_d;

    int nPathViaLink = 0;
    queue<int> pathQueue;

    while (que.size() > 0)
    {
        int asn = que.front();
        que.pop();

        // for queued AS
        if (asMap[asn].GetStage() == stage and FindLink(GetPath(asn, destASN), linkEdge1, linkEdge2))
        {
            ++nPathViaLink;
        }

        /*
        if(IsStub(asn) and asn != destASN)
        {
            continue;
        }
        */

        vector<int> v = asMap[asn].GetChild();
        for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
            if (asMap[*it].GetSearched() < stage)
            {
                /*
                if(not IsStub(*it))
                {
                    que.push(*it);
                }
                */
                que.push(*it);
                Node child = asMap[*it];
                child.SetSearched(stage);
                asMap[*it] = child;
            }
        }

        v = AddChildNodes(asn, stage);

        if (stage == 1 or stage == 3) // peer not queued
        {
            for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
            {
                if (asMap[*it].GetSearched() < stage)
                {
                    if (IsStub(*it)) // only for stage == 1
                    {
                        // for not queued
                        pathQueue.push(*it);
                    }
                    else
                    {
                        que.push(*it);
                    }
                    // que.push(*it);
                    Node child = asMap[*it];
                    child.SetSearched(stage);
                    asMap[*it] = child;
                }
            }
        }
        else if (stage == 2)
        {
            for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
            {
                if (asMap[*it].GetSearched() < stage)
                {
                    Node child = asMap[*it];
                    child.SetSearched(stage);
                    asMap[*it] = child;

                    // for not queued
                    pathQueue.push(*it);
                }
            }
        }
    }

    // for not queued
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
    Node node_d(d);
    node_d.SetDepth(0);
    node_d.SetStage(0);
    node_d.SetSearched(0);
    node_d.SetFlag(-1);
    asMap[d] = node_d;
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