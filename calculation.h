#ifndef CALCULATION_H
#define CALCULATION_H

#include <QMap>
#include <QTreeWidget>

#include "layoutscheme.h"

typedef QVector<QVector<std::complex<double>>> Matrix;

class Calculation
{
    struct MutualPair;
    typedef QMap<MutualPair*, complexnum> MutualVals;
    typedef QVector<MutualVals>  GroupsVal;
    typedef QVector<QMap<QString, complexnum>> FinalCurrent;

public:
    enum Mutual_Type{
      gen,
      gengen,
      branchgen,
      loadgen
    };
    Calculation();
    ~Calculation();

    void startingCalculations(LayoutScheme *layout);
    void endCalculations();

    void createTree(QTreeWidget *tree);

private:
    LayoutScheme *m_layout;

    Matrix m_condactionMatrix;
    Matrix m_invcondactionMatrix;

    GroupsVal m_Z;
    GroupsVal I_first;
    FinalCurrent I_final;

    void inverse();
    void out();
    void Zcalc();
    void Icalc();
    QString conver(const complexnum &num);
    QString Zname(MutualPair *pair, Node *currentNode);
    QString Iname(MutualPair *pair, Node *currentNode);
    QString Ifinalname(MutualPair *pair, Node *currentNode);
    complexnum Z(const MutualPair &pair, Node *currentNode);

    struct MutualPair
    {
        Node *m_node, *m_mutualNode;
        Mutual_Type m_mutualType;
    };
};

#endif // CALCULATION_H
