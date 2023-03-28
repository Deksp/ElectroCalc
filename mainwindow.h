#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QDialog>
#include <QLineEdit>

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
class InputWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    SchemeScene *getCurrentScene();

private:
    void createToolBar();
    void createToolGroup();
    void createMenuBar();
    void createActions();
    void getFileName();
    void setingsList();

signals:

private slots:
    void Calc();
    void inputWidgetShow(LayoutScheme *layout, Node *node, const QPoint &pos);
    void buttonGroupClicked(QAbstractButton *sender, bool cheked);
    void unCheckButtonGroup();
    void runNewFile();
    void runOpenFile();
    void runSaveFile();

private:
///-----interfase elements-----///
    QHBoxLayout *mainLayout;

    QTabWidget *tabview;
    ViewScheme *view;
    QVector<SchemeScene *> scene;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    QAction *newFile;
    QAction *openFile;
    QAction *saveFile;

    QAction *clean;
    QAction *_delete;
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
    QString lastDirectory;
    static const QString SetingsFileName;
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

class InputWidget : public QDialog
{
    Q_OBJECT
public:
    InputWidget(LayoutScheme *layout, Node *node, QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;
    complexnum convert(const QString &text);

public slots:
    void acceptValue();
    void setReversValue(bool state);
    void setDefValsStatus(bool state);

private:
    LayoutScheme *m_layout;
    Node *m_node;
    QLineEdit lineEdit;
    bool m_reversValue;
    static bool defValsStatus;
};

#endif // MAINWINDOW_H
