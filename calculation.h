#ifndef CALCULATION_H
#define CALCULATION_H

#include <QMap>

#include "layoutscheme.h"

typedef QVector<QVector<std::complex<double>>> Matrix;

class Calculation
{
    class MutualPair;
    typedef QMap<MutualPair, complexnum> MutualVal;

public:
    enum Mutual_Type{
      gengen,
        genvertex,
        vertexload,
        vertexbranch
    };
    Calculation();

private:
    Matrix condactionMatrix;
    Matrix invcondactionMatrix;

    MutualVal Z;
    MutualVal I_first;
    MutualVal I_second;
    MutualVal I_final;

    /*complexnum z(Node& gen);
    complexnum z(Node& gen, Node& mutualgen);
    complexnum z(Node& ver, Node& mutualgen, int key = 1);
    complexnum z(Node& ver, Node& linkver, Node& mutualgen);*/

    class MutualPair
    {
    public:
        MutualPair(Node *node, Node *mutualNode, Mutual_Type mutualtype);
    };
};

#endif // CALCULATION_H
