#############################################################################
# Makefile for building: qtplay
# Generated by qmake (2.01a) (Qt 4.5.2) on: p� 4 23 23:36:53 2010
# Project:  qtplay.pro
# Template: app
# Command: /usr/bin/qmake -unix -o Makefile qtplay.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -g -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -g -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I.
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)  -L/usr/lib -pthread -pthread -lgstinterfaces-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -lQtGui -lQtCore -lpthread
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -sf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = mainwin.cpp \
		player.cpp \
		gst.cpp \
		playerwidgetitem.cpp \
		aspectedlayout.cpp \
		volumebar.cpp \
		audiowatcher.cpp \
		bindisplay.cpp \
		gstbinmodel.cpp \
		cpuwidget.cpp \
		cpucounter.cpp moc_mainwin.cpp \
		moc_player.cpp \
		moc_volumebar.cpp \
		moc_cpuwidget.cpp
OBJECTS       = mainwin.o \
		player.o \
		gst.o \
		playerwidgetitem.o \
		aspectedlayout.o \
		volumebar.o \
		audiowatcher.o \
		bindisplay.o \
		gstbinmodel.o \
		cpuwidget.o \
		cpucounter.o \
		moc_mainwin.o \
		moc_player.o \
		moc_volumebar.o \
		moc_cpuwidget.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/link_pkgconfig.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		qtplay.pro
QMAKE_TARGET  = qtplay
DESTDIR       = 
TARGET        = qtplay

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: qtplay.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/link_pkgconfig.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -unix -o Makefile qtplay.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/link_pkgconfig.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile qtplay.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/qtplay1.0.0 || $(MKDIR) .tmp/qtplay1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/qtplay1.0.0/ && $(COPY_FILE) --parents mainwin.h player.h gsttest.h playerwidgetitem.h aspectedlayout.h volumebar.h audiowatcher.h bindisplay.h gstbinmodel.h cpuwidget.h cpucounter.h .tmp/qtplay1.0.0/ && $(COPY_FILE) --parents mainwin.cpp player.cpp gst.cpp playerwidgetitem.cpp aspectedlayout.cpp volumebar.cpp audiowatcher.cpp bindisplay.cpp gstbinmodel.cpp cpuwidget.cpp cpucounter.cpp .tmp/qtplay1.0.0/ && (cd `dirname .tmp/qtplay1.0.0` && $(TAR) qtplay1.0.0.tar qtplay1.0.0 && $(COMPRESS) qtplay1.0.0.tar) && $(MOVE) `dirname .tmp/qtplay1.0.0`/qtplay1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/qtplay1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_mainwin.cpp moc_player.cpp moc_volumebar.cpp moc_cpuwidget.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainwin.cpp moc_player.cpp moc_volumebar.cpp moc_cpuwidget.cpp
moc_mainwin.cpp: player.h \
		gsttest.h \
		audiowatcher.h \
		mainwin.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) mainwin.h -o moc_mainwin.cpp

moc_player.cpp: player.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) player.h -o moc_player.cpp

moc_volumebar.cpp: audiowatcher.h \
		volumebar.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) volumebar.h -o moc_volumebar.cpp

moc_cpuwidget.cpp: cpucounter.h \
		cpuwidget.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) cpuwidget.h -o moc_cpuwidget.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

mainwin.o: mainwin.cpp player.h \
		playerwidgetitem.h \
		mainwin.h \
		gsttest.h \
		audiowatcher.h \
		aspectedlayout.h \
		volumebar.h \
		bindisplay.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwin.o mainwin.cpp

player.o: player.cpp player.h \
		playerwidgetitem.h \
		volumebar.h \
		audiowatcher.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o player.o player.cpp

gst.o: gst.cpp gsttest.h \
		audiowatcher.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gst.o gst.cpp

playerwidgetitem.o: playerwidgetitem.cpp playerwidgetitem.h \
		player.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o playerwidgetitem.o playerwidgetitem.cpp

aspectedlayout.o: aspectedlayout.cpp aspectedlayout.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o aspectedlayout.o aspectedlayout.cpp

volumebar.o: volumebar.cpp volumebar.h \
		audiowatcher.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o volumebar.o volumebar.cpp

audiowatcher.o: audiowatcher.cpp audiowatcher.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o audiowatcher.o audiowatcher.cpp

bindisplay.o: bindisplay.cpp gstbinmodel.h \
		bindisplay.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o bindisplay.o bindisplay.cpp

gstbinmodel.o: gstbinmodel.cpp gstbinmodel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gstbinmodel.o gstbinmodel.cpp

cpuwidget.o: cpuwidget.cpp cpuwidget.h \
		cpucounter.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cpuwidget.o cpuwidget.cpp

cpucounter.o: cpucounter.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cpucounter.o cpucounter.cpp

moc_mainwin.o: moc_mainwin.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwin.o moc_mainwin.cpp

moc_player.o: moc_player.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_player.o moc_player.cpp

moc_volumebar.o: moc_volumebar.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_volumebar.o moc_volumebar.cpp

moc_cpuwidget.o: moc_cpuwidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_cpuwidget.o moc_cpuwidget.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

