#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "schemescene.h"

SchemeScene::SchemeScene()
    : m_layoutScheme(new LayoutScheme), m_gridBinging(true),
      m_mode(None), m_insertedItem(nullptr)
{
    setSceneRect(0,0, m_sceneSize, m_sceneSize);
    setItemIndexMethod(QGraphicsScene::ItemIndexMethod::BspTreeIndex);
    setMode(m_mode);
}

void SchemeScene::setMode(Mode mode)
{
    if ((m_mode == InsertVertex ||
        m_mode == InsertGenerator ||
        m_mode == InsertBranch ||
        m_mode == InsertLoad)
        && mode == Select) emit unChekButton();

    m_mode = mode;
    if (m_insertedItem != nullptr)
    {
        removeItem(m_insertedItem);
        delete m_insertedItem;
        m_insertedItem = nullptr;
    }

    switch (m_mode)
    {
    case None:
        for (QGraphicsItem* item : items())
            if (item->type() > QGraphicsItem::UserType)
            {
                item->setSelected(false);
                item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                item->setFlag(QGraphicsItem::ItemIsMovable, false);
            }
        emit unChekButton();
        break;
    case Select:
        for (QGraphicsItem* item : items())
            if (item->type() > QGraphicsItem::UserType)
            {
                item->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
            }
        break;
    case Grab:
        break;
    case InsertVertex:
        m_insertedItem = new VertexItem(m_layoutScheme->getNextVIndex());
        break;
    case InsertGenerator:
        m_insertedItem = new GeneratorItem(m_layoutScheme->getNextGIndex());
        break;
    case InsertBranch:
        break;
    case InsertLoad:
        break;
    default:
        break;
    }

    switch (mode)
    {
    case InsertVertex:
    case InsertGenerator:
    case InsertLoad:
        for (QGraphicsItem* item : items())
        {
            item->setFlag(QGraphicsItem::ItemIsSelectable, false);
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
        }
        m_insertedItem->setPos(-500, -500);
        addItem(m_insertedItem);
        break;
    case InsertBranch:

        break;
    default:
        break;
    }
}

void SchemeScene::setGridBinding(bool gridBinding)
{
    m_gridBinging = gridBinding;
}

QPointF SchemeScene::gridPoint(QPointF point)
{
    if (m_gridBinging)
    {
        qreal pos_x = m_gridStep * (int(point.x()) / m_gridStep) + m_gridStep / 2;
        qreal pos_y = m_gridStep * (int(point.y()) / m_gridStep) + m_gridStep / 2;
        return QPoint(pos_x, pos_y);
    }
    return point;
}

void SchemeScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
bool bloc = false;
    switch (event->button())
    {
    case Qt::LeftButton:
        switch (m_mode)
        {
        case InsertVertex:
        case InsertGenerator:
        case InsertLoad:
            if (!m_insertedItem->isBlock())
            {
                m_insertedItem->setTransparent(false);
                m_insertedItem = nullptr;
                setMode(m_mode);
            }
            break;

        case InsertBranch:
            if (itemAt(event->scenePos(),QTransform()) != nullptr && !bloc)
            {
                m_insertedItem = new BranchItem(static_cast<SchemeItem*>(itemAt(event->scenePos(),QTransform())));
                static_cast<BranchItem*>(m_insertedItem)->setP2(event->scenePos()+QPointF(1,1));
                bloc = true;
            }
            if (bloc)
                static_cast<BranchItem*>(m_insertedItem)->setBlock(true);
            break;

        default:
            break;
        }
        break;

    case Qt::RightButton:
        setMode(Select);
        break;

    default:
        break;
    }
}

void SchemeScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    bool block = false;
    switch (m_mode)
    {
    case Select:
        if (!selectedItems().isEmpty())
        for (QGraphicsItem *selitem : selectedItems())
        {
            for (QGraphicsItem *item : items())
            {
                if (item->type() != QGraphicsItem::UserType && selitem != item)
                {
                    QPointF distanceVector = selitem->pos() - item->pos();
                    qreal distance = sqrt(pow(distanceVector.x(), 2) + pow(distanceVector.y(), 2));
                    if (distance < m_distance)
                    {
                        static_cast<SchemeItem*>(selitem)->setBlock(true);
                        break;
                    }
                    else
                        static_cast<SchemeItem*>(selitem)->setBlock(false);
                }
            }
        }
        break;
    case InsertVertex:
    case InsertGenerator:
        m_insertedItem->setPos(gridPoint(event->scenePos()));
        for (QGraphicsItem *item : items())
            if (item->type() != QGraphicsItem::UserType && m_insertedItem != item)
            {
                QPointF distanceVector = event->scenePos() - item->pos();
                qreal distance = sqrt(pow(distanceVector.x(), 2) + pow(distanceVector.y(), 2));
                if (distance < m_distance)
                {
                    block = true;
                    break;
                }
            }
        m_insertedItem->setBlock(block);
        break;

    case InsertBranch:
        if (m_insertedItem != nullptr && !static_cast<BranchItem*>(m_insertedItem)->isBlock())
        static_cast<BranchItem*>(m_insertedItem)->setP2(event->scenePos());
        break;
    case InsertLoad:

        break;

    default:
        break;
    }
}

void SchemeScene::clearScene()
{

}

void SchemeScene::removeSelectedItems()
{

}

void SchemeScene::turn()
{

}
