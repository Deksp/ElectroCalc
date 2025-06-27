#include "calculation.h"
#include <QDebug>
#include <chrono>
#include <QRegExp>
#include <QtMath>


class SimpleTimer
{
    std::chrono::time_point<std::chrono::_V2::system_clock,std::chrono::duration<double>> start,end;
public:
    SimpleTimer()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~SimpleTimer()
    {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end-start;
        qDebug()<<"CRONO = "<<dur.count()<<"s";
    }
};

static double complex_abs(complexnum c)
{
    return sqrt(pow(c.real(),2)+pow(c.imag(),2));
}

QDebug& operator<<(QDebug& out, complexnum num)
{
    out<<"("<<num.real()<<","<<num.imag()<<")";
    return  out;
}

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

void Calculation::stepCalculations(LayoutScheme *layout)
{
    //SimpleTimer stimer;
    m_layout = layout;
    uint size = layout->getVertexes().size();
    m_condactionMatrix.clear();
    m_invcondactionMatrix.clear();
    m_condactionMatrix.resize(size);
    m_invcondactionMatrix.resize(size);

    //while (!iterCondition) {
if (firstCalc){
    for (auto it : m_layout->getBranchs())
    {
        qDebug().noquote()<<QString("Z(%1->%2) = ").arg(it->getFirstNode()->getIndex(), it->getSecondNode()->getIndex())
                         <<conver(it->getResistance());
        qDebug().noquote()<<QString("Y(%1->%2) = ").arg(it->getFirstNode()->getIndex(), it->getSecondNode()->getIndex())
                         <<conver((complexnum)1/it->getResistance());
    }
    qDebug()<<"  ";
    firstCalc = false;
}
else Iter();
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
                            m_condactionMatrix[i][j] += complexnum(1)/load->getResistance();
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
        scocIt++;
        if (scocIt > 1)
        {
            volatile int a =0;
        }
        VoltCalc();

qDebug()<<" ";
qDebug()<<"Iteration: "<<scocIt;
    out();

}

void Calculation::startingCalculations(LayoutScheme *layout, int value)
{
    //SimpleTimer stimer;
    m_layout = layout;
    uint size = layout->getVertexes().size();
    m_condactionMatrix.clear();
    m_invcondactionMatrix.clear();
    m_condactionMatrix.resize(size);
    m_invcondactionMatrix.resize(size);
value--;
    while (!iterCondition && scocIt<=value) {
if (firstCalc)
firstCalc = false;
else Iter();
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
                            m_condactionMatrix[i][j] += complexnum(1)/load->getResistance();
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
        scocIt++;
        if (scocIt > 1)
        {
            volatile int a =0;
        }
        VoltCalc();

qDebug()<<" ";
qDebug()<<"Iteration: "<<scocIt;
    out();
    }
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
    tree->headerItem()->setText(0, "");
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
    for  (QMap<QString, complexnum>::iterator it=branchCurrentName.begin();it!=branchCurrentName.end();++it)
    {
        VertexesCurrents.append(new QTreeWidgetItem(topicsItems[4], QStringList(QString(it.key() + " = " + conver(it.value()) + " A"))));
        /*QList<QTreeWidgetItem *> Currents;
        for (QMap<QString, complexnum>::iterator I=vertex.begin();I!=vertex.end();++I)
            Currents.append(new QTreeWidgetItem(VertexesCurrents[i],
                                                 QStringList(I.key() + " = " + conver(I.value()) + "A")));
        ++i;*/
    }
    i = 0;
    QList<QTreeWidgetItem *> CheckVertexesCurrents;
    for  (auto vertex : I_final)
    {
        complexnum num;
        for (QMap<QString, complexnum>::iterator I=vertex.begin();I!=vertex.end();++I)
            num += I.value();
        CheckVertexesCurrents.append(new QTreeWidgetItem(topicsItems[5],
                                                 QStringList(QObject::tr("Vertex %1 Amount without rounding\n ").arg(1+i) + conver(num))));
        CheckVertexesCurrents.append(new QTreeWidgetItem(topicsItems[5],
                                                 QStringList(QObject::tr("Vertex %1 The amount with rounding\nto the third digit\n ").arg(1+i) +
                                                             QString::number(num.real(), 'f', 3) +
                                                             " + " + QString::number(num.imag(), 'f', 3) + "i")));
        CheckVertexesCurrents.append(new QTreeWidgetItem(topicsItems[5],QStringList(QString(" "))));
        ++i;
    }
}



