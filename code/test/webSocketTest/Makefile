# Makefile for WebSocket++ Server

# 编译器
CXX = g++

# 编译器选项
CXXFLAGS = -std=c++11 -I/usr/local/include -I/usr/include -I/home/admin_ubuntu/websocketpp

# 链接选项
LDFLAGS = -L/usr/local/lib -L/usr/lib -lboost_system

# 目标文件
TARGET = websocket_server

# 源文件
SRCS = main.cpp

# 目标
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean

