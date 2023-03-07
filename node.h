#ifndef NODE_H
#define NODE_H

#include <complex>
#include <QMultiMap>
#include <QVector>

#include "layoutscheme.h"

class INode;
class VertexNode;
class GeneratorNode;
class BranchNode;
class LoadNode;

typedef std::complex<double> complexnum;

//--------------------------------------------------------------------------

class Node
{
    friend LayoutScheme;
    Node();
public:
    enum
    {
      TypeVertexNode,
      TypeGeneratorNode,
      TypeBranchNode,
      TypeLoadNode
    };
    virtual ~Node() = default;

    Node(Node &&) = delete;
    Node(const Node &) = delete;
    Node &operator=(Node &&) = delete;
    Node &operator=(const Node &) = delete;

    virtual int getId();
    virtual int type() = 0;
    virtual QString getIndex();

protected:
    unsigned int m_id;
    QString m_index;
    virtual void setId(unsigned int id);
    virtual void setIndex(QString index);
};

//--------------------------------------------------------------------------

class VertexNode : public Node
{
public:
    VertexNode();
    virtual ~VertexNode() = default;

private:
    QVector<LoadNode *> load;
    virtual void addLoad();

protected:
    QVector<BranchNode *> branch;
    virtual void addBranch();
};

//--------------------------------------------------------------------------

class GeneratorNode : public VertexNode
{
public:
    GeneratorNode();
    ~GeneratorNode() = default;

private:
    double m_voltage;

protected:
    virtual void setVoltage(double voltage);
};

//--------------------------------------------------------------------------

class BranchNode : public Node
{
    friend VertexNode;
public:
    BranchNode();
    ~BranchNode() = default;

private:
    VertexNode *m_firstNode;
    VertexNode *m_secondNode;

protected:
    complexnum m_resistace;

};

//--------------------------------------------------------------------------

class LoadNode : public Node
{
    friend VertexNode;
public:
    LoadNode();
    ~LoadNode() = default;

private:
    VertexNode *m_assignedNode;

protected:
    complexnum m_resistace;
};

//--------------------------------------------------------------------------

#endif // NODE_H
