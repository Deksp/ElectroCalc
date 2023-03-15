QT += core gui widgets

TRANSLATIONS = Translations/Stady_ru.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    calculation.cpp \
    layoutscheme.cpp \
    main.cpp \
    mainwindow.cpp \
    node.cpp \
    schemeitem.cpp \
    schemescene.cpp \
    viewscheme.cpp

HEADERS += \
    calculation.h \
    layoutscheme.h \
    mainwindow.h \
    node.h \
    schemeitem.h \
    schemescene.h \
    viewscheme.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
