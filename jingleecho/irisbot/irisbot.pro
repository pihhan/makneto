######################################################################
# Automatically generated by qmake (2.01a) so 1 23 15:28:06 2010
######################################################################

TEMPLATE = app
TARGET = 
CONFIG += console
CONFIG += qt 
CONFIG += debug
CONFIG += crypto
QT += xml network
DEPENDPATH += .
INCLUDEPATH += .

DEFINES += IRIS

CONFIG += link_pkgconfig
PKGCONFIG += farsight2-0.10

# Input
SOURCES += irisbot.cpp
HEADERS += irisbot.h

IRISPATH = /home/pihhan/Projekty/xmpp/psi-git/iris
INCLUDEPATH += $$IRISPATH/include
INCLUDEPATH += $$IRISPATH/src
INCLUDEPATH += $$IRISPATH/src/xmpp/xmpp-core
INCLUDEPATH += $$IRISPATH/src/xmpp/xmpp-im

INCLUDEPATH += ..
INCLUDEPATH += ../..

#INCLUDEPATH += /home/pihhan/Projekty/xmpp/psi-git/iris/include
#INCLUDEPATH += /home/pihhan/Projekty/xmpp/psi-git/iris/src

LIBS += -L/home/pihhan/Projekty/xmpp/psi-git/iris/lib
LIBS += -liris -lirisnet

LIBS += -L..
LIBS += -ljingleecho


