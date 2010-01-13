
GLOOX_CFLAGS=`pkg-config --cflags gloox`
GLOOX_LIBS=`pkg-config --libs gloox`

FARSIGHT_CFLAGS=`pkg-config --cflags farsight2-0.10`
FARSIGHT_LIBS=`pkg-config --libs farsight2-0.10`


CXXFLAGS += $(GLOOX_CFLAGS)
CXXFLAGS += $(FARSIGHT_CFLAGS)
CXXFLAGS += -Wall -g 
CXXFLAGS += -Iipv6
LIBS += $(GLOOX_LIBS)
LIBS += $(FARSIGHT_LIBS)

HEADERS = client.h logger.h parser.h requests.h versionhandler.h \
    jinglesession.h jinglemanager.h ipv6/v6interface.h \
    fsjingle.h session.h conference.h qpipeline.h
SOURCES = client.cpp logger.cpp
OBJECTS = client.o logger.o main.o parser.o requests.o versionhandler.o \
    jinglesession.o jinglemanager.o ipv6/v6interface.o \
    fsjingle.o session.o conference.o qpipeline.o

client: $(HEADERS) $(OBJECTS)
	$(CXX) -o client $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o client
   
tags: $(SOURCES) $(HEADERS)
	ctags $(SOURCES) $(HEADERS)

 
