#include "mainwindow.h"

#include <QLabel>
#include <QLayout>
#include <QAction>
#include <QToolBar>
#include <QButtonGroup>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QKeyEvent>

extern QString tabStyle;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mainLayout(new QHBoxLayout)
{
    createToolGroup();

    scene << new SchemeScene;
    view << new ViewScheme;
    view.first()->setScene(scene.first());
    connect(scene.first(), &SchemeScene::unChekButton, this, &MainWindow::unCheckButtonGroup);
    connect(scene.first(), &SchemeScene::input, this, &MainWindow::inputWidgetShow);

    createActions();
    createMenuBar();
    createToolBar();

    tabview = new QTabWidget;
    tabview->addTab(view.first(), "Scheme1");
    tabview->setTabPosition(tabPosition(Qt::DockWidgetArea::BottomDockWidgetArea));
    tabview->setStyleSheet(tabStyle);
    mainLayout->addWidget(tabview);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    setWindowTitle(tr("ElectroCalc"));
    setGeometry(300,200,1600,1000);

    fileToolBar->setEnabled(false);
    editToolBar->setEnabled(false);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createToolBar()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newFile);
    fileToolBar->addAction(openFile);
    fileToolBar->addAction(saveFile);

    editToolBar = addToolBar(tr("Edit"));
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

    openFile = new QAction(QIcon(":/images/openFile.png"), tr("Open File"), this);
    openFile->setStatusTip(tr("Open file"));

    saveFile = new QAction(QIcon(":/images/saveFile.png"), tr("Save File"), this);
    saveFile->setStatusTip(tr("Save file"));
    saveFile->setShortcut(tr("Ctrl+S"));

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
    connect(choiseModeAction, &QAction::triggered, getCurrentView(), &ViewScheme::setChoiseViewMode);

    grabModeAction = new QAction(QIcon(":/images/grab.png"), tr("Grab"), this);
    grabModeAction->setStatusTip(tr("Grab mode"));
    grabModeAction->setCheckable(true);
    grabModeAction->setShortcut(tr("Shift+G"));
    connect(grabModeAction, &QAction::triggered, getCurrentView(), &ViewScheme::setGrabViewMode);

    gridvisible = new QAction(QIcon(":/images/gridVisible.png"), tr("GridVisible"), this);
    gridvisible->setStatusTip(tr("Grid visible"));
    gridvisible->setCheckable(true);
    gridvisible->setChecked(true);
    gridvisible->setShortcut(tr("Shift+F5"));
    connect(gridvisible, &QAction::toggled, getCurrentView(), &ViewScheme::showGrid);

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

SchemeScene *MainWindow::getCurrentScene()
{
    return scene.first();
}

ViewScheme *MainWindow::getCurrentView()
{
    return view.first();
}

void MainWindow::unCheckButtonGroup()
{
    for (QAbstractButton *button : buttonGroup->buttons())
        button->setChecked(false);
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

void MainWindow::inputWidgetShow(LayoutScheme *layout, Node *node)
{
    InputWidget *input = new InputWidget(layout, node);
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

InputWidget::InputWidget(LayoutScheme *layout, Node *node)
    : QLineEdit(), m_layout(layout), m_node(node)
{

}

void InputWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        switch (m_node->type())
        {
        case Node::TypeGeneratorNode:
            m_layout->setVoltage(m_node, text().toDouble());
            break;
        case Node::TypeBranchNode:
            //m_layout->setBranch(m_node, text().)
            break;
        case Node::TypeLoadNode:

            break;
        }
        this->~InputWidget();
    }
    else
        QLineEdit::keyPressEvent(event);
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






