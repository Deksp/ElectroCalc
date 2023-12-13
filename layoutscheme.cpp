#include "layoutscheme.h"
#include <QString>

uint LayoutScheme::m_quanityLayout = 0;

//--------------------------------------------------------------------------

LayoutScheme::LayoutScheme()
{
    m_vertex.resize(0);
    m_generator.resize(0);
    m_branch.resize(0);
    m_load.resize(0);
}

LayoutScheme::~LayoutScheme()
{
    for (auto it : m_vertex)
        delete it;
    for (auto it : m_generator)
        delete it;
    for (auto it : m_branch)
        delete it;
    for (auto it : m_load)
        delete it;
}

Node *LayoutScheme::addVertex()
{
    m_vertex << new VertexNode;
    m_vertex.last()->setId(m_vertex.size());
    m_vertex.last()->setIndex("V"+QString().setNum(m_vertex.size()));
    return m_vertex.last();
}

Node *LayoutScheme::addGenerator()
{
    m_generator << new GeneratorNode;
    m_generator.last()->setId(m_generator.size());
    m_generator.last()->setIndex("E"+QString().setNum(m_generator.size()));
    m_generator.last()->setVoltage(220);
    return m_generator.last();
}

Node *LayoutScheme::addBranch(Node *firstNode, Node *secondNode)
{
    if (firstNode == secondNode ||
            (firstNode->type() == Node::TypeGeneratorNode &&
             secondNode->type() == Node::TypeGeneratorNode))
        return nullptr;
    if (firstNode->type() == Node::TypeGeneratorNode ||
            secondNode->type() == Node::TypeGeneratorNode)
    {
        if (firstNode->type() == Node::TypeGeneratorNode &&
                static_cast<VertexNode*>(firstNode)->getBranchs().size()==1)
            return nullptr;
        if (secondNode->type() == Node::TypeGeneratorNode &&
                static_cast<VertexNode*>(secondNode)->getBranchs().size()==1)
            return nullptr;
    }
    m_branch << new BranchNode(firstNode, secondNode);
    m_branch.last()->setId(m_branch.size());
    m_branch.last()->setResistance(complexnum(defaultValue));
    static_cast<VertexNode*>(firstNode)->addBranch(m_branch.last());
    static_cast<VertexNode*>(secondNode)->addBranch(m_branch.last());
    return m_branch.last();
}

Node *LayoutScheme::addLoad(Node *node)
{
    if (node->type() != Node::TypeVertexNode)
        return nullptr;
    m_load << new LoadNode(node);
    static_cast<VertexNode*>(node)->addLoad(m_load.last());
    m_load.last()->setResistance(defaultValue);
    return m_load.last();
}

void LayoutScheme::setVoltage(Node *generator, complexnum voltage)
{
    if (generator->type() == Node::TypeGeneratorNode)
        static_cast<GeneratorNode*>(generator)->setVoltage(voltage);
}

void LayoutScheme::setLoad(Node *load, complexnum resistance)
{
    if (load->type() == Node::TypeLoadNode)
        static_cast<LoadNode*>(load)->setResistance(resistance);
}

void LayoutScheme::setBranch(Node *branch, complexnum resistance)
{
    static_cast<BranchNode*>(branch)->setResistance(resistance);
}

void LayoutScheme::setPoverful(Node *load, complexnum powerful)
{
    static_cast<LoadNode*>(load)->setPowerful(powerful);
}

void LayoutScheme::deleteNode(Node *node)
{
    switch (node->type())
    {
    case Node::TypeVertexNode:
        m_vertex.removeOne(static_cast<VertexNode*>(node));
        break;
    case Node::TypeGeneratorNode:
        m_generator.removeOne(static_cast<GeneratorNode*>(node));
        break;
    case Node::TypeBranchNode:
    case Node::TypeGenBranchNode:
        m_branch.removeOne(static_cast<BranchNode*>(node));
        break;
    case Node::TypeLoadNode:
        m_load.removeOne(static_cast<LoadNode*>(node));
        break;
    }
}

complexnum LayoutScheme::getVoltage(Node *generator) const
{
    return static_cast<GeneratorNode*>(generator)->getVoltage();
}

QVector<LayoutScheme::VertexNode *> LayoutScheme::getVertexes() const
{
    return m_vertex;
}

QVector<LayoutScheme::GeneratorNode *> LayoutScheme::getGenerators() const
{
    return m_generator;
}

