#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QDialog>
#include <QLineEdit>
#include <QSettings>
#include <QPlainTextEdit>      // добавь для PlainTextEdit
#include <QSplitter>           // добавь для QSplitter

#include "viewscheme.h"
#include "schemescene.h"
#include "layoutscheme.h"
#include "calculation.h"

QT_BEGIN_NAMESPACE
class QAction;
class QButtonGroup;
class QToolButton;
class QHBoxLayout;
class QGridLayout;
class QSpinBox;
class QSplitter;
QT_END_NAMESPACE

class AddButton;
class InputWidget;

// ---------- новый класс для бокового редактора ----------
class SidePlainTextWidget : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit SidePlainTextWidget(QWidget *parent = nullptr);
    void setDefaultText(const QString &text = QString());
};
// --------------------------------------------------------

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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void Calc();
    void stepCalc();
    void inputWidgetShow(LayoutScheme *layout, Node *node, const QPoint &pos);
    void buttonGroupClicked(QAbstractButton *sender, bool cheked);
    void unCheckButtonGroup();
    void runNewFile();
    void runOpenFile();
    void runOpenExampleFile();
    void runSaveFile();

private:
    ///-----интерфейс-----///
    QHBoxLayout *mainLayout;
    QSplitter *centralSplitter;            // новый splitter
    QTabWidget *tabview;
    ViewScheme *view;
    QVector<SchemeScene *> scene;

    // новый редактор
    SidePlainTextWidget *sideTextWidget;   // наш редактор справа

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    QAction *newFile;
    QAction *openExample;
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
    QSpinBox *stepsForRun;
    QAction *stepRunAction;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *pointerToolBar;
    QToolBar *runToolBar;

    QButtonGroup *buttonGroup;
    AddButton *addVertexButton;
    AddButton *addGeneratorButton;
    AddButton *addBranchButton;
    AddButton *addLoadButton;

    Calculation calc;
    LayoutScheme *layoutScheme;
    QString lastSaveDirectory;
    QString lastOpenDirectory;
    QString argvFilePath;
    QSettings programSetings;
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
    void polyarVisible();

private:
    LayoutScheme *m_layout;
    Node *m_node;
    QLineEdit lineEdit;
    QLineEdit lineEditTwo;
    QLineEdit polyarLineEdit;
    QGridLayout *layoutWidget;
    bool polyarState;
    bool m_reversValue;
    static bool defValsStatus;
};

#endif // MAINWINDOW_H
