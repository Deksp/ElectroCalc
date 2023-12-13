#include "node.h"

Node::Node()
{

}

uint Node::getId() const
{
    return m_id;
}

QString Node::getIndex() const
{
    return m_index;
}

Node *Node::getAssignedNode(Node *node)
{
    Q_UNUSED(node)
    return nullptr;
}

QDebug operator<<(QDebug output, const Node *obj)
{
    switch (obj->type())
    {
    case Node::TypeVertexNode:
        output.noquote()<<"Vertex_"
                          + QString().setNum(obj->getId());
        break;
    case Node::TypeGeneratorNode:
        output.noquote()<<"Generator_"
                          + QString().setNum(obj->getId());
        break;
    case Node::TypeGenBranchNode:
        output.noquote()<<"GenBranch_"
                          + QString().setNum(obj->getId());
        break;
    case Node::TypeBranchNode:
        output.noquote()<<"Branch_"
                          + QString().setNum(obj->getId());
        break;
    case Node::TypeLoadNode:
        output.noquote()<<"Load_"
                          + QString().setNum(obj->getId());
        break;
    default:
        break;
    }
    return output;
}

