#include "mainwindow.h"

#include <QFile>
#include <QLabel>
#include <QtMath>
#include <QLayout>
#include <QAction>
#include <QToolBar>
#include <QButtonGroup>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QDockWidget>
#include <QDataStream>
#include <QTreeWidget>
#include <QKeyEvent>



extern QString tabStyle;
bool InputWidget::defValsStatus = false;
const QString MainWindow::SetingsFileName = "setingsFile";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mainLayout(new QHBoxLayout),
      layoutScheme(nullptr)
{
    setingsList();
    createToolGroup();

    scene << new SchemeScene;
    view = new ViewScheme;
    view->setScene(scene.first());
    connect(scene.first(), &SchemeScene::unChekButton, this, &MainWindow::unCheckButtonGroup);
    connect(scene.first(), &SchemeScene::input, this, &MainWindow::inputWidgetShow);

    createActions();
    createMenuBar();
    createToolBar();

    tabview = new QTabWidget;
    tabview->addTab(view, "Scheme1");
    tabview->setTabPosition(tabPosition(Qt::DockWidgetArea::BottomDockWidgetArea));
    tabview->setStyleSheet(tabStyle);
    mainLayout->addWidget(tabview);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    setWindowTitle(tr("ElectroCalc"));
    setGeometry(100,100,800,500);

    //fileToolBar->setEnabled(false);
    //editToolBar->setEnabled(false);
    cut->setEnabled(false);
    copy->setEnabled(false);
    paste->setEnabled(false);
    undo->setEnabled(false);
    redo->setEnabled(false);
}

MainWindow::~MainWindow()
{
    QFile file(SetingsFileName);
    QDataStream stream(&file);

    file.open(QIODevice::WriteOnly);
    stream << lastDirectory;
    file.close();
}

void MainWindow::createToolBar()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newFile);
    fileToolBar->addAction(openFile);
    fileToolBar->addAction(saveFile);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(clean);
    editToolBar->addAction(_delete);
    editToolBar->addAction(cut);
    editToolBar->addAction(copy);
    editToolBar->addAction(undo);
    editToolBar->addAction(redo);

    pointerToolBar = addToolBar(tr("Pointer"));
    pointerToolBar->addAction(choiseModeAction);
    pointerToolBar->addAction(grabModeAction);
    pointerToolBar->addAction(gridvisible);
    pointerToolBar->addAction(gridbinding);
    QActionGroup *viewMods = new QActionGroup(pointerToolBar);
    viewMods->addAction(choiseModeAction);
    viewMods->addAction(grabModeAction);

    runToolBar = addToolBar(tr("Run"));
    runToolBar->addAction(runAction);
}

void MainWindow::createToolGroup()
{
    buttonGroup = new QButtonGroup;
    buttonGroup->setExclusive(false);

    addVertexButton = new AddButton(tr("&Vertex"),
                                    QIcon(":/images/Vertex.png"), buttonGroup, 3);
    addGeneratorButton = new AddButton(tr("&Generator"),
                                       QIcon(":/images/generator.png"), buttonGroup, 4);
    addBranchButton = new AddButton(tr("&Branch"),
                                    QIcon(":/images/branch.png"), buttonGroup, 5);
    addLoadButton = new AddButton(tr("&Load"),
                                  QIcon(":/images/load.png"), buttonGroup, 6);
    connect(buttonGroup,
            QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
            this, &MainWindow::buttonGroupClicked);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(addVertexButton->createCellWidget());
    layout->addWidget(addGeneratorButton->createCellWidget());
    layout->addWidget(addBranchButton->createCellWidget());
    layout->addWidget(addLoadButton->createCellWidget());
    layout->setAlignment(Qt::AlignTop);
    mainLayout->addLayout(layout);
}

void MainWindow::createMenuBar()
{

}

