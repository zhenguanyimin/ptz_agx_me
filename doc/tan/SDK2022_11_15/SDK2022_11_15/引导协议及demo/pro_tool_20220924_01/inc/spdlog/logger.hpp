#pragma once
#define SPDLOG_ACTIVE_LEVEL 0
#include <spdlog/spdlog.h>  /// Basic usage
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h> /// Basic file logger
#include <spdlog/sinks/daily_file_sink.h> /// Daily files
#include <spdlog/fmt/bin_to_hex.h> /// Log binary data in hex
#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>

#ifdef _DEBUG
#ifndef _WIN64
#pragma comment(lib, "spdlog/lib/spdlogd.lib")
#else
#pragma comment(lib, "spdlog/lib/v142_x64/spdlogd.lib")
#endif
#else
#ifndef _WIN64
#pragma comment(lib, "spdlog/lib/spdlog.lib")
#else
#pragma comment(lib, "spdlog/lib/v142_x64/spdlog.lib")
#endif
#endif

#define SPDLOG_HELPER_NAMESPACE_BEGIN namespace SPDLOG_HELPER {
#define SPDLOG_HELPER_NAMESPACE_END }
#define SPDLOG_HELPER_NS SPDLOG_HELPER

SPDLOG_HELPER_NAMESPACE_BEGIN
enum {
    SPDLOG_SINK_STDOUT = 0x01,
    SPDLOG_SINK_DAILY_FILE = 0x02,
    SPDLOG_SINK_MSVC = 0x04,
};


