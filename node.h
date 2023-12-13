#ifndef NODE_H
#define NODE_H

#include <complex>
#include <QString>
#include <QDebug>

typedef std::complex<double> complexnum;

class Node
{
public:
    enum
    {
      TypeVertexNode,
      TypeGeneratorNode,
      UserType,
      TypeGenBranchNode,
      TypeBranchNode,
      TypeLoadNode
    };
    Node();
    virtual ~Node() = default;

    Node(Node &&) = delete;
    Node(const Node &) = delete;
    Node &operator=(Node &&) = delete;
    Node &operator=(const Node &) = delete;

    virtual uint getId() const;
    virtual int type() const = 0;
    virtual QString getIndex() const;
    virtual bool isLink(Node *node) const = 0;
    virtual complexnum getTypeNodeProperty() const = 0;
    virtual QString getStringTypeNodeProperty() const = 0;
    virtual Node *getAssignedNode(Node *node = nullptr);

    friend QDebug operator<<(QDebug output, const Node* obj);

protected:
    uint m_id;
    QString m_index;

    virtual void setId(uint id) = 0;
    virtual void setIndex(QString index) = 0;
};



#endif // NODE_H
