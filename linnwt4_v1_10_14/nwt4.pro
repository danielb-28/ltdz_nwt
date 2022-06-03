#-------------------------------------------------
#
# Project created by QtCreator 2016-12-24T11:07:26
#
#-------------------------------------------------

QT       += core

#if QT_VERSION >= 0x050000
QT += widgets
#else
QT += gui
#endif

#DEFINES += QT_NO_CAST_FROM_ASCII

!win32 {
    TARGET = NWT4000lin
}
win32 {
    TARGET = NWT4000win
}
TEMPLATE = app


SOURCES += main.cpp\
    configfile.cpp \
    nwt4window.cpp \
    wdisplay.cpp \
    calibdlg.cpp \
    qfmarkedlg.cpp \
    qsetupdlg.cpp \
    hkurveproperty.cpp \
    qpeaklist.cpp \
    qkurvenanalyse.cpp

HEADERS  += configfile.h \
    nwt4window.h \
    konstdef.h \
    wdisplay.h \
    calibdlg.h \
    qfmarkedlg.h \
    qsetupdlg.h \
    hkurveproperty.h \
    qpeaklist.h \
    qkurvenanalyse.h

DISTFILES += nwt4_en.ts

TRANSLATIONS += nwt4_en.ts
QMAKE_EXTRA_TARGETS += transl

!win32 {
target.path = /usr/local/bin/
icon.path = /usr/share/pixmaps/
icon.files = images/NWT4x.png
transl.path = /usr/share/nwt4transl/
transl.files = nwt*.qm

INSTALLS += target
INSTALLS += icon
INSTALLS += transl
}


!win32 {
     SOURCES += lin_modem.cpp
     HEADERS += lin_modem.h
}
win32 {
     SOURCES += win_modem.cpp
     HEADERS += win_modem.h
}
RESOURCES += icon.qrc

FORMS +=

OTHER_FILES += \
    history.txt
