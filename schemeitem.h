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
    void setNode(Node *node);
    void setAllowed(bool);

    bool isBlock() const;
    bool isStage() const;
    bool isAllowed() const;

    Node *getNode() const;

private:
    bool m_block, m_stage, m_allowed;
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
    virtual void addBranch(BranchItem *branch);
    virtual void addLoad(LoadItem *load);

private:
    QVector<LoadItem *> m_load;

protected:
    QRectF m_rect;
    QVector<BranchItem *> m_branch;

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
    void addBranch(BranchItem *branch) override;

private:
    void addLoad(LoadItem *) override;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

//--------------------------------------------------------------------------

class BranchItem : public SchemeItem
{
public:
    BranchItem(SchemeItem *const strartItem, const QString &resistsnce);
    BranchItem(SchemeItem *const strartItem, SchemeItem *const endItem, const QString &resistsnce);
    int type() const override;

    const SchemeItem *getStartItem() const;
    const SchemeItem *getEndItem() const;

    void moveStartPoint(const QPointF &start);
    void moveEndPoint(const QPointF &end);
    void setEndItem(SchemeItem *const endItem);
    void setResText(QString &resistsnce);

private:
    QPointF m_start;
    QPointF m_end;
    QLineF m_line;
    QPolygonF m_resistor;
    QPainterPath m_path;
    QString m_resistsnce;
    SchemeItem *m_startItem;
    SchemeItem *m_endItem; 

    void movePoint(const QPointF &start, const QPointF &end);

    static const quint16 resistor_width = 80;
    static const quint16 resistor_height = 30;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

//--------------------------------------------------------------------------

class LoadItem : public SchemeItem
{
public:
    LoadItem(const QString &resistance);
    void setItem(SchemeItem *const item);
    const SchemeItem *getItem();
    int type() const override;
    void setResText(QString &resistsnce);
    void moveLoad(const QPointF &point);
    void movableItem(const QPointF &point);

private:
    QRectF m_resistor;
    QString m_resistsnce;
    QPointF m_start, m_end;
    QPainterPath m_path;
    SchemeItem *m_item;

    static const quint16 resistor_width = 80;
    static const quint16 resistor_height = 30;

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

//--------------------------------------------------------------------------

#endif // SCHEMITEM_H
