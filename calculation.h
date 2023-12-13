#ifndef CALCULATION_H
#define CALCULATION_H

#include <QMap>
#include <QPair>
#include <QTreeWidget>
#include "layoutscheme.h"


typedef QVector<QVector<std::complex<double>>> Matrix;

class Calculation
{
    struct MutualPair;
    typedef QMap<MutualPair*, complexnum> MutualVals;
    typedef QVector<MutualVals>  GroupsVal;
    typedef QVector<QMap<QString, complexnum>> FinalCurrent;

    using BranchsCurrentName = QMap<QString, complexnum>;
    using BranchsCurrent = QMap<LayoutScheme::BranchNode*, complexnum>;
    using LoadCurrent = QMap<LayoutScheme::LoadNode*, complexnum>;
    using BranchVolt = QMap<LayoutScheme::BranchNode*, complexnum>;
    using LoadVolt = QMap<LayoutScheme::LoadNode*, complexnum>;
    using VoltageDropName = QMap<QString,complexnum>;
    using VoltageDrop = QMap<Node*,complexnum>;

    BranchsCurrent branchCurrent{};
    LoadCurrent loadCurrent{};
    BranchVolt branchVolt{};
    LoadVolt loadVolt{};
    VoltageDrop voltageDrop{};
    VoltageDropName voltageDropName{};
public:
    enum Mutual_Type{
      gen,
      gengen,
      branchgen,
      loadgen
    };
    Calculation();
    ~Calculation();

    void stepCalculations(LayoutScheme *layout);
    void startingCalculations(LayoutScheme *layout, int value);
    void endCalculations();

    void createTree(QTreeWidget *tree);
int scocIt = 0;
private:
    LayoutScheme *m_layout;

    Matrix m_condactionMatrix;
    Matrix m_invcondactionMatrix;

    GroupsVal m_Z;
    GroupsVal I_first;
    FinalCurrent I_final;
    BranchsCurrentName branchCurrentName{};
    bool iterCondition = false;
    bool firstCalc = true;


    void inverse();
    void out();
    void Zcalc();
    void Icalc();
    void VoltCalc();
    Node* returnNodeFromTextCurrent(QString);
    void Iter();
    QString conver(const complexnum &num);
    QString Zname(MutualPair *pair, Node *currentNode);
    QString Iname(MutualPair *pair, Node *currentNode);
    QString Ifinalname(MutualPair *pair, Node *currentNode);
    complexnum Z(const MutualPair &pair, Node *currentNode);
    QString trueCurrentName(QString currentFinalName);

    struct MutualPair
    {
        Node *m_node, *m_mutualNode;
        Mutual_Type m_mutualType;
    };
};

#endif // CALCULATION_H
