#include <QMouseEvent>

#include "viewscheme.h"
#include "schemescene.h"

ViewScheme::ViewScheme(QWidget *parent)
    : QGraphicsView(parent)
{
    setMouseTracking(true);
    centerOn(sceneRect().width() / 2, sceneRect().height() / 2);
    showGrid(true);
    setCacheMode(CacheBackground);
    setTransformationAnchor(AnchorUnderMouse);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ViewScheme::wheelEvent(QWheelEvent *event)
{
    qreal scaleFactor = pow(2., -event->angleDelta().y() / 240.0);
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor > 0.07 && factor < 100)
        scale(scaleFactor, scaleFactor);
}

void ViewScheme::setChoiseViewMode()
{
    setDragMode(QGraphicsView::NoDrag);
    static_cast<SchemeScene*>(scene())->setMode(SchemeScene::Select);
}

void ViewScheme::setGrabViewMode()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    static_cast<SchemeScene*>(scene())->setMode(SchemeScene::None);
}

void ViewScheme::showGrid(bool visible)
{
    visible ? setBackgroundBrush(QPixmap(":/images/grid.png"))
            : setBackgroundBrush(Qt::NoBrush);
}
