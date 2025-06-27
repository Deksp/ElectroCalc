#ifndef DEBUGOUTPUTHANDLER_H
#define DEBUGOUTPUTHANDLER_H

#include <QObject>
#include <QMutex>

class SidePlainTextWidget;

class DebugOutputHandler : public QObject
{
    Q_OBJECT
public:
    static void install(SidePlainTextWidget *widget);
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:
    void newDebugMessage(const QString &msg);

private:
    static SidePlainTextWidget *s_widget;
    static QMutex s_mutex;
};

#endif // DEBUGOUTPUTHANDLER_H
