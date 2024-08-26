#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

enum class LogLevel
{
    _DEBUG,
    INFO,
    WARNING,
    ERROR
};
class LogFormatter
{
public:
    static std::string format(LogLevel level, const std::string &message, const std::string &file, int line)
    {
        std::ostringstream oss;
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm *local_tm = std::localtime(&now_c);

        // 格式化时间
        oss << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S");

        // 日志级别
        oss << " [" << logLevelToString(level) << "] ";

        // 文件名和行号
        oss << file << ":" << line << "\n";

        // 日志消息
        oss << message;

        return oss.str();
    }

private:
    static std::string logLevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::_DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }
};

class Logger
{
public:
    static Logger &getInstance()
    {
        static Logger instance;
        return instance;
    }

    template <typename T>
    void log(LogLevel level, const T &objORmessage, const std::string &file, int line)
    {
        logImpl(level, objORmessage, file, line);
    }
    template <typename T>
    void debug_log(LogLevel level, const T &objORmessage, const std::string &file, int line)
    {
        dlogImpl(level, objORmessage, file, line);
    }



    void setLogFile(const std::string &filename)
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        if (fileStream.is_open())
        {
            fileStream.close();
        }
        fileStream.open(filename, std::ios::app);
    }

    void start()
    {
        running = true;
        logThread = std::thread(&Logger::processQueue, this);
    }

    void stop()
    {
        running = false;
        cv.notify_one();
        if (logThread.joinable())
        {
            logThread.join();
        }
    }

private:
    Logger() : running(false) {}
    ~Logger()
    {
        stop();
        if (fileStream.is_open())
        {
            fileStream.close();
        }
    }

    void processQueue()
    {
        while (running || !logQueue.empty())
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this]()
                    { return !logQueue.empty() || !running; });

            while (!logQueue.empty())
            {
                std::string message = logQueue.front();
                logQueue.pop();
                lock.unlock(); // 解锁队列

                // 写入日志
                std::lock_guard<std::mutex> fileLock(fileMutex);
                if (fileStream.is_open())
                {
                    fileStream << message << "\n";
                }
                std::cout << message << std::endl; // 同时输出到控制台

                lock.lock(); // 重新加锁队列
            }
        }
    }
    template <typename T>
    std::enable_if_t<std::is_same_v<decltype(std::declval<const T &>().log()), std::string>, void>
    logImpl(LogLevel level, const T &obj, const std::string &file, int line)
    {
        log(level, obj.log(), file, line);
    }

    template <typename T>
    std::enable_if_t<!std::is_same_v<decltype(std::declval<const T &>().log()), std::string>, void>
    logImpl(LogLevel level, const T &message, const std::string &file, int line)
    {
        log(level, std::string("No log method for the object"), file, line);
    }
    template <typename T>
    std::enable_if_t<std::is_same_v<decltype(std::declval<const T &>().log()), std::string>, void>
    dlogImpl(LogLevel level, const T &obj, const std::string &file, int line)
    {
        debug_log(level, obj.log(), file, line);
    }

    template <typename T>
    std::enable_if_t<!std::is_same_v<decltype(std::declval<const T &>().log()), std::string>, void>
    dlogImpl(LogLevel level, const T &message, const std::string &file, int line)
    {
        debug_log(level, std::string("No log method for the object"), file, line);
    }
    std::atomic<bool> running;
    std::thread logThread;
    std::mutex queueMutex;
    std::mutex fileMutex;
    std::condition_variable cv;
    std::queue<std::string> logQueue;
    std::ofstream fileStream;
};

template <>
inline void Logger::log(LogLevel level, const std::string &message, const std::string &file, int line)
{
    std::string formattedMessage = LogFormatter::format(level, message, file, line);
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push(formattedMessage);
    }
    cv.notify_one(); // 通知日志线程有新消息需要处理
}
template <>
inline void Logger::debug_log(LogLevel level, const std::string &message, const std::string &file, int line)
{
    std::lock_guard<std::mutex> lock(fileMutex);

    std::string formattedMessage = LogFormatter::format(level, message, file, line);
    std::cout <<  formattedMessage << std::endl;

    if (fileStream.is_open())
    {
        fileStream << formattedMessage << std::endl;
    }
}

// 如果定义了 DEBUG 宏，则启用 LOG_DEBUG 宏
#ifdef DEBUG
#define LOG_DEBUG(message) Logger::getInstance().debug_log(LogLevel::_DEBUG, message, __FILE__, __LINE__)
#else
#define LOG_DEBUG(message) // 空定义，什么也不做
#endif
#define LOG_INFO(message) Logger::getInstance().log(LogLevel::INFO, message, __FILE__, __LINE__)
#define LOG_WARNING(message) Logger::getInstance().debug_log(LogLevel::WARNING, message, __FILE__, __LINE__)
#define LOG_ERROR(message) Logger::getInstance().log(LogLevel::ERROR, message, __FILE__, __LINE__)

#endif