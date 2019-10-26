CXXFLAGS = -Ibase -pthread -O0 -g
TARGET = test
SRCS = $(wildcard *.cc base/*.cc) #wildcard：获取匹配模式的文件名
OBJS = $(patsubst %.cc, %.o, $(SRCS)) #patsubst：模式替换
# 如果目录很多的话可以这样：
# DIRS = dir1 dir2 dir3 dir4
# SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
# 上面可以获取四个目录下所有的.cc文件
# $(foreach VAR, LIST, EXPRESSION) ：该函数的作用是将LIST中每个取出赋值给VAR，然后执行EXPRESSION

$(TARGET): $(OBJS)
	g++ -o $@ $^ $(CXXFLAGS)

#这里使用了隐含规则: 对于一个目标为.o的文件，自动推导其依赖文件为同名的源文件，并且命令也自动执行，比如g++ -c file.cc $(CXXFLAGS)

clean:
	$(RM) $(TARGET) $(OBJS) 
