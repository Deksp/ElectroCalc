#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "schemescene.h"

SchemeScene::SchemeScene()
    : m_layoutScheme(new LayoutScheme), m_gridBinging(false),
      m_mode(None), m_insertedItem(nullptr)
{
    setSceneRect(0,0, m_sceneSize, m_sceneSize);
    setItemIndexMethod(QGraphicsScene::ItemIndexMethod::BspTreeIndex);
    setMode(m_mode);
}

SchemeScene::~SchemeScene()
{
    delete m_insertedItem;
    delete m_layoutScheme;
}

LayoutScheme *SchemeScene::getLayoutSchem() const
{
    return m_layoutScheme;
}

void SchemeScene::setMode(Mode mode)
{
    if ((m_mode == InsertVertex ||
        m_mode == InsertGenerator ||
        m_mode == InsertBranch ||
        m_mode == InsertLoad)
        && mode == Select) emit unChekButton();


    if (m_insertedItem != nullptr)
    {
        removeItem(m_insertedItem);
        delete m_insertedItem;
        m_insertedItem = nullptr;
    }

    switch (mode)
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
                item->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
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
        for (QGraphicsItem* item : items())
            if (item->type() > QGraphicsItem::UserType)
            {
                item->setSelected(false);
                item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                item->setFlag(QGraphicsItem::ItemIsMovable, false);
            }
        break;
    case InsertLoad:
        m_insertedItem = new LoadItem(m_layoutScheme->getDefResistance());
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

    m_mode = mode;
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
    static bool stage = false;
    switch (event->button())
    {
    case Qt::LeftButton:
        switch (m_mode)
        {
        case None:
            break;
        case Select:
            break;
        case Grab:
            break;
        case InsertVertex:
            if (!m_insertedItem->isBlock())
            {
                m_insertedItem->setTransparent(false);
                m_insertedItem->setNode(m_layoutScheme->addVertex());
                m_insertedItem = nullptr;
                setMode(m_mode);
            }
            break;
        case InsertGenerator:
            if (!m_insertedItem->isBlock())
            {
                m_insertedItem->setTransparent(false);
                m_insertedItem->setNode(m_layoutScheme->addGenerator());
                m_insertedItem = nullptr;
                setMode(m_mode);
            }
        case InsertLoad:
            if (m_insertedItem->isAllowed())
            {
                m_insertedItem->setStage(true);
                SchemeItem *item = static_cast<SchemeItem*>(
                            itemAt(event->scenePos(), QTransform())->parentItem());
                static_cast<VertexItem*>(item)->addLoad(static_cast<LoadItem*>(m_insertedItem));
                m_insertedItem->setNode(m_layoutScheme->addLoad(item->getNode()));
                static_cast<LoadItem*>(m_insertedItem)->setItem(item);
            }
            break;
        case InsertBranch:
        {
            SchemeItem *item = static_cast<SchemeItem*>(itemAt(event->scenePos(), QTransform()));
            if (item != nullptr &&
                (item->parentItem()->type() == SchemeItem::TypeVertexItem ||
                item->parentItem()->type() == SchemeItem::TypeGeneratorItem))
            {
                if (stage && !m_insertedItem->isBlock())
                {
                    static_cast<BranchItem*>(m_insertedItem)->setStage(true);
                    static_cast<BranchItem*>(m_insertedItem)->
                            setEndItem(static_cast<SchemeItem*>(item->parentItem()));
                    static_cast<VertexItem*>(item->parentItem())->
                            addBranch(static_cast<BranchItem*>(m_insertedItem));
                    m_insertedItem->setNode(m_layoutScheme->addBranch(
                                                static_cast<BranchItem*>(m_insertedItem)->getStartItem()->getNode(),
                                                static_cast<BranchItem*>(m_insertedItem)->getEndItem()->getNode()));
                    m_insertedItem->setAllowed(false);
                    m_insertedItem = nullptr;
                    stage = false;
                    setMode(m_mode);
                }
                else if (!stage)
                {
                    m_insertedItem = new BranchItem(static_cast<SchemeItem*>(item->parentItem()),
                                                    m_layoutScheme->getDefResistance());
                    static_cast<VertexItem*>(item->parentItem())->
                            addBranch(static_cast<BranchItem*>(m_insertedItem));
                    addItem(m_insertedItem);
                    stage = true;
                }
            }
        }
            break;
        default:
            break;
        }
        break;

    case Qt::RightButton:
        stage = false;
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
                if (item->type() > SchemeItem::TypeLoadItem && selitem != item &&
                        selitem->type() > SchemeItem::TypeLoadItem)
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
            if (item->type() > SchemeItem::TypeLoadItem && m_insertedItem != item)
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
    {
        if (m_insertedItem != nullptr)
        {
            if (itemAt(event->scenePos(), QTransform()) != nullptr)
            {
                if (itemAt(event->scenePos(), QTransform())->parentItem() != nullptr)
                {
                SchemeItem *item = static_cast<SchemeItem*>(itemAt(event->scenePos(), QTransform())->parentItem());

                if (item->isBlock())
                    m_insertedItem->setBlock(true);
                else if (item->type() > SchemeItem::TypeLoadItem)
                {
                    if (!item->isStage())
                        m_insertedItem->setAllowed(true);
                    else if (item->isStage())
                        m_insertedItem->setBlock(true);
                }
                }
            }
            if (itemAt(event->scenePos(), QTransform()) == nullptr)
            {
                m_insertedItem->setBlock(false);
                m_insertedItem->setAllowed(false);
                m_insertedItem->update();
                //qDebug()<<m_insertedItem->isAllowed();
            }
            if (!static_cast<BranchItem*>(m_insertedItem)->isStage())
            {
                static_cast<BranchItem*>(m_insertedItem)->moveEndPoint(gridPoint(event->scenePos()));
            }
        }
        break;
    }
    case InsertLoad:
    {
        if (m_insertedItem->isStage())
        {
            static_cast<LoadItem*>(m_insertedItem)->moveLoad(gridPoint(event->scenePos()));
            break;
        }
        bool allowed = false;
        m_insertedItem->setPos(gridPoint(event->scenePos()));
        for (auto item : m_insertedItem->collidingItems())
        {
            if (item->type() == SchemeItem::TypeVertexItem)
                allowed = true;
            if (item->type() == SchemeItem::TypeGeneratorItem)
                block = true;
            m_insertedItem->setAllowed(allowed);
            m_insertedItem->setBlock(block);
        }
        break;
    }
    default:
        break;
    }
}

void SchemeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (m_mode == InsertLoad)
    {
        if (m_insertedItem->isAllowed())
        {
            m_insertedItem->setTransparent(false);
            m_insertedItem->setAllowed(false);
            if (sqrt(pow(m_insertedItem->scenePos().x()-
                         static_cast<LoadItem*>(m_insertedItem)->getItem()->scenePos().x(), 2) +
                     pow(m_insertedItem->scenePos().y()-
                         static_cast<LoadItem*>(m_insertedItem)->getItem()->scenePos().y(), 2))
                    < 26)
            {
                m_insertedItem->setPos(static_cast<LoadItem*>(m_insertedItem)->
                                   getItem()->scenePos());
                m_insertedItem->setStage(false);
            }
            m_insertedItem = nullptr;
            setMode(m_mode);
        }
    }
}

void SchemeScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (itemAt(event->scenePos(), QTransform()) != nullptr)
        if (itemAt(event->scenePos(), QTransform())->parentItem() != nullptr)
            if (itemAt(event->scenePos(), QTransform())->parentItem()->type() == SchemeItem::TypeGeneratorItem)
                emit input(m_layoutScheme,
                           static_cast<SchemeItem*>(itemAt(event->scenePos(), QTransform())->parentItem())->getNode());
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
