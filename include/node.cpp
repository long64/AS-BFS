#include "node.hpp"
#include <vector>

Node::Node(const int asn)
{
    m_asn = asn;
    m_depth = -1;
    m_stage = -1;
    m_searched = 0;
    m_flag = -1;
}

int Node::GetAsn()
{
    return m_asn;
}

std::vector<int> Node::GetParent()
{
    return m_parent;
}

void Node::AddParent(const int asn)
{
    m_parent.push_back(asn);
}

std::vector<int> Node::GetChild()
{
    return m_child;
}

void Node::AddChild(const int asn)
{
    m_child.push_back(asn);
}

int Node::GetDepth()
{
    return m_depth;
}

void Node::SetDepth(const int depth)
{
    m_depth = depth;
}

int Node::GetStage()
{
    return m_stage;
}

void Node::SetStage(const int stage)
{
    m_stage = stage;
}

int Node::GetSearched()
{
    return m_searched;
}

void Node::SetSearched(const int searched)
{
    m_searched = searched;
}

int Node::GetFlag()
{
    return m_flag;
}

void Node::SetFlag(const int flag)
{
    m_flag = flag;
}