void MainWindow::createActions()
{
    newFile = new QAction(QIcon(":/images/newFile.png"), tr("New File"), this);
    newFile->setStatusTip(tr("Create new file"));
    connect(newFile, &QAction::triggered, this, &MainWindow::runNewFile);

    openFile = new QAction(QIcon(":/images/openFile.png"), tr("Open File"), this);
    openFile->setStatusTip(tr("Open file"));
    connect(openFile, &QAction::triggered, this, &MainWindow::runOpenFile);

    saveFile = new QAction(QIcon(":/images/saveFile.png"), tr("Save File"), this);
    saveFile->setStatusTip(tr("Save file"));
    saveFile->setShortcut(tr("Ctrl+S"));
    connect(saveFile, &QAction::triggered, this, &MainWindow::runSaveFile);

    clean = new QAction(QIcon(":/images/clean.png"), tr("Clean"), this);
    clean->setStatusTip(tr("Clean"));
    connect(clean, &QAction::triggered, getCurrentScene(), &SchemeScene::clearScene);

    _delete = new QAction(QIcon(":/images/delete.png"), tr("Delete"), this);
    _delete->setStatusTip(tr("Delete"));
    _delete->setShortcut(tr("DEL"));

    cut = new QAction(QIcon(":/images/cut.png"), tr("Cut"), this);
    cut->setStatusTip(tr("Cut"));
    cut->setShortcut(tr("Ctrl+X"));

    copy = new QAction(QIcon(":/images/copy.png"), tr("Copy"), this);
    copy->setStatusTip(tr("Copy"));
    copy->setShortcut(tr("Ctrl+C"));

    paste = new QAction(QIcon(":/images/paste.png"), tr("Paste"), this);
    paste->setStatusTip(tr("Paste"));
    paste->setShortcut(tr("Ctrl+V"));

    undo = new QAction(QIcon(":/images/undo.png"), tr("Undo"), this);
    undo->setStatusTip(tr("Undo"));
    undo->setShortcut(tr("Ctrl+Z"));

    redo = new QAction(QIcon(":/images/redo.png"), tr("Redo"), this);
    redo->setStatusTip(tr("Redo"));
    redo->setShortcut(tr("Ctrl+Y"));

    choiseModeAction = new QAction(QIcon(":/images/pointer.png"), tr("Choise"), this);
    choiseModeAction->setStatusTip(tr("Usual mode"));
    choiseModeAction->setCheckable(true);
    choiseModeAction->setChecked(true);
    choiseModeAction->setShortcut(tr("Shift+S"));
    connect(choiseModeAction, &QAction::triggered, view, &ViewScheme::setChoiseViewMode);

    grabModeAction = new QAction(QIcon(":/images/grab.png"), tr("Grab"), this);
    grabModeAction->setStatusTip(tr("Grab mode"));
    grabModeAction->setCheckable(true);
    grabModeAction->setShortcut(tr("Shift+G"));
    connect(grabModeAction, &QAction::triggered, view, &ViewScheme::setGrabViewMode);

    gridvisible = new QAction(QIcon(":/images/gridVisible.png"), tr("GridVisible"), this);
    gridvisible->setStatusTip(tr("Grid visible"));
    gridvisible->setCheckable(true);
    gridvisible->setChecked(true);
    gridvisible->setShortcut(tr("Shift+F5"));
    connect(gridvisible, &QAction::toggled, view, &ViewScheme::showGrid);

    gridbinding = new QAction(QIcon(":/images/gridBinding.png"), tr("GridBinding"), this);
    gridbinding->setStatusTip(tr("Grid binding"));
    gridbinding->setCheckable(true);
    gridbinding->setChecked(false);
    gridbinding->setShortcut(tr("Shift+F6"));
    connect(gridbinding, &QAction::toggled, getCurrentScene(), &SchemeScene::setGridBinding);

    runAction = new QAction(QIcon(":/images/run"), tr("Run"), this);
    runAction->setStatusTip(tr("Run"));
    runAction->setShortcut(tr("Ctrl+R"));
    connect(runAction, &QAction::triggered, this, &MainWindow::Calc);
}

void MainWindow::setingsList()
{
    QFile file(SetingsFileName);
    QDataStream stream(&file);

    if (!file.exists())
    {
        lastDirectory = "\\";
        file.open(QIODevice::WriteOnly);
        stream << lastDirectory;
        file.close();
        return;
    }
    else
    {
        file.open(QIODevice::ReadOnly);
        stream >> lastDirectory;
        file.close();
        return;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Escape)
        getCurrentScene()->setMode(SchemeScene::Mode::Select);
}

SchemeScene *MainWindow::getCurrentScene()
{
    return scene.last();
}

void MainWindow::unCheckButtonGroup()
{
    for (QAbstractButton *button : buttonGroup->buttons())
        button->setChecked(false);
}

void MainWindow::runNewFile()
{

}

void MainWindow::runOpenFile()
{
    QString fileName =
        QFileDialog::getOpenFileName(this, tr("Open file"), "/", "*.*");
    if (fileName.isNull())
        return;
    QFile file(fileName);
    QDataStream stream(&file);

    file.open(QIODevice::ReadOnly);
    SchemeScene *scene = new SchemeScene;
    stream >> *scene;
    qDebug()<<scene;
    this->scene << scene;
    file.close();
    view->setScene(this->scene.last());
    connect(clean, &QAction::triggered, scene, &SchemeScene::clearScene);
    connect(gridbinding, &QAction::toggled, scene, &SchemeScene::setGridBinding);
}

void MainWindow::runSaveFile()
{
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save file"), lastDirectory+getCurrentScene()->getSchemeName(), "*.scm");
    lastDirectory = fileName.section('/', 0, -2) + '/';

    QFile file(fileName);
    QDataStream stream(&file);

    file.open(QIODevice::WriteOnly);
    stream << *getCurrentScene();
    file.close();
}

