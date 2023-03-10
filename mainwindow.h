#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>

#include "viewscheme.h"
#include "schemescene.h"
#include "layoutscheme.h"
#include "calculation.h"

QT_BEGIN_NAMESPACE
class QAction;
class QButtonGroup;
class QToolButton;
class QHBoxLayout;
QT_END_NAMESPACE
class AddButton;

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
    SchemeScene *getCurrentScene();
    ViewScheme *getCurrentView();

signals:

private slots:
    void buttonGroupClicked(QAbstractButton *sender, bool cheked);
    void unCheckButtonGroup();

private:
///-----interfase elements-----///
    QHBoxLayout *mainLayout;

    QTabWidget *tabview;
    QVector<ViewScheme *> view;
    QVector<SchemeScene *> scene;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    QAction *newFile;
    QAction *openFile;
    QAction *saveFile;

    QAction *cut;
    QAction *copy;
    QAction *paste;
    QAction *undo;
    QAction *redo;

    QAction *choiseModeAction;
    QAction *grabModeAction;

    QAction *gridvisible;
    QAction *gridbinding;

    QAction *runAction;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *pointerToolBar;
    QToolBar *runToolBar;

    QButtonGroup *buttonGroup;
    AddButton *addVertexButton;
    AddButton *addGeneratorButton;
    AddButton *addBranchButton;
    AddButton *addLoadButton;
///-----interfase elements-----///

    Calculation calc;
    LayoutScheme *layoutScheme;
};

///-----class for setting up buttons----///
class AddButton : public QToolButton
{
    Q_OBJECT
public:
    AddButton(QString text, QIcon icon, QButtonGroup *group, int id = -1,
              QWidget *parent = nullptr);
    QWidget *createCellWidget();
};
///-----class for setting up buttons----///

#endif // MAINWINDOW_H
