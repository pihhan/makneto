
GLOOX_CFLAGS=`pkg-config --cflags gloox`
GLOOX_LIBS=`pkg-config --libs gloox`

CXXFLAGS += $(GLOOX_CFLAGS)
CXXFLAGS += -Wall -g 
CXXFLAGS += -Iipv6
LIBS += $(GLOOX_LIBS)

HEADERS = client.h logger.h parser.h requests.h versionhandler.h \
    jinglesession.h jinglemanager.h ipv6/v6interface.h
SOURCES = client.cpp logger.cpp
OBJECTS = client.o logger.o main.o parser.o requests.o versionhandler.o \
    jinglesession.o jinglemanager.o ipv6/v6interface.o

client: $(HEADERS) $(OBJECTS)
	$(CXX) -o client $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o client
   
tags: $(SOURCES) $(HEADERS)
	ctags $(SOURCES) $(HEADERS)

 
