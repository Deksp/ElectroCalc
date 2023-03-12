#ifndef SCHEMITEM_H
#define SCHEMITEM_H

#include <QGraphicsItem>
#include <QGraphicsObject>

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
      SelectionGlowType,
      SchemeType = QGraphicsItem::UserType,
      TypeBranchItem,
      TypeLoadItem,
      TypeVertexItem,
      TypeGeneratorItem
    };

    SchemeItem(QString index);
    virtual ~SchemeItem() = default;

    void setTransparent(bool opacity);
    void setStage(bool);
    void setBlock(bool);
    void setNode(const Node *node);
    void setAllowed(bool);

    bool isBlock() const;
    bool isStage() const;
    bool isAllowed() const;

    const Node *getNode() const;

private:
    bool m_block, m_stage, m_allowed;
    qreal m_opacity;
    SelectionGlow *selectionGlow;

protected:
    const Node *m_node;
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
    void addLoad(LoadItem *load);

protected:
    QRectF m_rect;
    QVector<BranchItem *> m_branch;
    QVector<LoadItem *> m_load;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
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
    BranchItem(SchemeItem *const strartItem);
    BranchItem(SchemeItem *const strartItem, SchemeItem *const endItem);
    int type() const override;

    const SchemeItem *getStartItem() const;
    const SchemeItem *getEndItem() const;

    void moveStartPoint(const QPointF &start);
    void moveEndPoint(const QPointF &end);
    void setEndItem(SchemeItem *const endItem);

private:
    QPointF m_start;
    QPointF m_end;
    QLineF m_line;
    QPolygonF m_resistor;
    QPainterPath m_path;
    SchemeItem *m_startItem;
    SchemeItem *m_endItem; 

    void movePoint(const QPointF &start, const QPointF &end);

    static const quint16 resistor_width = 80;
    static const quint16 resistor_height = 30;

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
    void setItem(SchemeItem *const item);
    const SchemeItem *getItem();
    int type() const override;

private:
    QRectF m_resistor;
    SchemeItem *m_item;

    static const quint16 resistor_width = 80;
    static const quint16 resistor_height = 30;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

//--------------------------------------------------------------------------

#endif // SCHEMITEM_H