void Calculation::inverse()
{
    //SimpleTimer stim;
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
{int i =0;
    qDebug()<<"  ";

    for (auto it : m_layout->getLoads())
    {
        qDebug().noquote()<<QString("Z(%1[%2]) = ").arg(it->getAssignedNode()->getIndex(),
        QString::number(static_cast<LayoutScheme::VertexNode*>(it->getAssignedNode())->getLoadIdAt(it)))
                         <<conver(it->getResistance());
        qDebug().noquote()<<QString("Y(%1[%2]) = ").arg(it->getAssignedNode()->getIndex(),
        QString::number(static_cast<LayoutScheme::VertexNode*>(it->getAssignedNode())->getLoadIdAt(it)))
                         <<conver((complexnum)1/it->getResistance());
    }

    qDebug()<<"  ";
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
    for (QMap<QString, complexnum>::iterator it=branchCurrentName.begin();it!=branchCurrentName.end();++it)
    {
        if (it.value().real()<0)
        {
            QRegExp pat("I\\(V\\d+->V\\d+\\)");

            if (pat.indexIn(it.key()) > -1)
            {
                QRegExp Num("(\\d+)");
                QStringList list;
                int pos = 0;

                while ((pos = Num.indexIn(it.key(), pos)) != -1) {
                    list << Num.cap(1);
                    pos += Num.matchedLength();
                }
                qDebug().noquote()<<QString("I(V%1->V%2)").arg(list[1],list[0]) <<" = " + conver(it.value()) + " A";
            }
        }
        qDebug().noquote()<<it.key() + " = " + conver(it.value()) + " A";
    }
    qDebug()<<"  ";
    for (QMap<QString, complexnum>::iterator it=voltageDropName.begin();it!=voltageDropName.end();++it)
    {
        qDebug().noquote()<<it.key() + " = " <<
                            //conver(it.value()) << " V";
                            std::abs(it.value()) << " V";
    }
    qDebug()<<"  ";
    for (QMap<QString, complexnum>::iterator it=voltageDropName.begin();it!=voltageDropName.end();++it)
    {
        qDebug().noquote()<<it.key() + " = " <<
                            conver(it.value()) << " V";
                            //std::abs(it.value()) << " V";
    }

}

void Calculation::Zcalc()
{
    for (auto ver : m_Z)
    {
        for (MutualVals::iterator t=ver.begin();t!=ver.end();++t)
            delete t.key();
    }
    m_Z.clear();
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
    I_first.clear();
    I_final.clear();
    branchCurrentName.clear();
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
    for (auto vert : I_final)
    {
        for (BranchsCurrentName::iterator Icur=vert.begin();Icur!=vert.end();++Icur)
            if (!branchCurrentName.contains(trueCurrentName(Icur.key())))
                if (Icur.key() == trueCurrentName(Icur.key()))
                {
                    QRegExp paternBranch("I\\(V\\d+->V\\d+\\)");

                    int sovp = paternBranch.indexIn(Icur.key());
                    if(sovp > -1)
                        branchCurrentName[trueCurrentName(Icur.key())] = Icur.value() * complexnum(-1);
                    else
                        branchCurrentName[trueCurrentName(Icur.key())] = Icur.value();
                }
    }
}

complexnum myabsForIwhithU(complexnum c)
{
    return c.real() >= 0 ? c : c*complexnum(-1);
}

void Calculation::VoltCalc()
{
    voltageDrop.clear();
    voltageDropName.clear();
    for (BranchsCurrentName::iterator it=branchCurrentName.begin();it!=branchCurrentName.end();++it)
    {
        QString forma(it.key());
        forma[0] = 'U';
        auto curentNode = static_cast<LayoutScheme::BranchNode*>(returnNodeFromTextCurrent(it.key()));
        auto rZ = static_cast<LayoutScheme::BranchNode*>
                (curentNode)->getResistance();
        //qDebug()<<conver(rZ);
        auto Ielem =myabsForIwhithU(it.value());
        complexnum volt = complexnum(rZ * Ielem);
        voltageDrop[returnNodeFromTextCurrent(it.key())] = volt;
        voltageDropName[forma] = volt;
    }
}

Node *Calculation::returnNodeFromTextCurrent(QString zapis)
{
    QRegExp paternGen("I\\(E\\d+\\)");
    QRegExp paternBranch("I\\(V\\d+->V\\d+\\)");
    QRegExp paternLoad("I\\(V\\d+\\[\\d+\\]\\)");

    int sovppaternGen = paternGen.indexIn(zapis);
    int sovppaternBranch = paternBranch.indexIn(zapis);
    int sovppaternLoad = paternLoad.indexIn(zapis);

    if(sovppaternGen > -1)
    {
        QRegExp Num("(\\d+)");
        QStringList list;
        int pos = 0;

        while ((pos = Num.indexIn(zapis, pos)) != -1) {
            list << Num.cap(1);
            pos += Num.matchedLength();
        }
        uint32_t firstNum = list[0].toInt() - 1;
        return m_layout->getGenerator(firstNum)->getBranchs()[0];
    }
    if(sovppaternBranch > -1)
    {

        QRegExp Num("(\\d+)");
        QStringList list;
        int pos = 0;

        while ((pos = Num.indexIn(zapis, pos)) != -1) {
            list << Num.cap(1);
            pos += Num.matchedLength();
        }
        uint32_t firstNum = list[0].toInt() - 1;
        uint32_t secondNum = list[1].toInt() - 1;
        return m_layout->getVertex(firstNum)->getBranchAt(m_layout->getVertex(secondNum));
    }
    if(sovppaternLoad > -1)
    {

        QRegExp Num("(\\d+)");
        QStringList list;
        int pos = 0;

        while ((pos = Num.indexIn(zapis, pos)) != -1) {
            list << Num.cap(1);
            pos += Num.matchedLength();
        }
        uint32_t firstNum = list[0].toInt() - 1;
        uint32_t secondNum = list[1].toInt() - 1;
        return m_layout->getVertex(firstNum)->getLoads()[secondNum];
    }
    return 0;
}
bool condSovp(complexnum newZ, complexnum oldZ){
    double procDizSovp = 0.01;
    double realProc;
    if (complex_abs(newZ) > complex_abs(oldZ))
        realProc = (complex_abs(newZ) / complex_abs(oldZ))-1;
    else
        realProc = (complex_abs(oldZ) / complex_abs(newZ))-1;
    return procDizSovp > realProc;
}
void Calculation::Iter()
{
    complexnum soprZ;
    for (auto node : m_layout->getLoads())
    {
        soprZ = (sqrt(3)*std::pow(std::abs(voltageDrop[node]),2))/complexnum(node->getPowerful().real(),-1*node->getPowerful().imag());
        auto newZ = complexnum(soprZ.real(),soprZ.imag());
        //auto newZ = complexnum((sqrt(3)*std::pow(voltageDrop[node],2))/node->getPowerful());
        iterCondition = true;
        if (!condSovp(newZ,node->getResistance()))
        {
            if (newZ.real() <= 0)
            {
                //qDebug()<<conver(voltageDrop[node])<<conver(std::pow(voltageDrop[node],2))<<conver(sqrt(3)*std::pow(voltageDrop[node],2))
                  //     <<conver(soprPowerful)+" "<<conver(soprZ)<<conver(newZ);
            }
            node->setResistance(newZ);
            iterCondition = false;
        }
    }
}

QString Calculation::conver(const complexnum &num)
{
    if(num.imag()>=0)
        return QString("%1 +%2i").arg(QString::number(num.real()), QString::number(num.imag()));
    return QString("%1 %2i").arg(QString::number(num.real()), QString::number(num.imag()));
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

QString Calculation::trueCurrentName(QString currentFinalName)
{
    QRegExp paternBranch("I\\(V\\d+->V\\d+\\)");

    int sovp = paternBranch.indexIn(currentFinalName);
    if(sovp > -1)
    {

        QRegExp Num("(\\d+)");
        QStringList list;
        int pos = 0;

        while ((pos = Num.indexIn(currentFinalName, pos)) != -1) {
            list << Num.cap(1);
            pos += Num.matchedLength();
        }
        uint32_t firstNum = list[0].toInt();
        uint32_t secondNum = list[1].toInt();

        if(firstNum > secondNum)
            return QString("I(V%1->V%2)").arg(secondNum,firstNum);
        return currentFinalName;
    }
    else
        return currentFinalName;
}
