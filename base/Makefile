CXXFLAGS = -pthread -g
TARGET = log_test
SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc, %.o, $(SRCS))


$(TARGET): $(OBJS)
	g++ -o $@ $^ $(CXXFLAGS)



clean:
	$(RM) $(TARGET) $(OBJS)