QVector<LayoutScheme::BranchNode *> LayoutScheme::getBranchs() const
{
    return m_branch;
}

QVector<LayoutScheme::LoadNode *> LayoutScheme::getLoads() const
{
    return m_load;
}

QString LayoutScheme::getNextVIndex() const
{
    return QString().setNum(m_vertex.size() + 1);
}

QString LayoutScheme::getNextGIndex() const
{
    return QString().setNum(m_generator.size() + 1);
}

QString LayoutScheme::getDefResistance() const
{
    if (defaultValue>=0)
        return QString("%1+%2i Ω").
                arg(QString::number(defaultValue),
                    QString::number(defaultValue));
    return QString("%1%2i Ω").
            arg(QString::number(defaultValue),
                QString::number(defaultValue));
}

LayoutScheme::VertexNode *LayoutScheme::getVertex(uint id_1)
{
    return m_vertex[id_1];
}

LayoutScheme::GeneratorNode *LayoutScheme::getGenerator(uint id_1)
{
    return m_generator[id_1];
}

LayoutScheme::BranchNode *LayoutScheme::getBranch(uint id_1)
{
    return m_branch[id_1];
}

LayoutScheme::LoadNode *LayoutScheme::genLoad(uint id_1)
{
    return m_load[id_1];
}

uint LayoutScheme::getQuanityLayout()
{
    return 0;
}

QList<Dpair> LayoutScheme::serializationNode(Node *node)
{
    switch (node->type())
    {
    case Node::TypeVertexNode:
        return QList<Dpair>()<< Dpair(node->type(),0) << Dpair(node->getId(),0);
        break;
    case Node::TypeGeneratorNode:
        return QList<Dpair>()<< Dpair(node->type(),0) << Dpair(node->getId(),0) <<
               Dpair(static_cast<GeneratorNode*>(node)->getVoltage().real(),
                     static_cast<GeneratorNode*>(node)->getVoltage().imag());
        break;
    case Node::TypeBranchNode:
        return QList<Dpair>()<< Dpair(node->type(),0) <<
               Dpair(static_cast<BranchNode*>(node)->getFirstNode()->getId(),0) <<
               Dpair(static_cast<BranchNode*>(node)->getSecondNode()->getId(),0) <<
               Dpair(static_cast<BranchNode*>(node)->getResistance().real(),
                     static_cast<BranchNode*>(node)->getResistance().imag());
        break;
    case Node::TypeGenBranchNode:
        return QList<Dpair>()<< Dpair(node->type(),0) <<
               Dpair(static_cast<BranchNode*>(node)->getFirstNode()->getId(),0) <<
               Dpair(static_cast<BranchNode*>(node)->getSecondNode()->getId(),0) <<
               Dpair(static_cast<BranchNode*>(node)->getResistance().real(),
                     static_cast<BranchNode*>(node)->getResistance().imag());
        break;
    case Node::TypeLoadNode:
        return QList<Dpair>()<< Dpair(node->type(),0) <<
               Dpair(static_cast<LoadNode*>(node)->getAssignedNode()->getId(),0) <<
               Dpair(static_cast<LoadNode*>(node)->getResistance().real(),
                     static_cast<LoadNode*>(node)->getResistance().imag()) <<
               Dpair(static_cast<LoadNode*>(node)->getPowerful().real(),
                     static_cast<LoadNode*>(node)->getPowerful().imag());
        break;
    default:
        break;
    }
    return QList<Dpair>();
}

