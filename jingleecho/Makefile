
GLOOX_CFLAGS=`pkg-config --cflags gloox`
GLOOX_LIBS=`pkg-config --libs gloox`

CXXFLAGS += $(GLOOX_CFLAGS)
LIBS += $(GLOOX_LIBS)

HEADERS = client.h logger.h
SOURCES = client.cpp logger.cpp
OBJECTS = client.o logger.o

client: $(HEADERS) $(OBJECTS)
	$(CXX) -o client $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

