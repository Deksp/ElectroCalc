#ifndef CALCULATION_H
#define CALCULATION_H

#include <QMap>

#include "layoutscheme.h"

typedef QVector<QVector<std::complex<double>>> Matrix;

class Calculation
{
    struct MutualPair;
    typedef QMap<MutualPair*, complexnum> MutualVals;
    typedef QVector<MutualVals>  GroupsVal;

public:
    enum Mutual_Type{
      gen,
      gengen,
      branchgen,
      loadgen
    };
    Calculation();

    void startingCalculations(LayoutScheme *layout);

private:
    LayoutScheme *m_layout;

    Matrix m_condactionMatrix;
    Matrix m_invcondactionMatrix;

    GroupsVal m_Z;
    MutualVals I_first;
    MutualVals I_second;
    MutualVals I_final;

    void inverse();
    void OUTCHECK(LayoutScheme *layout);
    void OUT(LayoutScheme *layout);
    void Zcalc(LayoutScheme *layout);
    void Icalc(LayoutScheme *layout);
    QString Zname(MutualPair &pair, Node *currentNode);
    QString Iname(MutualPair &pair, Node *currentNode);
    complexnum Z(const MutualPair &pair, Node *currentNode);

    struct MutualPair
    {
        MutualPair(Node *node, Node *mutualNode, Mutual_Type mutualtype);
        Node *node, *mutualNode;
        Mutual_Type mutualType;
    };
};

#endif // CALCULATION_H
