#include "calculation.h"
#include <QDebug>

Calculation::Calculation()
{

}

void Calculation::startingCalculations(LayoutScheme *layout)
{
    OUTCHECK(layout);
    uint size = layout->getVertexes().size();
    m_condactionMatrix.resize(size);
    m_invcondactionMatrix.resize(size);
    for (uint i=0; i<size; i++)
    {
        m_condactionMatrix[i].resize(size);
        m_invcondactionMatrix[i].resize(size);
        for (uint j=0; j<size; j++)
        {
            if (i==j)
            {
                for (auto branch : layout->getVertex(i)->getBranchs())
                    if (branch->getResistance()!=complexnum(0))
                        m_condactionMatrix[i][j] += complexnum(1)/branch->getResistance();
                for (auto load : layout->getVertex(i)->getLoads())
                    if (load->getResistance()!=complexnum(0))
                        m_condactionMatrix[i][j] += load->getResistance();
                m_condactionMatrix[i][j] *= complexnum(-1);
            }
            else if (layout->getVertex(i)->getBranchAt(layout->getVertex(j)) != nullptr &&
                     layout->getVertex(i)->getBranchAt(layout->getVertex(j))->getResistance()!=complexnum(0))
                m_condactionMatrix[i][j] = complexnum(1)/
                        layout->getVertex(i)->getBranchAt(layout->getVertex(j))->getResistance();
        }
    }
    inverse();
    qDebug()<<"catch37";
    Zcalc(layout);
    qDebug()<<"catch39";
    Icalc(layout);
    //OUT(layout);
}

Calculation::MutualPair::MutualPair(Node *node, Node *mutualNode, Mutual_Type mutualtype)
    :  node(node), mutualNode(mutualNode), mutualType(mutualtype)
{

}

