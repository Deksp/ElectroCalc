#ifndef VIEWSCHEM_H
#define VIEWSCHEM_H

#include <QGraphicsView>

class ViewScheme : public QGraphicsView
{
    Q_OBJECT
public:
    ViewScheme(QWidget *parent = nullptr);

private:
    QPixmap texture;

protected:
    void wheelEvent(QWheelEvent *event) override;

public slots:
    void setChoiseViewMode();
    void setGrabViewMode();
    void showGrid(bool visible);
};

#endif // VIEWSCHEM_H
