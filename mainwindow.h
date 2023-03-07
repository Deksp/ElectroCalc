#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "viewschem.h"
#include "schemescene.h"
#include "layoutscheme.h"
#include "calculation.h"

QT_BEGIN_NAMESPACE
class QAction;
class QButtonGroup;
class QToolButton;
class QHBoxLayout;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createToolBar();
    void createToolGroup();
    void createMenuBar();
    void createActions();

signals:

private slots:
    void buttonGroupClicked(int id);

protected:

private:
///-----interfase elements-----///
    QHBoxLayout *mainLayout;

    QTabWidget *tabview;
    QVector<ViewSchem *> view;
    QVector<SchemeScene *> scene;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *pointerToolBar;
    QToolBar *runToolBar;

    QButtonGroup *buttonGroup;
    QToolButton *addVertexButton;
    QToolButton *addGeneratorButton;
    QToolButton *addBranchButton;
    QToolButton *addLoadButton;
///-----interfase elements-----///

    Calculation calc;
    LayoutScheme *layoutScheme;
};
#endif // MAINWINDOW_H