void Calculation::inverse()
{
    int N=m_condactionMatrix.length();

    complexnum **matrix = new complexnum *[N];
    for (int i = 0; i < N; i++)
        matrix[i] = new complexnum [N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i][j] = m_condactionMatrix[i][j];

    complexnum temp;
    complexnum **E = new complexnum *[N];
    for (int i = 0; i < N; i++)
        E[i] = new complexnum [N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            E[i][j] = 0.0;
            if (i == j)
                E[i][j] = 1.0;
        }
    for (int k = 0; k < N; k++)
    {
        temp = matrix[k][k];
        for (int j = 0; j < N; j++)
        {
            matrix[k][j] /= temp;
            E[k][j] /= temp;
        }
        for (int i = k + 1; i < N; i++)
        {
            temp = matrix[i][k];
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] -= matrix[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }
    for (int k = N - 1; k > 0; k--)
    {
        for (int i = k - 1; i >= 0; i--)
        {
            temp = matrix[i][k];
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] -= matrix[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i][j] = E[i][j];
    for (int i = 0; i < N; i++)
        delete [] E[i];
    delete [] E;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            m_invcondactionMatrix[i][j] = matrix[i][j];

    for (int i = 0; i < N; i++)
        delete [] matrix[i];
    delete [] matrix;
}

void Calculation::OUTCHECK(LayoutScheme *layout)
{
    qDebug()<<"OUTCHECK";
    for (auto ver : layout->getVertexes())
    {
        qDebug()<<ver;
        for (auto load : ver->getLoads())
            qDebug().space()<<load;
        for (auto branch : ver->getBranchs())
        {
            qDebug().space()<< branch;
            qDebug().space().space()<<branch->getFirstNode();
            qDebug().space().space()<<branch->getSecondNode();
        }
        qDebug()<<" ";
    }
    for (auto gen : layout->getGenerators())
    {
        qDebug()<<gen;
        for (auto branch : gen->getBranchs())
            qDebug().space()<< branch;
    }
    qDebug()<<"OUTCHECK";
}

void Calculation::OUT(LayoutScheme *layout)
{
    for (auto ver : m_Z)
    {
        for (MutualVals::iterator t=ver.begin();t!=ver.end();++t)
            qDebug()<<t.key()->mutualType<<t.value().real();
        qDebug()<<"  ";
    }
}

void Calculation::Zcalc(LayoutScheme *layout)
{
    for (auto vertex : layout->getVertexes())
    {
        MutualVals mutualVals;
        for (auto generator : layout->getGenerators())
        {
            if (vertex->isLink(generator))
            {
                MutualPair pairgen(generator,generator,gen);
                mutualVals.insert(&pairgen, Z(pairgen,vertex));

                for (auto mutualGenerator : layout->getGenerators())
                {
                    if (mutualGenerator == generator)
                        continue;
                    MutualPair pairgengen(generator,mutualGenerator,gengen);
                    mutualVals.insert(&pairgengen, Z(pairgengen, vertex));
                }
                for (auto branch : vertex->getBranchs())
                    if (branch->type() != Node::TypeGenBranchNode)
                    {
                        MutualPair pairbranch(branch,generator,branchgen);
                        mutualVals.insert(&pairbranch, Z(pairbranch, vertex));
                    }
            }
            else
                for (auto branch : vertex->getBranchs())
                    if (branch->type() != Node::TypeGenBranchNode)
                    {
                        MutualPair pairbranch(branch,generator,branchgen);
                        mutualVals.insert(&pairbranch, Z(pairbranch, vertex));
                    }
            for (auto load : vertex->getLoads())
                if (load->getResistance() != complexnum(0))
                {
                    MutualPair pairload(load,generator,loadgen);
                    mutualVals.insert(&pairload, Z(pairload, vertex));
                }
        }
        m_Z << mutualVals;
    }
}

void Calculation::Icalc(LayoutScheme *layout)
{

}

QString Calculation::Zname(Calculation::MutualPair &pair, Node *currentNode)
{
    switch (pair.mutualType)
    {
    case gen:
        return QString("Z(%1,%1)").arg(pair.node->getIndex());
        break;
    case gengen:
        return QString("Z(%1,%2)").arg(pair.node->getIndex(),
                                       pair.mutualNode->getIndex());
        break;
    case branchgen:
        return QString("Z(%1->%2;%3)").arg(
                    currentNode->getIndex(),
                    pair.node->getAssignedNode(currentNode)->getIndex(),
                    pair.mutualNode->getIndex());
        break;
    case loadgen:
        return QString("Z(%1[%2];%3)").arg(
                            currentNode->getIndex(),
                            QString::number(pair.node->getId()),
                            pair.mutualNode->getIndex());
        break;
    }

}

QString Calculation::Iname(Calculation::MutualPair &pair, Node *currentNode)
{
    switch (pair.mutualType)
    {
    case gen:
        return QString("Z(%1,%1)").arg(pair.node->getIndex());
        break;
    case gengen:
        return QString("Z(%1,%2)").arg(pair.node->getIndex(),
                                       pair.mutualNode->getIndex());
        break;
    case branchgen:
        return QString("Z(%1->%2;%3)").arg(
                    currentNode->getIndex(),
                    pair.node->getAssignedNode(currentNode)->getIndex(),
                    pair.mutualNode->getIndex());
        break;
    case loadgen:
        return QString("Z(%1[%2];%3)").arg(
                            currentNode->getIndex(),
                            QString::number(pair.node->getId()),
                            pair.mutualNode->getIndex());
        break;
    }
}

complexnum Calculation::Z(const MutualPair &pair, Node *currentNode)
{
    switch (pair.mutualType)
    {
    case gen:
        return
      pair.node->getTypeNodeProperty() * pair.node->getTypeNodeProperty()
                                /
(pair.node->getTypeNodeProperty() + m_invcondactionMatrix[pair.node->getAssignedNode()->getId()-1]
                                        [pair.node->getAssignedNode()->getId()-1]);
        break;
    case gengen:
        return
   (pair.node->getTypeNodeProperty() * pair.mutualNode->getTypeNodeProperty())
                                 /
     m_invcondactionMatrix[pair.node->getAssignedNode()->getId()-1]
                            [pair.mutualNode->getAssignedNode()->getId()-1];
        break;
    case branchgen:
        return
       (pair.node->getTypeNodeProperty() * pair.mutualNode->getTypeNodeProperty())
                                       /
    (m_invcondactionMatrix[currentNode->getId()-1]
                          [pair.mutualNode->getAssignedNode()->getId()-1] -
                m_invcondactionMatrix[pair.mutualNode->getAssignedNode()->getId()-1]
                                     [pair.mutualNode->getAssignedNode()->getId()-1]);
        break;
    case loadgen:
        return
          (pair.node->getTypeNodeProperty() * pair.mutualNode->getTypeNodeProperty())
                               /
      m_invcondactionMatrix[currentNode->getId()-1]
                                   [pair.mutualNode->getAssignedNode()->getId()-1];
        break;
    }qDebug()<<"catch";
}

