QT += core gui widgets

TRANSLATIONS = Translations/Stady_ru.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++23

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


win32:RC_FILE = logores.rc

INCLUDEPATH += ./boost_mingw_64/include/boost-1_83
LIBS += -LC:/My_DIRECTORY/QT_Projects/ElectroCalc/ElectroCalc/boost_mingw_64/lib -llibboost_locale-mgw7-mt-x64-1_83

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
