# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -I/usr/include -I/usr/local/include
LDFLAGS = -L/usr/lib -L/usr/local/lib -lboost_system -lboost_thread

# 目标文件
TARGET = client
SRC = client.cpp

# 规则
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

