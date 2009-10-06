
GLOOX_CFLAGS=`pkg-config --cflags gloox`
GLOOX_LIBS=`pkg-config --libs gloox`

CXXFLAGS += $(GLOOX_CFLAGS)
LIBS += $(GLOOX_LIBS)

HEADERS = client.h logger.h parser.h
SOURCES = client.cpp logger.cpp
OBJECTS = client.o logger.o main.o parser.o

client: $(HEADERS) $(OBJECTS)
	$(CXX) -o client $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

