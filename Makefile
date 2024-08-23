# 顶层Makefile

# 编译器设置
CC = g++
CFLAGS = -Iinclude -Wall -g -std=c++20 -fdiagnostics-color=always 

# 子目录
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# 日志文件
ERROR_LOG = build/error.log

# 源文件和对象文件
SRCS =  $(wildcard $(SRC_DIR)/**/**/*.cpp) $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/buildTest

# 默认目标
all: cleanlog $(TARGET)

cleanlog:
	@echo "Cleaning up previous logs..."
	@rm -f $(ERROR_LOG)

# 链接
$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) 2>> $(ERROR_LOG)

# 编译规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 2>> $(ERROR_LOG)

# 清理
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# 打开日志
see : 
	vim build/error.log

.PHONY: all clean

