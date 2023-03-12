#include <QPainter>
#include <QGraphicsEffect>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QSize>
#include <QDebug>
#include <QtMath>

#include "schemeitem.h"
#include "schemescene.h"

//--------------------------------------------------------------------------

SchemeItem::SchemeItem(QString index)
   : m_block(false), m_stage(false), m_allowed(false),
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

void SchemeItem::setNode(const Node *node)
{
    m_node = node;
}

void SchemeItem::setAllowed(bool allowed)
{
    m_allowed = allowed;
}

bool SchemeItem::isBlock() const
{
    return m_block;
}

bool SchemeItem::isStage() const
{
    return m_stage;
}

bool SchemeItem::isAllowed() const
{
    return m_allowed;
}

const Node *SchemeItem::getNode() const
{
    return m_node;
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
    return SchemeItem::SelectionGlowType;
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
    else if (static_cast<SchemeItem*>(parentItem())->isAllowed())
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
    m_branch << branch;
}

void VertexItem::addLoad(LoadItem *load)
{
    m_load << load;
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

void VertexItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
    setPos(static_cast<SchemeScene*>(scene())->gridPoint(event->scenePos()));
    QPointF position(scenePos());
    for (auto branch : m_branch)
    {
        if (branch->getEndItem() == this)
        {
            branch->moveEndPoint(position);
        }
        if (branch->getStartItem() == this)
        {
            branch->moveStartPoint(position);
        }
    }
    for (auto load : m_load)
        load->setPos(position);
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
BranchItem::BranchItem(SchemeItem *const startItem)
    : SchemeItem("Branch"), m_start(), m_end(),
      m_line(0,0,0,0), m_resistor(), m_path(),
      m_startItem(startItem), m_endItem(nullptr)
{
    setTransparent(false);
    setPos(m_startItem->scenePos());
    //moveStartPoint(scenePos());
    moveEndPoint(m_startItem->scenePos());
}

BranchItem::BranchItem(SchemeItem *const startItem, SchemeItem *const endItem)
    : SchemeItem("Branch"), m_start(), m_end(),
      m_line(0,0,0,0), m_resistor(), m_path(),
      m_startItem(startItem), m_endItem(endItem)
{
    setTransparent(false);
    setPos(m_startItem->scenePos());
    moveEndPoint(m_endItem->scenePos());
    setEndItem(m_endItem);
}

void BranchItem::setEndItem(SchemeItem *const endItem)
{
    if (m_startItem == endItem)
        return;
    m_endItem = endItem;
    moveEndPoint(m_endItem->scenePos());
}

void BranchItem::movePoint(const QPointF &start, const QPointF &end)
{
    if (start != m_start)
        m_start = start - pos();
    if (end != m_end)
        m_end = end - pos();

    m_line.setP1(m_start);
    m_line.setP2(m_end);
    QPointF centerP(m_line.center());
    const quint16 rweight = resistor_width/2;
    const quint16 rheight = resistor_height/2;

    qreal halfLen = sqrt(pow(centerP.x() - m_start.x(), 2) +
                         pow(centerP.y() - m_start.y(), 2));

    if (halfLen > 90)
    {
    QPointF delta((rweight/halfLen)*(centerP.x()-m_start.x()),
                  (rweight/halfLen)*(centerP.y()-m_start.y()));

    qreal degBranch = atan((m_end.y()-m_start.y())/(m_end.x()-m_start.x()));
    if ((m_end.x()-m_start.x())<0)
        degBranch += qDegreesToRadians(180.);

    QPointF beginResistorP(centerP - delta);
    QPointF resistorP1(beginResistorP.x()+cos(degBranch-(qDegreesToRadians(90.)))*rheight,
                       beginResistorP.y()+sin(degBranch-(qDegreesToRadians(90.)))*rheight);
    QPointF resistorP2(resistorP1.x() + cos(degBranch)*resistor_width,
                       resistorP1.y() + sin(degBranch)*resistor_width);
    QPointF endResistorP(resistorP2.x()+cos(degBranch-(qDegreesToRadians(-90.)))*rheight,
                         resistorP2.y()+sin(degBranch-(qDegreesToRadians(-90.)))*rheight);
    QPointF resistorP3(endResistorP.x()+cos(degBranch-(qDegreesToRadians(-90.)))*rheight,
                       endResistorP.y()+sin(degBranch-(qDegreesToRadians(-90.)))*rheight);
    QPointF resistorP4(resistorP3.x() + cos(degBranch+qDegreesToRadians(180.))*resistor_width,
                       resistorP3.y() + sin(degBranch+qDegreesToRadians(180.))*resistor_width);

    m_path.clear();
    m_path.moveTo(m_start);
    m_path.lineTo(beginResistorP);
    m_path.lineTo(resistorP1);
    m_path.lineTo(resistorP2);
    m_path.lineTo(endResistorP);
    m_path.lineTo(m_end);
    m_path.moveTo(endResistorP);
    m_path.lineTo(resistorP3);
    m_path.lineTo(resistorP4);
    m_path.lineTo(beginResistorP);
    }
    else
    {
        m_path.clear();
        m_path.moveTo(m_start);
        m_path.lineTo(m_end);
    }

    setPath(m_path);
}

void BranchItem::moveStartPoint(const QPointF &start)
{
    movePoint(start, m_end);
}
void BranchItem::moveEndPoint(const QPointF &end)
{
    movePoint(m_start, end);
}

void BranchItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    SchemeItem::paint(painter, option);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap));
    painter->setBrush(Qt::white);

    painter->drawPath(path());

    painter->setPen(QPen(Qt::black, 2));

    painter->drawText(boundingRect(), Qt::AlignCenter, "Уhdr");

}
int BranchItem::type() const
{
    return TypeBranchItem;
}

const SchemeItem *BranchItem::getStartItem() const
{
    return m_startItem;
}

const SchemeItem *BranchItem::getEndItem() const
{
    return m_endItem;
}



//--------------------------------------------------------------------------

LoadItem::LoadItem()
    : SchemeItem("non")
{
    QPainterPath path;
    path.moveTo(0,0);
    path.lineTo(0, 50);
    path.addRect(-15,50,30,80);
    path.moveTo(0,130);
    path.lineTo(0,200);
    path.lineTo(10,190);
    path.lineTo(-10,190);
    path.lineTo(0,200);
    setPath(path);
}

void LoadItem::setItem(SchemeItem * const item)
{
    m_item = item;
    setPos(m_item->scenePos());
}

const SchemeItem *LoadItem::getItem()
{
    return m_item;
}

int LoadItem::type() const
{
    return SchemeItem::TypeLoadItem;
}

void LoadItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(widget)
    SchemeItem::paint(painter, option);

    painter->setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap));
    painter->setBrush(Qt::white);
    painter->drawPath(path());
}

//--------------------------------------------------------------------------