Node *LayoutScheme::deserializationNode(QList<Dpair> list)
{
    Dpair type;
    type = list[0];
    Node *node = nullptr;
    switch (int(type.first))
    {
    case Node::TypeVertexNode:
    {
        node = addVertex();
    }
        break;
    case Node::TypeGeneratorNode:
    {
        node = addGenerator();
        static_cast<GeneratorNode*>(node)->setVoltage(
                    complexnum(list[2].first, list[2].second));
    }
        break;
    case Node::TypeBranchNode:
    {
        Node *firstNode = getVertex(list[1].first-1);
        Node *secondNode = getVertex(list[2].first-1);
        node = addBranch(firstNode, secondNode);
        static_cast<BranchNode*>(node)->setResistance(
                    complexnum(list[3].first, list[3].second));
    }
        break;
    case Node::TypeGenBranchNode:
    {
        Node *firstNode = getGenerator(list[1].first-1);
        Node *secondNode = getVertex(list[2].first-1);
        node = addBranch(firstNode, secondNode);
        static_cast<BranchNode*>(node)->setResistance(
                    complexnum(list[3].first, list[3].second));
    }
        break;
    case Node::TypeLoadNode:
    {
        Node *assignNode = getVertex(list[1].first-1);
        node = addLoad(assignNode);
        auto resistance = complexnum(list[2].first, list[2].second);
        auto powerful = complexnum(list[3].first, list[3].second);
        complexnum Ysopr;
        auto num = static_cast<LoadNode*>(node)->getAssignedNode()->getId();
        switch (num) {
        case 3:
        case 4:
        case 5:
        default:
            static_cast<LoadNode*>(node)->setResistance(resistance);
            static_cast<LoadNode*>(node)->setPowerful(powerful);
        }

        //static_cast<LoadNode*>(node)->setResistance(resistance);
        //static_cast<LoadNode*>(node)->setPowerful(powerful);
    }
        break;
    default:
        break;
    }
    return node;
}

//--------------------------------------------------------------------------

LayoutScheme::VertexNode::VertexNode()
{

}

void LayoutScheme::VertexNode::addLoad(LayoutScheme::LoadNode *load)
{
    m_load << load;
    load->setId(m_load.size());
}

void LayoutScheme::VertexNode::addBranch(BranchNode *branch)
{
    m_branch << branch;
}

const QVector<LayoutScheme::LoadNode *> LayoutScheme::VertexNode::getLoads() const
{
    return m_load;
}

const QVector<LayoutScheme::BranchNode *> LayoutScheme::VertexNode::getBranchs() const
{
    return m_branch;
}

LayoutScheme::BranchNode *LayoutScheme::VertexNode::getBranchAt(Node *node)
{
    for (auto branch : m_branch)
        if (branch->getFirstNode() == node ||
                branch->getSecondNode() == node)
            return branch;
    return nullptr;
}

int LayoutScheme::VertexNode::type() const
{
    return TypeVertexNode;
}

uint LayoutScheme::VertexNode::getId() const
{
    return m_id;
}

void LayoutScheme::VertexNode::setId(unsigned int id)
{
    m_id = id;
}

void LayoutScheme::VertexNode::setIndex(QString index)
{
    m_index = index;
}

bool LayoutScheme::VertexNode::isLink(Node *node) const
{
    for (auto branch : m_branch)
        if (branch->getFirstNode() == node ||
                branch->getSecondNode() == node)
            return true;
    return false;
}

complexnum LayoutScheme::VertexNode::getTypeNodeProperty() const
{
    if (m_load.size()==1)
        return m_load.last()->getResistance();
}

QString LayoutScheme::VertexNode::getStringTypeNodeProperty() const
{
    if (m_load.size()==1)
        return QString("%1%2i Ω").
                arg(QString::number(m_load.last()->getResistance().real()),
                    QString::number(m_load.last()->getResistance().imag()));
    return QString("");
}

Node *LayoutScheme::VertexNode::getAssignedNode(Node *node)
{
    if (node == nullptr)
        return nullptr;
    if (!m_branch.contains(static_cast<BranchNode*>(node)))
        return nullptr;
    if (node->type() != Node::TypeGenBranchNode &&
            node->type() != Node::TypeBranchNode)
        return nullptr;
    if (static_cast<BranchNode*>(node)->getFirstNode() == this)
        return static_cast<BranchNode*>(node)->getSecondNode();
    return static_cast<BranchNode*>(node)->getFirstNode();
}

//--------------------------------------------------------------------------

LayoutScheme::GeneratorNode::GeneratorNode()
{

}

void LayoutScheme::GeneratorNode::setVoltage(complexnum voltage)
{
    m_voltage = voltage;
}

complexnum LayoutScheme::GeneratorNode::getVoltage() const
{
    return m_voltage;
}

int LayoutScheme::GeneratorNode::type() const
{
    return TypeGeneratorNode;
}

Node *LayoutScheme::GeneratorNode::getAssignedNode(Node *node)
{
    Q_UNUSED(node)
    if (m_branch.at(0)->getFirstNode() == this)
        return m_branch.at(0)->getSecondNode();
    return m_branch.at(0)->getFirstNode();
}

complexnum LayoutScheme::GeneratorNode::getTypeNodeProperty() const
{
    return m_branch.at(0)->getResistance();
}

