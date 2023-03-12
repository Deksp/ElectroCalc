#ifndef LAUOUYTSCHEM_H
#define LAUOUYTSCHEM_H

#include <QVector>

#include "node.h"

class LayoutScheme
{
///----Inner_Classes----///
    class VertexNode;
    class GeneratorNode;
    class BranchNode;
    class LoadNode;
///----Inner_Classes----///
public:
    LayoutScheme();

    Node *addVertex();
    Node *addGenerator();
    Node *addBranch(const Node *startNode, const Node *endNnode);
    Node *addLoad();

    void setVoltage(Node *Generator, double voltage);
    void setLoad(Node *load, complexnum resistance);
    void setLoad(Node *vertex, uint idLoad, complexnum load);
    void setBranch(Node *branch, complexnum resistance);
    void setBranch(Node *node, Node* linkNode, complexnum resistance);

    void deleteBranch(Node *node, Node* linkNode);
    void deleteNode(Node *node);

    uint getQuanityVertexes() const;
    uint getQuanityGenerators() const;
    uint getQuanityBranchs() const;
    uint getQuanityLoads() const;
    QString getNextVIndex();
    QString getNextGIndex();

    static uint getQuanityLayout();

private:
    uint m_layoutId;
    static uint m_quanityLayout;

    QVector<VertexNode *> m_vertex;
    QVector<GeneratorNode *> m_generator;
    QVector<BranchNode *> m_branch;
    QVector<LoadNode *> m_load;

///-------------Inner_Classes-------------///

    class VertexNode : public Node
    {
    public:
        VertexNode();
        virtual ~VertexNode() = default;

        virtual void addLoad();
        virtual void addBranch();

        virtual int type() const override;
        virtual uint getId() const override;
        virtual void setId(unsigned int id) override;
        virtual void setIndex(QString index) override;
        virtual complexnum getTypeNodeProperty() const override;

    private:
        QVector<LoadNode *> m_load;

    protected:
        QVector<BranchNode *> m_branch;
    };

//-------------------------------------------

    class GeneratorNode : public VertexNode
    {
    public:
        GeneratorNode();
        ~GeneratorNode() = default;
        virtual void setVoltage(double voltage);

        virtual int type() const override;

    private:
        double m_voltage;
        void addLoad() override;
    };

//-------------------------------------------

    class BranchNode : public Node
    {
    public:
        BranchNode();
        virtual ~BranchNode() = default;

        virtual void setResistance();

    private:
        VertexNode *m_firstNode;
        VertexNode *m_secondNode;

    protected:
        complexnum m_resistace;

    };

//-------------------------------------------

    class LoadNode : public BranchNode
    {
    public:
        LoadNode();
        ~LoadNode() = default;

    private:
        VertexNode *m_assignedNode;
    };

///-------------Inner_Classes-------------///
};

#endif // LAUOUYTSCHEM_H