void MainWindow::Calc()
{
    calc.startingCalculations(getCurrentScene()->getLayoutSchem());

    QDockWidget *info = new QDockWidget(tr("Information about current scheme"),this);
    info->setMinimumWidth(250);

    QTreeWidget *treeWidget  = new QTreeWidget();
    treeWidget->setColumnCount(1);
    calc.createTree(treeWidget);

    info->setWidget(treeWidget);
    addDockWidget(Qt::RightDockWidgetArea, info);

    calc.endCalculations();
}

void MainWindow::inputWidgetShow(LayoutScheme *layout, Node *node, const QPoint &pos)
{
    InputWidget *input = new InputWidget(layout, node, this);
    input->setGeometry(pos.x(),pos.y(),300,1);
    input->setWindowTitle(tr("Input"));
    input->show();
}

void MainWindow::buttonGroupClicked(QAbstractButton *sender, bool cheked)
{
    if (grabModeAction->isChecked())
    {
        grabModeAction->setChecked(false);
        choiseModeAction->setChecked(true);
        choiseModeAction->triggered(true);
    }

    for (QAbstractButton *button : buttonGroup->buttons())
    {
        if (sender != button && cheked)
            button->setChecked(false);
        else if (cheked)
            getCurrentScene()->setMode(SchemeScene::Mode(buttonGroup->id(sender)));
    }
}

AddButton::AddButton(QString text, QIcon icon,
                                 QButtonGroup *group, int id, QWidget *parent)
    : QToolButton(parent)
{
    setText(text);
    setIcon(icon);
    setMinimumSize(70,70);
    setIconSize(QSize(50,50));
    setCheckable(true);
    group->addButton(this, id);
}

QWidget *AddButton::createCellWidget()
{
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(this, 0, 0, Qt::AlignCenter);
    layout->addWidget(new QLabel(this->text().remove("&")), 1, 0, Qt::AlignCenter);

    QWidget *cellWidget = new QWidget;
    cellWidget->setLayout(layout);
    return cellWidget;
}

InputWidget::InputWidget(LayoutScheme *layout, Node *node, QWidget *parent)
    : QDialog(parent), m_layout(layout), m_node(node), lineEdit(this), polyarState(false), m_reversValue(false)
{
    layoutWidget = new QGridLayout;

    QToolButton *aceptButton = new QToolButton;
    aceptButton->setIcon(QIcon(":/images/accept.png"));
    connect(aceptButton, &QToolButton::clicked, this, &InputWidget::acceptValue);

    QToolButton *inverseCondactions = new QToolButton;
    inverseCondactions->setIcon(QIcon(":/images/inverseCondaction.png"));
    inverseCondactions->setCheckable(true);
    inverseCondactions->setToolTip(tr("Reverse the value"));
    inverseCondactions->setShortcut(tr("Shift+i"));
    connect(inverseCondactions, &QToolButton::toggled, this, &InputWidget::setReversValue);

    QToolButton *defVals = new QToolButton;
    defVals->setIcon(QIcon(":/images/def_values.png"));
    defVals->setCheckable(true);
    defVals->setChecked(defValsStatus);
    defVals->setToolTip(tr("Use default values"));
    connect(defVals, &QToolButton::toggled, this, &InputWidget::setDefValsStatus);
    connect(defVals, &QToolButton::toggled,
            static_cast<MainWindow*>(parent)->getCurrentScene(), &SchemeScene::setSetingDefaultVal);

    layoutWidget->addWidget(&lineEdit,0,2);
    layoutWidget->addWidget(defVals, 0, 0);
    layoutWidget->addWidget(inverseCondactions, 0, 1);
    layoutWidget->setMargin(0);

    if (node->type() == 1)
    {
        QToolButton *polyarVid = new QToolButton;
        polyarVid->setFont(QFont("",12));
        polyarVid->setText("∠");
        connect(polyarVid, &QToolButton::clicked, this, &InputWidget::polyarVisible);
        layoutWidget->addWidget(polyarVid, 0, 3);
        layoutWidget->addWidget(aceptButton, 0, 4);
    }
    else
        layoutWidget->addWidget(aceptButton, 0, 3);

    setLayout(layoutWidget);

    QString lineValue(m_node->getStringTypeNodeProperty());
    QFont font = lineEdit.font();
    font.setPointSize(12);
    lineEdit.setFont(font);
    lineEdit.setText(lineValue.contains('v') ? lineValue.remove(" v") : lineValue.remove(QString(" Ω")));
    lineEdit.setSelection(0, lineValue.size());
}

void InputWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        acceptValue();
    else
        QDialog::keyPressEvent(event);
}

