#ifndef MOptional_H
#define MOptional_H
#include <stdexcept>
#include <utility> // For std::move

template <typename T>
class mOptional {
public:
    // 默认构造函数，表示没有值
    mOptional() : has_value_(false) {}
    // 用于传入true构造不含具体对象的mOptional
    mOptional(bool has_value) : has_value_(has_value) {}
    // 构造函数，使用一个值初始化
    mOptional(const T& value) : has_value_(true), value_(value) {}

    mOptional(T&& value) : has_value_(true), value_(std::move(value)) {}

    // 拷贝构造函数
    mOptional(const mOptional& other) : has_value_(other.has_value_) {
        if (has_value_) {
            value_ = other.value_;
        }
    }

    // 移动构造函数
    mOptional(mOptional&& other) noexcept : has_value_(other.has_value_) {
        if (has_value_) {
            value_ = std::move(other.value_);
        }
    }

    // 拷贝赋值运算符
    mOptional& operator=(const mOptional& other) {
        if (this != &other) {
            has_value_ = other.has_value_;
            if (has_value_) {
                value_ = other.value_;
            }
        }
        return *this;
    }

    // 移动赋值运算符
    mOptional& operator=(mOptional&& other) noexcept {
        if (this != &other) {
            has_value_ = other.has_value_;
            if (has_value_) {
                value_ = std::move(other.value_);
            }
        }
        return *this;
    }

    // 返回是否有值
    bool has_value() const {
        return has_value_;
    }

    // 返回值，如果没有值则抛出异常
    T& value() {
        if (!has_value_) {
            throw std::runtime_error("No value present");
        }
        return value_;
    }

    const T& value() const {
        if (!has_value_) {
            throw std::runtime_error("No value present");
        }
        return value_;
    }

    // 重置为无值状态
    void reset() {
        has_value_ = false;
    }

    // 布尔转换操作符
    explicit operator bool() const {
        return has_value_;
    }
private:
    bool has_value_;
    T value_;
};


#endif