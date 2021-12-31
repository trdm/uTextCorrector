# https://forum.mista.ru/topic.php?id=874910
# E:\Projects\_Utils\uTextCorrector\__\DSpellCheck-1.4.21\deps\Hunspell\src\hunspell
# C:\Program Files\CMake\bin
# https://wiki.qt.io/Spell-Checking-with-Hunspell
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/settingsdlg.cpp \
    src/uTextCorrector.cpp \
    src/widget.cpp

HEADERS += \
    src/settingsdlg.h \
    src/uTextCorrector.h \
    src/widget.h

FORMS += \
    src/settingsdlg.ui \
    src/widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#C:\Program Files\CMake\bin\deps\Hunspell\Debug
#LIBS += Llib
debug {
#   LIBS += -llib/hunspelld
}
release {
#   LIBS += -llib/hunspell
}


#DISTFILES += \
#    lib/release/hunspell.lib
