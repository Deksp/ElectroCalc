#include "layoutscheme.h"
#include <QString>

uint LayoutScheme::m_quanityLayout = 0;

LayoutScheme::LayoutScheme()
{
    m_vertex.resize(0);
    m_generator.resize(0);
}

Node *LayoutScheme::addVertex()
{
    return nullptr;
}

Node *LayoutScheme::addGenerator()
{
    return nullptr;
}

Node *LayoutScheme::addBranch()
{
    return nullptr;
}

Node *LayoutScheme::addLoad()
{
    return nullptr;
}

void LayoutScheme::setVoltage(Node *Generator, double voltage)
{

}

void LayoutScheme::setLoad(Node *load, complexnum resistance)
{

}

void LayoutScheme::setLoad(Node *vertex, uint idLoad, complexnum load)
{

}

void LayoutScheme::setBranch(Node *branch, complexnum resistance)
{

}

void LayoutScheme::setBranch(Node *node, Node *linkNode, complexnum resistance)
{

}

void LayoutScheme::deleteBranch(Node *node, Node *linkNode)
{

}

void LayoutScheme::deleteNode(Node *node)
{

}

uint LayoutScheme::getQuanityVertexes() const
{
    return 0;
}

uint LayoutScheme::getQuanityGenerators() const
{
    return 0;
}

uint LayoutScheme::getQuanityBranchs() const
{
    return 0;
}

uint LayoutScheme::getQuanityLoads() const
{
    return 0;
}

QString LayoutScheme::getNextVIndex()
{
    return QString().setNum(m_vertex.size() + 1);
}

QString LayoutScheme::getNextGIndex()
{
    return QString(m_generator.size() + 1);
}

uint LayoutScheme::getQuanityLayout()
{
    return 0;
}

LayoutScheme::VertexNode::VertexNode()
{

}

void LayoutScheme::VertexNode::addLoad()
{

}

void LayoutScheme::VertexNode::addBranch()
{

}

int LayoutScheme::VertexNode::type() const
{
    return 0;
}

uint LayoutScheme::VertexNode::getId() const
{
    return 0;
}

void LayoutScheme::VertexNode::setId(unsigned int id)
{

}

void LayoutScheme::VertexNode::setIndex(QString index)
{

}

complexnum LayoutScheme::VertexNode::getTypeNodeProperty() const
{
    return 0;
}

LayoutScheme::GeneratorNode::GeneratorNode()
{

}

void LayoutScheme::GeneratorNode::setVoltage(double voltage)
{

}

int LayoutScheme::GeneratorNode::type() const
{
    return 0;
}

void LayoutScheme::GeneratorNode::addLoad()
{
    return;
}

LayoutScheme::BranchNode::BranchNode()
{

}

void LayoutScheme::BranchNode::setResistance()
{

}

LayoutScheme::LoadNode::LoadNode()
{

}
