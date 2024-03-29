#ifndef SCHEMSCENE_H
#define SCHEMSCENE_H

#include <QGraphicsScene>

#include "layoutscheme.h"
#include "schemeitem.h"


class SchemeScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Mode
    {
        None,
        Select,
        Grab,
        InsertVertex,
        InsertGenerator,
        InsertBranch,
        InsertLoad
    };

    SchemeScene();
    ~SchemeScene();
    LayoutScheme *getLayoutSchem() const;
    const QString getSchemeName() const;
    void setSchemeName(const QString &name);
    SchemeItem *getItemfromNode(Node *node);

    friend QDataStream& operator<<(QDataStream &out, SchemeScene &scene);
    friend QDataStream& operator>>(QDataStream &in, SchemeScene &scene);

public slots:
    void turn();
    void clearScene();
    void removeSelectedItems();
    void setMode(Mode mode);
    void setSetingDefaultVal(bool state);
    void setGridBinding(bool gridBinding);
    QPointF gridPoint(QPointF point);

signals:
    void unChekButton();
    void input(LayoutScheme *layout, Node *node, const QPoint &pos);

private:
    LayoutScheme *m_layoutScheme;

    static const quint8 m_distance = 150;
    static const quint8 m_gridStep = 26;
    static const quint16 m_sceneSize = 8060;

    bool m_gridBinging;
    bool m_setingDefaultVal;
    Mode m_mode;
    SchemeItem *m_insertedItem;
    QString schemeName;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // SCHEMSCENE_H
