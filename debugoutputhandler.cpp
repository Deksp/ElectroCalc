#include "debugoutputhandler.h"
#include "mainwindow.h"
#include <QMetaObject>

SidePlainTextWidget *DebugOutputHandler::s_widget = nullptr;
QMutex DebugOutputHandler::s_mutex;

void DebugOutputHandler::install(SidePlainTextWidget *widget)
{
    QMutexLocker locker(&s_mutex);
    s_widget = widget;
    qInstallMessageHandler(DebugOutputHandler::messageHandler);
}

void DebugOutputHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    QMutexLocker locker(&s_mutex);
    static const QStringList skipList = {
        "Unsupported ICC profile class",
        "fromIccProfile: failed general sanity check",
        "QPngHandler: Failed to parse ICC profile"
    };
    for (const auto& pattern : skipList) {
        if (msg.contains(pattern)) return;
    }
    if (s_widget) {
        QMetaObject::invokeMethod(s_widget, [msg, w=s_widget](){
            w->appendPlainText(msg);
        }, Qt::QueuedConnection);
    }
}