QString LayoutScheme::GeneratorNode::getStringTypeNodeProperty() const
{
    if (m_voltage.imag() == 0)
        return QString("%1 v").arg(QString::number(m_voltage.real()));
    if (m_voltage.imag() > 0)
        return QString("%1+%2i v").arg(QString::number(m_voltage.real()), QString::number(m_voltage.imag()));
    return QString("%1%2i v").arg(QString::number(m_voltage.real()), QString::number(m_voltage.imag()));
}

void LayoutScheme::GeneratorNode::addLoad(LoadNode *)
{
    return;
}

const QVector<LayoutScheme::LoadNode *> LayoutScheme::GeneratorNode::getLoads() const
{
    return VertexNode::getLoads();
}

//--------------------------------------------------------------------------

LayoutScheme::BranchNode::BranchNode()
    : m_resistace(defaultValue)
{

}

LayoutScheme::BranchNode::BranchNode(Node *firstNode, Node *secondNode)
    : m_resistace(defaultValue)
{
    m_firstNode = static_cast<VertexNode*>(firstNode);
    m_secondNode = static_cast<VertexNode*>(secondNode);
    setResistance(0);
}

int LayoutScheme::BranchNode::type() const
{
    if (m_firstNode->type() == TypeGeneratorNode ||
            m_secondNode->type() == TypeGeneratorNode)
        return TypeGenBranchNode;
    return TypeBranchNode;
}

void LayoutScheme::BranchNode::setId(uint id)
{
    m_id = id;
}

void LayoutScheme::BranchNode::setIndex(QString index)
{
    m_index = index;
}

complexnum LayoutScheme::BranchNode::getTypeNodeProperty() const
{
    return m_resistace;
}

QString LayoutScheme::BranchNode::getStringTypeNodeProperty() const
{
    if (m_resistace.imag()>=0)
        return QString("%1+%2i Ω").
                arg(QString::number(m_resistace.real()),
                    QString::number(m_resistace.imag()));
    return QString("%1%2i Ω").
            arg(QString::number(m_resistace.real()),
                QString::number(m_resistace.imag()));
}

LayoutScheme::VertexNode *LayoutScheme::BranchNode::getFirstNode()
{
    if (type() == TypeGenBranchNode)
    {
        if (m_firstNode->type() == TypeGeneratorNode)
            return m_firstNode;
        return m_secondNode;
    }
    return m_firstNode;
}

LayoutScheme::VertexNode *LayoutScheme::BranchNode::getSecondNode()
{
    if (type() == TypeGenBranchNode)
    {
        if (m_firstNode->type() == TypeGeneratorNode)
            return m_secondNode;
        return m_firstNode;
    }
    return m_secondNode;
}

Node *LayoutScheme::BranchNode::getAssignedNode(Node *node)
{
    if (m_firstNode == node || m_secondNode == node)
    {
        if (m_firstNode == node)
            return  m_secondNode;
        return m_firstNode;
    }
    return nullptr;
}

bool LayoutScheme::BranchNode::isLink(Node *node) const
{
    if (m_firstNode == node || m_secondNode == node)
        return true;
    return false;
}

void LayoutScheme::BranchNode::setResistance(complexnum resistance)
{
    m_resistace = resistance;
}

complexnum LayoutScheme::BranchNode::getResistance() const
{
    return m_resistace;
}

//--------------------------------------------------------------------------

LayoutScheme::LoadNode::LoadNode(Node *node)
{
    m_assignedNode = static_cast<VertexNode*>(node);
}

int LayoutScheme::LoadNode::type() const
{
    return TypeLoadNode;
}

bool LayoutScheme::LoadNode::isLink(Node *node) const
{
    if (m_assignedNode == node)
        return  true;
    return false;
}

Node *LayoutScheme::LoadNode::getAssignedNode(Node *node)
{
    Q_UNUSED(node)
    return m_assignedNode;
}

void LayoutScheme::LoadNode::setPowerful(complexnum powerful)
{
    this->powerful = powerful;
}

complexnum LayoutScheme::LoadNode::getPowerful()
{
    return powerful;
}

QString LayoutScheme::LoadNode::getStringPowerful()
{
    if (m_resistace.imag()>=0)
        return QString("%1+%2i").
                arg(QString::number(powerful.real()),
                    QString::number(powerful.imag()));
    return QString("%1%2i").
            arg(QString::number(powerful.real()),
                QString::number(powerful.imag()));
}