static inline void construct_log_sinks(std::vector<spdlog::sink_ptr>& vecSinks, uint32_t sinks, std::string daily_file_name)
{
    if (sinks & SPDLOG_SINK_STDOUT)
    {
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
        if (stdout_sink)
        {
            stdout_sink->set_level(spdlog::level::trace);
            vecSinks.emplace_back(stdout_sink);
        }
    }
    if (sinks & SPDLOG_SINK_DAILY_FILE)
    {
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(daily_file_name, 0, 1, false, 30);
        if (daily_sink)
        {
            daily_sink->set_level(spdlog::level::info);
            vecSinks.emplace_back(daily_sink);
        }
    }
    if (sinks & SPDLOG_SINK_MSVC)
    {
        auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        if (msvc_sink)
        {
            msvc_sink->set_level(spdlog::level::trace);
            vecSinks.emplace_back(msvc_sink);
        }
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="proj_name"></param>
/// <param name="sinks">0x01-stdout_color_sink_mt 0x02-daily_file_sink_mt 0x04-msvc_sink_mt</param>
/// <param name="daily_file_name"></param>
/// <param name="pattern"></param>
static inline void spdlog_load_default(std::string proj_name, uint32_t sinks, std::string daily_file_name, bool sync = true, std::string pattern= "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v [%@][tid %t][pid %P]")
{
    try
    {
        std::vector<spdlog::sink_ptr> vecSinks;
        construct_log_sinks(vecSinks, sinks, daily_file_name);
        if (vecSinks.empty())
        {
            return;
        }
        if (!spdlog::thread_pool())
        {
            spdlog::init_thread_pool(8192, 1);
        }
        std::shared_ptr<spdlog::logger> logger;
        if (sync)
        {
            logger = std::make_shared<spdlog::logger>(proj_name.data(), vecSinks.begin(), vecSinks.end());
        }
        else {
            logger = std::make_shared<spdlog::async_logger>(proj_name.data(), vecSinks.begin(), vecSinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        if (!logger)
        {
            return;
        }
        if (pattern.length() > 0)
        {
            logger->set_pattern(pattern);
        }
        spdlog::register_logger(logger);
        spdlog::set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);
        spdlog::flush_every(std::chrono::seconds(10));
    }
    catch (const std::exception&)
    {
        //OutputDebugString("error");
    }
    catch (...)
    {
        //OutputDebugString("error");
    }
}

class loggerwrap
{
protected:
    loggerwrap() =default;
    ~loggerwrap() = default;
public:
    static loggerwrap& get()
    {
        static loggerwrap ins;
        return ins;
    }
    void setIns(std::shared_ptr<spdlog::logger> ptr, std::shared_ptr<spdlog::details::thread_pool> threadpool) {
        logger = ptr;
        logger_threadpool = threadpool;
    }
    std::shared_ptr<spdlog::logger>& getlogger() { 
        if (!logger) {
            return (std::shared_ptr<spdlog::logger>&)spdlog::default_logger();
        }
        return logger; 
    }
    bool isconstruct() { return logger ? true : false; }
private:
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::details::thread_pool> logger_threadpool;
};

static inline void spdlog_construct(std::string proj_name, uint32_t sinks, std::string daily_file_name, bool sync = true, std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v [%@][tid %t][pid %P]")
{
    if (loggerwrap::get().isconstruct())
    {
        return;
    }
    try
    {
        std::vector<spdlog::sink_ptr> vecSinks;
        construct_log_sinks(vecSinks, sinks, daily_file_name);
        if (vecSinks.empty())
        {
            return;
        }

        std::shared_ptr<spdlog::logger> logger;
        std::shared_ptr<spdlog::details::thread_pool> threadpool;
        if (sync)
        {
            logger = std::make_shared<spdlog::logger>(proj_name.data(), vecSinks.begin(), vecSinks.end());
        }
        else {
            threadpool = std::make_shared<spdlog::details::thread_pool>(8192, 1);
            if (!threadpool)
            {
                //OutputDebugString("error");
                return;
            }
            logger = std::make_shared<spdlog::async_logger>(proj_name.data(), vecSinks.begin(), vecSinks.end(), threadpool, spdlog::async_overflow_policy::block);

        }
        if (!logger)
        {
            //OutputDebugString("error");
            return;
        }
        if (pattern.length() > 0)
        {
            logger->set_pattern(pattern);
        }
        spdlog::register_logger(logger);
        logger->set_level(spdlog::level::trace);
        loggerwrap::get().setIns(logger, threadpool);
        /// Ìí¼ÓË¢ÐÂ
    }
    catch (const std::exception&)
    {
        //OutputDebugString("error");
    }
    catch (...)
    {
        //OutputDebugString("error");
    }
}

#ifdef LOG_TRACE
#undef LOG_TRACE
#endif
#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#ifdef LOG_INFO
#undef LOG_INFO
#endif
#ifdef LOG_WARN
#undef LOG_WARN
#endif
#ifdef LOG_CRITICAL
#undef LOG_CRITICAL
#endif
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#define LOG_TRACE(...) SPDLOG_LOGGER_TRACE(SPDLOG_HELPER_NS::loggerwrap::get().getlogger().get(), __VA_ARGS__)
#else
#define LOG_TRACE(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(SPDLOG_HELPER_NS::loggerwrap::get().getlogger().get(), __VA_ARGS__)
#else
#define LOG_DEBUG(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
#define LOG_INFO(...) SPDLOG_LOGGER_INFO(SPDLOG_HELPER_NS::loggerwrap::get().getlogger().get(), __VA_ARGS__)
#else
#define LOG_INFO(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#define LOG_WARN(...) SPDLOG_LOGGER_WARN(SPDLOG_HELPER_NS::loggerwrap::get().getlogger().get(), __VA_ARGS__)
#else
#define LOG_WARN(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(SPDLOG_HELPER_NS::loggerwrap::get().getlogger().get(), __VA_ARGS__)
#else
#define LOG_ERROR(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
#define LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(SPDLOG_HELPER_NS::loggerwrap::get().getlogger().get(), __VA_ARGS__)
#else
#define LOG_CRITICAL(...) (void)0
#endif

static inline void spdlog_clear()
{
    try
    {
        spdlog::shutdown();
    }
    catch (const std::exception&)
    {

    }
    catch (...)
    {

    }
}

static inline void spdlog_flush()
{
    try
    {
        if (SPDLOG_HELPER_NS::loggerwrap::get().getlogger())
            SPDLOG_HELPER_NS::loggerwrap::get().getlogger()->flush();
    }
    catch (const std::exception&)
    {

    }
    catch (...)
    {

    }
}
SPDLOG_HELPER_NAMESPACE_END