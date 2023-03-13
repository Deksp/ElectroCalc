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
    Node *addBranch(Node *firstNode, Node *secondNnode);
    Node *addLoad(Node *node);

    void setVoltage(Node *Generator, double voltage);
    void setLoad(Node *load, complexnum resistance);
    void setBranch(Node *branch, complexnum resistance);
    void setBranch(Node *node, Node* linkNode, complexnum resistance);

    void deleteBranch(Node *node, Node* linkNode);
    void deleteNode(Node *node);

    QVector<VertexNode *> getVertexes() const;
    QVector<GeneratorNode *> getGenerators() const;
    QVector<BranchNode *> getBranchs() const;
    QVector<LoadNode *> getLoads() const;
    QString getNextVIndex() const;
    QString getNextGIndex() const;
    QString getDefResistance() const;

    VertexNode *getVertex(uint id_1);
    GeneratorNode *getGenerator(uint id_1);
    BranchNode *getBranch(uint id_1);
    LoadNode *genLoad(uint id_1);

    static uint getQuanityLayout();

private:
    uint m_layoutId;
    static uint m_quanityLayout;
    static const int defaultValue = 1;

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

        virtual void addLoad(LoadNode *load);
        virtual void addBranch(BranchNode *branch);
        virtual const QVector<LoadNode*> getLoads() const;
        virtual const QVector<BranchNode*> getBranchs() const;
        virtual BranchNode *getBranchAt(Node *node);

        virtual int type() const override;
        virtual uint getId() const override;
        virtual void setId(unsigned int id) override;
        virtual void setIndex(QString index) override;
        virtual bool isLink(Node *node) const override;
        virtual complexnum getTypeNodeProperty() const override;
        virtual QString getStringTypeNodeProperty() const override;
        virtual Node *getAssignedNode(Node *node = nullptr) override;

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
        virtual Node *getAssignedNode(Node *node = nullptr) override;
        virtual complexnum getTypeNodeProperty() const override;
        QString getStringTypeNodeProperty() const override;

    private:
        double m_voltage;
        void addLoad(LoadNode *) override;
        const QVector<LoadNode *> getLoads() const override;
    };

//-------------------------------------------

    class BranchNode : public Node
    {
    public:
        BranchNode();
        BranchNode(Node *firstNode, Node *secondNode);
        virtual ~BranchNode() = default;
        virtual int type() const override;
        virtual void setId(uint id) override;
        virtual void setIndex(QString index) override;
        virtual complexnum getTypeNodeProperty() const override;
        virtual QString getStringTypeNodeProperty() const override;
        virtual Node *getAssignedNode(Node *node = nullptr) override;
        virtual bool isLink(Node *node) const override;
        virtual VertexNode *getFirstNode();
        virtual VertexNode *getSecondNode();

        virtual void setResistance(complexnum resistance);
        virtual complexnum getResistance() const;

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
        LoadNode(Node *node);
        ~LoadNode() = default;
        virtual int type() const override;
        virtual bool isLink(Node *node) const override;
        virtual Node *getAssignedNode(Node *node = nullptr) override;

    private:
        VertexNode *m_assignedNode;
    };

///-------------Inner_Classes-------------///
};

#endif // LAUOUYTSCHEM_H
