#include "calculation.h"
#include <QDebug>

Calculation::Calculation()
{

}

Calculation::~Calculation()
{
    for (auto ver : m_Z)
    {
        for (MutualVals::iterator t=ver.begin();t!=ver.end();++t)
            delete t.key();
    }
}

void Calculation::startingCalculations(LayoutScheme *layout)
{
    m_layout = layout;
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
    Zcalc();
    Icalc();
    out();
}

void Calculation::endCalculations()
{

    m_condactionMatrix.clear();
    m_invcondactionMatrix.clear();
    for (auto ver : m_Z)
    {
        for (MutualVals::iterator t=ver.begin();t!=ver.end();++t)
            delete t.key();
    }
    m_Z.clear();
    I_first.clear();
    I_final.clear();
    m_layout = nullptr;
}

void Calculation::createTree(QTreeWidget *tree)
{
    QList<QTreeWidgetItem *> topicsItems;
    QVector<QStringList> topics;
    topics << QStringList(QString(QObject::tr("Condactions matrix")))<< QStringList(QString(QObject::tr("Reverse Condactions matrix")))
              << QStringList(QString(QObject::tr("Mutual condactions")))<< QStringList(QString(QObject::tr("Mutual currents")))
                 << QStringList(QString(QObject::tr("Curents")))<< QStringList(QString(QObject::tr("Check")));
    for (auto i : topics)
        topicsItems.append(new QTreeWidgetItem(i));
    tree->insertTopLevelItems(0, topicsItems);

    QList<QTreeWidgetItem *> CondactionsMatrix;
    QString rows;
    for (auto i : m_condactionMatrix)
    {
        QString colums;
        for (auto j : i)
            colums += conver(j) + "; ";
        rows += colums + "\n";
    }
    CondactionsMatrix.append(new QTreeWidgetItem(topicsItems[0], QStringList(QString(rows))));

    QList<QTreeWidgetItem *> ReverseCondactionsMatrix;
    rows.clear();
    for (auto i : m_invcondactionMatrix)
    {
        QString colums;
        for (auto j : i)
            colums += conver(j) + "; ";
        rows += colums + "\n";
    }
    ReverseCondactionsMatrix.append(new QTreeWidgetItem(topicsItems[1], QStringList(QString(rows))));

    QList<QTreeWidgetItem *> VertexesMutualCondactions;
    int i = 0;
    for  (auto vertex : m_Z)
    {
        VertexesMutualCondactions.append(new QTreeWidgetItem(topicsItems[2], QStringList(QString(QObject::tr("Vertex %1")).arg(1 + i))));
        QList<QTreeWidgetItem *> mutualCondactions;
        for (MutualVals::iterator z=vertex.begin();z!=vertex.end();++z)
            mutualCondactions.append(new QTreeWidgetItem(VertexesMutualCondactions[i],
                                     QStringList(QString(Zname(z.key(), m_layout->getVertex(i)) + " = " + conver(z.value())))));
        ++i;
    }
    i = 0;
    QList<QTreeWidgetItem *> VertexesMutualCurrents;
    for  (auto vertex : I_first)
    {
        VertexesMutualCurrents.append(new QTreeWidgetItem(topicsItems[3], QStringList(QString(QObject::tr("Vertex %1")).arg(1 + i))));
        QList<QTreeWidgetItem *> mutualCurrents;
        for (MutualVals::iterator I=vertex.begin();I!=vertex.end();++I)
            mutualCurrents.append(new QTreeWidgetItem(VertexesMutualCurrents[i],
                                     QStringList(QString(Iname(I.key(), m_layout->getVertex(i)) + " = " + conver(I.value()) + "A"))));
        ++i;
    }
    i = 0;
    QList<QTreeWidgetItem *> VertexesCurrents;
    for  (auto vertex : I_final)
    {
        VertexesCurrents.append(new QTreeWidgetItem(topicsItems[4], QStringList(QString(QObject::tr("Vertex %1")).arg(1 + i))));
        QList<QTreeWidgetItem *> Currents;
        for (QMap<QString, complexnum>::iterator I=vertex.begin();I!=vertex.end();++I)
            Currents.append(new QTreeWidgetItem(VertexesCurrents[i],
                                                 QStringList(I.key() + " = " + conver(I.value()) + "A")));
        ++i;
    }
    i = 0;
    QList<QTreeWidgetItem *> CheckVertexesCurrents;
    for  (auto vertex : I_final)
    {
        CheckVertexesCurrents.append(new QTreeWidgetItem(topicsItems[5], QStringList(QString(QObject::tr("Vertex %1")).arg(1 + i))));
        QList<QTreeWidgetItem *> CheckCurrents;
        complexnum num;
        for (QMap<QString, complexnum>::iterator I=vertex.begin();I!=vertex.end();++I)
            num += I.value();
        CheckCurrents.append(new QTreeWidgetItem(CheckVertexesCurrents[i],
                                                 QStringList(QObject::tr("Amount without rounding\n ") + conver(num))));
        CheckCurrents.append(new QTreeWidgetItem(CheckVertexesCurrents[i],
                                                 QStringList(QObject::tr("The amount with rounding\nto the third digit\n ") +
                                                             QString::number(num.real(), 'f', 3) +
                                                             " + " + QString::number(num.imag(), 'f', 3) + "i")));
        ++i;
    }
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

void Calculation::out()
{
    for (auto i : m_condactionMatrix)
    {
        QString out;
        for (auto j : i)
            out = out + QString("(%1,%2)").arg(QString::number(j.real()),QString::number(j.imag()));
        qDebug().noquote()<<out;
    }
    qDebug()<<" ";
    for (auto i : m_invcondactionMatrix)
    {
        QString out;
        for (auto j : i)
            out = out + QString("(%1,%2)").arg(QString::number(j.real()),QString::number(j.imag()));
        qDebug().noquote()<<out;
    }
    qDebug()<<" ";
    int i =0;
    for (auto ver : m_Z)
    {
        for (MutualVals::iterator t=ver.begin();t!=ver.end();++t)
            qDebug().noquote()<<Zname(t.key(), m_layout->getVertex(i))<<t.value().real()<<t.value().imag();
        qDebug()<<"  ";
        i++;
    }
    qDebug()<<" ";
    i =0;
    for (auto ver : I_first)
    {
        for (MutualVals::iterator t=ver.begin();t!=ver.end();++t)
            qDebug().noquote()<<Iname(t.key(), m_layout->getVertex(i))<<t.value().real()<<t.value().imag();
        qDebug()<<"  ";
        i++;
    }
    qDebug()<<" ";
    i =0;
    for (auto ver : I_final)
    {
        for (QMap<QString, complexnum>::iterator t=ver.begin();t!=ver.end();++t)
            qDebug().noquote()<<t.key()<<t.value().real()<<t.value().imag();
        qDebug()<<"  ";
        i++;
    }
}

void Calculation::Zcalc()
{
    for (auto vertex : m_layout->getVertexes())
    {
        MutualVals mutualVals;
        for (auto generator : m_layout->getGenerators())
        {
            if (vertex->isLink(generator))
            {
                MutualPair *pairgen = new MutualPair{generator,generator,gen};
                mutualVals.insert(pairgen, Z(*pairgen,vertex));
                for (auto mutualGenerator : m_layout->getGenerators())
                {
                    if (mutualGenerator == generator)
                        continue;
                    MutualPair *pairgengen= new MutualPair{generator,mutualGenerator,gengen};
                    mutualVals.insert(pairgengen, Z(*pairgengen, vertex));
                }
                for (auto branch : vertex->getBranchs())
                    if (branch->type() != Node::TypeGenBranchNode)
                    {
                        MutualPair *pairbranch= new MutualPair{branch,generator,branchgen};
                        mutualVals.insert(pairbranch, Z(*pairbranch, vertex));
                    }
            }
            else
                for (auto branch : vertex->getBranchs())
                    if (branch->type() != Node::TypeGenBranchNode)
                    {
                        MutualPair *pairbranch= new MutualPair{branch,generator,branchgen};
                        mutualVals.insert(pairbranch, Z(*pairbranch, vertex));
                    }
            for (auto load : vertex->getLoads())
                if (load->getResistance() != complexnum(0))
                {
                    MutualPair *pairload= new MutualPair{load,generator,loadgen};
                    mutualVals.insert(pairload, Z(*pairload, vertex));
                }
        }
        m_Z << mutualVals;
    }
}

void Calculation::Icalc()
{
    for (auto Zcondaction : m_Z)
    {
        MutualVals map;
        for (MutualVals::iterator it=Zcondaction.begin();it!=Zcondaction.end();++it)
        {
            switch (it.key()->m_mutualType)
            {
            case gen:
                map.insert(it.key(), m_layout->getVoltage(it.key()->m_node)
                                                /
                                            it.value());
                break;
            case gengen:
                map.insert(it.key(), m_layout->getVoltage(it.key()->m_mutualNode)
                                                /
                                            it.value());
                break;
            case branchgen:
                map.insert(it.key(), m_layout->getVoltage(it.key()->m_mutualNode)
                                                /
                                            it.value());
                break;
            case loadgen:
                map.insert(it.key(), m_layout->getVoltage(it.key()->m_mutualNode)
                                                /
                                            it.value());
                break;
            }
        }
        I_first << map;
    }
    int vertexid_1 = 0;
    for (auto Icurrent : I_first)
    {
        QMap<QString, complexnum> map;
        for (MutualVals::iterator it=Icurrent.begin();it!=Icurrent.end();++it)
        {
                map[Ifinalname(it.key(), m_layout->getVertex(vertexid_1))] +=
                        it.value();
        }
        I_final << map;
        vertexid_1++;
    }
}

QString Calculation::conver(const complexnum &num)
{
    if(num.imag()>=0)
        return QString("%1+%2i").arg(QString::number(num.real()), QString::number(num.imag()));
    return QString("%1%2i").arg(QString::number(num.real()), QString::number(num.imag()));
}

QString Calculation::Zname(Calculation::MutualPair *pair, Node *currentNode)
{
    switch (pair->m_mutualType)
    {
    case gen:
        return QString("Z(%1,%1)").arg(pair->m_node->getIndex());
        break;
    case gengen:
        return QString("Z(%1,%2)").arg(pair->m_node->getIndex(),
                                       pair->m_mutualNode->getIndex());
        break;
    case branchgen:
        return QString("Z(%1->%2;%3)").arg(
                    currentNode->getIndex(),
                    pair->m_node->getAssignedNode(currentNode)->getIndex(),
                    pair->m_mutualNode->getIndex());
        break;
    case loadgen:
        return QString("Z(%1[%2];%3)").arg(
                            currentNode->getIndex(),
                            QString::number(pair->m_node->getId()),
                            pair->m_mutualNode->getIndex());
        break;
    }

}

QString Calculation::Iname(Calculation::MutualPair *pair, Node *currentNode)
{
    switch (pair->m_mutualType)
    {
    case gen:
        return QString("I(%1,%1)").arg(pair->m_node->getIndex());
        break;
    case gengen:
        return QString("I(%1,%2)").arg(pair->m_node->getIndex(),
                                       pair->m_mutualNode->getIndex());
        break;
    case branchgen:
        return QString("I(%1->%2;%3)").arg(
                    currentNode->getIndex(),
                    pair->m_node->getAssignedNode(currentNode)->getIndex(),
                    pair->m_mutualNode->getIndex());
        break;
    case loadgen:
        return QString("I(%1[%2];%3)").arg(
                            currentNode->getIndex(),
                            QString::number(pair->m_node->getId()),
                            pair->m_mutualNode->getIndex());
        break;
    }
}

QString Calculation::Ifinalname(Calculation::MutualPair *pair, Node *currentNode)
{
    switch (pair->m_mutualType)
    {
    case gen:
    case gengen:
        return QString("I(%1)").arg(pair->m_node->getIndex());
        break;
    case branchgen:
        return QString("I(%1->%2)").arg(
                    currentNode->getIndex(),
                    pair->m_node->getAssignedNode(currentNode)->getIndex());
        break;
    case loadgen:
        return QString("I(%1[%2])").arg(
                            currentNode->getIndex(),
                            QString::number(pair->m_node->getId()));
        break;
    }
}

complexnum Calculation::Z(const MutualPair &pair, Node *currentNode)
{
    switch (pair.m_mutualType)
    {
    case gen:
        return
      pair.m_node->getTypeNodeProperty() * pair.m_node->getTypeNodeProperty()
                                /
(pair.m_node->getTypeNodeProperty() + m_invcondactionMatrix[pair.m_node->getAssignedNode()->getId()-1]
                                        [pair.m_node->getAssignedNode()->getId()-1]);
        break;
    case gengen:
        return
   (pair.m_node->getTypeNodeProperty() * pair.m_mutualNode->getTypeNodeProperty())
                                 /
     m_invcondactionMatrix[pair.m_node->getAssignedNode()->getId()-1]
                            [pair.m_mutualNode->getAssignedNode()->getId()-1];
        break;
    case branchgen:
        return
       (pair.m_node->getTypeNodeProperty() * pair.m_mutualNode->getTypeNodeProperty())
                                       /
    (m_invcondactionMatrix[currentNode->getId()-1]
                          [pair.m_mutualNode->getAssignedNode()->getId()-1] -
                m_invcondactionMatrix[pair.m_node->getAssignedNode(currentNode)->getId()-1]
                                     [pair.m_mutualNode->getAssignedNode()->getId()-1]);
        break;
    case loadgen:
        return
          (pair.m_node->getTypeNodeProperty() * pair.m_mutualNode->getTypeNodeProperty())
                               /
      m_invcondactionMatrix[currentNode->getId()-1]
                                   [pair.m_mutualNode->getAssignedNode()->getId()-1];
        break;
    }
}
