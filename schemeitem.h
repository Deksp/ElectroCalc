#ifndef SCHEMITEM_H
#define SCHEMITEM_H

#include <QGraphicsItem>

#include "node.h"

class SchemeItem;
class SelectionGlow;
class VertexItem;
class GeneratorItem;
class BranchItem;
class LoadItem;

//--------------------------------------------------------------------------

class SchemeItem : public QGraphicsPathItem
{
public:
    enum
    {
      SchemeType = QGraphicsItem::UserType,
      TypeVertexItem,
      TypeGeneratorItem,
      TypeBranchItem,
      TypeLoadItem
    };

    SchemeItem(QString index);
    virtual ~SchemeItem() = default;

    void setTransparent(bool opacity);
    void setStage(bool);
    void setBlock(bool);
    void setNode(Node *node);

    bool isBlock() const;

private:
    bool m_block, m_stage;
    qreal m_opacity;
    SelectionGlow *selectionGlow;

protected:
    Node *m_node;
    QString m_index;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

//--------------------------------------------------------------------------

class SelectionGlow : public QGraphicsItem
{
public:
    SelectionGlow(QGraphicsItem* parent = nullptr);
    int type() const override;

protected:

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
};

//--------------------------------------------------------------------------

class VertexItem : public SchemeItem
{
public:
    VertexItem(QString index);
    virtual ~VertexItem() = default;
    int type() const override;
    void removeBranch(BranchItem *branch);
    void removeBranchs();
    void addBranch(BranchItem *branch);

private:
    QVector<BranchItem *> m_branch;

protected:
    QRectF m_rect;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

//--------------------------------------------------------------------------

class GeneratorItem : public VertexItem
{
public:
    GeneratorItem(QString index);
    int type() const override;

private:
    //QRectF m_rect;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

//--------------------------------------------------------------------------

class BranchItem : public SchemeItem
{
public:
    BranchItem(SchemeItem *startItem);
    int type() const override;

    void setP2(const QPointF point);
    void setEndItem(SchemeItem *item);

private:
    QLineF m_line;
    QRectF m_resistor;
    SchemeItem *m_startItem;
    SchemeItem *m_endItem;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

//--------------------------------------------------------------------------

class LoadItem : public SchemeItem
{
public:
    LoadItem();
    int type() const override;

private:
    QRectF m_resistor;
    QPolygonF m_arrow;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

//--------------------------------------------------------------------------

#endif // SCHEMITEM_H
