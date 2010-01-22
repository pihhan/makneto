
#GLOOX_CFLAGS=`pkg-config --cflags gloox`
#GLOOX_LIBS=`pkg-config --libs gloox`

GLOOX_TREE=/home/pihhan/xmpp/gloox-0.9.9.12
GLOOX_CFLAGS=-I$(GLOOX_TREE)
GLOOX_LIBS=-L$(GLOOX_TREE)/src/.libs -lgloox

FARSIGHT_CFLAGS=`pkg-config --cflags farsight2-0.10`
FARSIGHT_LIBS=`pkg-config --libs farsight2-0.10`


CXXFLAGS += $(GLOOX_CFLAGS)
CXXFLAGS += $(FARSIGHT_CFLAGS)
CXXFLAGS += -Wall -g 
#CXXFLAGS += -Iipv6
CXXFLAGS += -I..
LIBS += $(GLOOX_LIBS)
LIBS += $(FARSIGHT_LIBS)

HEADERS = client.h glogger.h parser.h requests.h versionhandler.h \
    jinglesession.h jinglemanager.h ipv6/v6interface.h \
    jingle-content.h jingle-description.h jingle-transport.h \
    jingle-candidate.h pjid.h \
    fstjingle.h session.h conference.h qpipeline.h
SOURCES = client.cpp logger.cpp
OBJECTS = client.o glogger.o main.o parser.o requests.o versionhandler.o \
    jinglesession.o jinglemanager.o \
    jingle-content.o jingle-description.o jingle-transport.o \
    jingle-candidate.o pjid.o \
    fstjingle.o session.o conference.o qpipeline.o
EXTERNAL= ../ipv6/v6interface.o ../logger/logger.o
LIBS += $(EXTERNAL)

all: client

qpipeline.o: qpipeline.h qpipeline.cpp
fstjingle.o: fstjingle.h qpipeline.cpp qpipeline.h
conference.o: conference.h

ipv6:
	make -C ../ipv6

logger:
	make -C ../logger

client: $(HEADERS) $(OBJECTS)
	$(CXX) -o client $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o client
   
tags: $(SOURCES) $(HEADERS)
	ctags $(SOURCES) $(HEADERS)

 