void InputWidget::acceptValue()
{
    complexnum num;
    num = convert(lineEdit.text());
    if (polyarState)
        num = complexnum(num.real()*cos(qDegreesToRadians(polyarLineEdit.text().toDouble())),
                num.real()*sin(qDegreesToRadians(polyarLineEdit.text().toDouble())));
    if (m_reversValue)
        num = complexnum(1)/num;
    switch (m_node->type())
    {
    case Node::TypeGeneratorNode:
        m_layout->setVoltage(m_node, num);
        break;
    case Node::TypeBranchNode:
    case Node::TypeGenBranchNode:
        m_layout->setBranch(m_node, num);
        break;
    case Node::TypeLoadNode:
        m_layout->setLoad(m_node, num);
        break;
    }
    this->~InputWidget();
}

void InputWidget::setReversValue(bool state)
{
    m_reversValue = state;
}

void InputWidget::setDefValsStatus(bool state)
{
    defValsStatus = state;
}

void InputWidget::polyarVisible()
{
    layoutWidget->removeWidget(static_cast<QWidget*>(sender()));
    delete sender();

    QGridLayout *subLayout = new QGridLayout;
    QLabel *polyarLabel = new QLabel("∠:");
    subLayout->addWidget(polyarLabel, 0, 0);
    subLayout->addWidget(&polyarLineEdit, 0, 1);
    subLayout->setMargin(0);
    polyarLineEdit.setMaximumSize(30,30);
    polyarLineEdit.setFont(QFont("", 12));
    layoutWidget->addLayout(subLayout, 0, 3);
    setLayout(layoutWidget);
    polyarState = true;
}

complexnum InputWidget::convert(const QString &text)
{
    if (text.contains('i'))
    {
        int i=0;
        for (i=text.size()-1;i!=-1;i--)
            if (text[i] == '+' || text[i] == '-')
                break;
        QString real, image;
        for (int k=0;k<text.size()-1;k++)
        {
            if(k<i)
                real += text[k];
            if (k>=i)
                image += text[k];
        }
        return complexnum(real.toDouble(), image.toDouble());
    }
    if (!text.contains(',') && !text.contains('/'))
        return complexnum(text.toDouble());
    QStringList textNew;
    if (text.contains(','))
        textNew = text.split(',');
    if (text.contains('/'))
        textNew = text.split('/');
    return complexnum(textNew[0].toDouble(), textNew[1].toDouble());
}

QString tabStyle =
        R"SONG(QTabWidget::pane {
                                       border: 1px solid black;
                                       background: white;
                                   }

                                   QTabWidget::tab-bar:top {
                                       top: 1px;
                                   }

                                   QTabWidget::tab-bar:bottom {
                                       bottom: 1px;
                                   }

                                   QTabWidget::tab-bar:left {
                                       right: 1px;
                                   }

                                   QTabWidget::tab-bar:right {
                                       left: 1px;
                                   }

                                   QTabBar::tab {
                                       border: 1px solid black;
                                   }

                                   QTabBar::tab:selected {
                                       background: white;
                                   }

                                   QTabBar::tab:!selected {
                                       background: silver;
                                   }

                                   QTabBar::tab:!selected:hover {
                                       background: #999;
                                   }

                                   QTabBar::tab:top:!selected {
                                       margin-top: 3px;
                                   }

                                   QTabBar::tab:bottom:!selected {
                                       margin-bottom: 3px;
                                   }

                                   QTabBar::tab:top, QTabBar::tab:bottom {
                                       min-width: 8ex;
                                       margin-right: -1px;
                                       padding: 5px 10px 5px 10px;
                                   }

                                   QTabBar::tab:top:selected {
                                       border-bottom-color: none;
                                   }

                                   QTabBar::tab:bottom:selected {
                                       border-top-color: none;
                                   }

                                   QTabBar::tab:top:last, QTabBar::tab:bottom:last,
                                   QTabBar::tab:top:only-one, QTabBar::tab:bottom:only-one {
                                       margin-right: 0;
                                   }

                                   QTabBar::tab:left:!selected {
                                       margin-right: 3px;
                                   }

                                   QTabBar::tab:right:!selected {
                                       margin-left: 3px;
                                   }

                                   QTabBar::tab:left, QTabBar::tab:right {
                                       min-height: 8ex;
                                       margin-bottom: -1px;
                                       padding: 10px 5px 10px 5px;
                                   }

                                   QTabBar::tab:left:selected {
                                       border-left-color: none;
                                   }

                                   QTabBar::tab:right:selected {
                                       border-right-color: none;
                                   }

                                   QTabBar::tab:left:last, QTabBar::tab:right:last,
                                   QTabBar::tab:left:only-one, QTabBar::tab:right:only-one {
                                       margin-bottom: 0;
        })SONG";






