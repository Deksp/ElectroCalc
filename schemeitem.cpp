#include <QPainter>
#include <QGraphicsEffect>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QSize>
#include <QDebug>

#include "schemeitem.h"
#include "schemescene.h"

//--------------------------------------------------------------------------

SchemeItem::SchemeItem(QString index)
   : m_block(false), m_stage(false),
     m_opacity(0.5), selectionGlow(new SelectionGlow(this)),
     m_node(nullptr), m_index(index)
{

}

void SchemeItem::setTransparent(bool transparent)
{
    m_opacity = transparent ? 0.5 : 1;
}

void SchemeItem::setBlock(bool block)
{
    m_block = block;
}

void SchemeItem::setNode(Node *node)
{
    m_node = node;
}

bool SchemeItem::isBlock() const
{
    return m_block;
}

void SchemeItem::setStage(bool stage)
{
    m_stage = stage;
}

void SchemeItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setOpacity(m_opacity);
    selectionGlow->update();
}

void SchemeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
    setPos(static_cast<SchemeScene*>(scene())->gridPoint(event->scenePos()));
}

//--------------------------------------------------------------------------

SelectionGlow::SelectionGlow(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
    blur->setBlurRadius(3);
    setGraphicsEffect(blur);
}

int SelectionGlow::type() const
{
    return QGraphicsItem::UserType;
}

void SelectionGlow::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (static_cast<SchemeItem*>(parentItem())->isBlock())
    {
        painter->setPen(QPen(QColor(255, 0, 0, 150), 5));
        painter->drawPath(parentItem()->shape());
    }
    else if (parentItem()->isSelected())
    {
        painter->setPen(QPen(QColor(0, 65, 255, 150), 5));
        painter->drawPath(parentItem()->shape());
    }
}

QRectF SelectionGlow::boundingRect() const {
    return parentItem()->boundingRect();
}

//--------------------------------------------------------------------------

VertexItem::VertexItem(QString index)
    : SchemeItem(index)
{
    QPainterPath form;
    m_rect = QRectF(-26.5, -26.5, 53, 53);
    form.addEllipse(m_rect);
    setPath(form);
    setZValue(0.2);
}

int VertexItem::type() const
{
    return TypeVertexItem;
}

void VertexItem::removeBranch(BranchItem *branch)
{

}

void VertexItem::removeBranchs()
{

}

void VertexItem::addBranch(BranchItem *branch)
{

}

void VertexItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(widget)
    SchemeItem::paint(painter, option);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse(m_rect);

    QFont font = painter->font();
    font.setPointSize(22);
    painter->setFont(font);
    painter->drawText(m_rect, Qt::AlignCenter, "V" + m_index);
}

//--------------------------------------------------------------------------

GeneratorItem::GeneratorItem(QString index)
    : VertexItem(index)
{
    QPainterPath form;
    m_rect = QRectF(-26.5, -26.5, 53, 53);
    form.addRect(m_rect);
    setPath(form);
    setZValue(0.2);
}

int GeneratorItem::type() const
{
    return TypeGeneratorItem;
}

void GeneratorItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(widget)
    SchemeItem::paint(painter, option);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRect(m_rect);

    QFont font = painter->font();
    font.setPointSize(22);
    painter->setFont(font);
    painter->drawText(m_rect, Qt::AlignCenter, "Е" + m_index);
}

//--------------------------------------------------------------------------

BranchItem::BranchItem(SchemeItem *startItem)
    : SchemeItem("non"), m_startItem(startItem),
      m_endItem(nullptr)
{
    m_line.setP1(m_startItem->pos());
    const quint16 width = 100;
    const quint16 height = 30;
    qreal xLine = boundingRect().width()/2 - width/2;
    qreal yLine = boundingRect().height()/2 - height/2;
    m_resistor = QRectF(xLine, yLine, width, height);
    setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap));
}

int BranchItem::type() const
{
    return TypeBranchItem;
}

void BranchItem::setP2(const QPointF point)
{
    QPainterPath form;
    form.moveTo(m_line.p1());
    form.lineTo(point + QPointF(0.5, 0.5));
    setPath(form);
    m_line.setP2(point + QPointF(0.5, 0.5));
}

void BranchItem::setEndItem(SchemeItem *item)
{
    m_endItem = item;
}

void BranchItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(widget)

    SchemeItem::paint(painter, option);

    painter->setPen(pen());
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(m_line.p1(), m_line.p2());
    painter->drawRect(m_resistor);
}

//--------------------------------------------------------------------------

LoadItem::LoadItem()
    : SchemeItem("non")
{

}

int LoadItem::type() const
{
    return 0;
}

void LoadItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{

}

//--------------------------------------------------------------------------
