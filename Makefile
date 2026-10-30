CXX = g++
CXXFLAGS = -std=c++17 -O2
TARGET = main.exe
SRCS = main.cpp lexer.cpp eval.cpp parser.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJS) $(TARGET)