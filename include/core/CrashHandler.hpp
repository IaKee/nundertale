#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
// Declared directly instead of including windows.h, whose GDI/user symbols conflict with
// raylib's Rectangle, CloseWindow and ShowCursor declarations.
extern "C" __declspec(dllimport) unsigned short __stdcall CaptureStackBackTrace(
    unsigned long framesToSkip, unsigned long framesToCapture, void** backTrace,
    unsigned long* backTraceHash);
#endif

namespace nundertale {

struct CrashContext {
    std::string scene;
    std::string room;
};

namespace crash_detail {

inline constexpr std::size_t CONTEXT_CAPACITY = 128;
inline std::array<char, CONTEXT_CAPACITY> sceneContext{};
inline std::array<char, CONTEXT_CAPACITY> roomContext{};
inline std::array<char, 512> reportDirectory{ '.', '\0' };
inline std::atomic_flag handlingCrash = ATOMIC_FLAG_INIT;

inline void copyFixed(std::string_view source, char* destination, std::size_t capacity) noexcept
{
    if (capacity == 0) return;
    const std::size_t count = std::min(source.size(), capacity - 1);
    std::copy_n(source.data(), count, destination);
    destination[count] = '\0';
}

inline std::tm localTime(std::time_t value) noexcept
{
    std::tm result{};
#ifdef _WIN32
    (void)localtime_s(&result, &value);
#else
    (void)localtime_r(&value, &result);
#endif
    return result;
}

inline std::string timeText(std::time_t value)
{
    const std::tm local = localTime(value);
    std::ostringstream stream;
    stream << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

inline std::vector<std::string> captureStack()
{
#ifdef _WIN32
    std::array<void*, 62> frames{};
    const unsigned short count = CaptureStackBackTrace(
        0, static_cast<unsigned long>(frames.size()), frames.data(), nullptr);
    std::vector<std::string> result;
    result.reserve(count);
    for (unsigned short index = 0; index < count; ++index) {
        std::ostringstream line;
        line << frames[index];
        result.push_back(line.str());
    }
    return result;
#else
    return { "stack unavailable" };
#endif
}

inline const char* signalName(int signalNumber) noexcept
{
    switch (signalNumber) {
    case SIGSEGV: return "SIGSEGV";
    case SIGABRT: return "SIGABRT";
    case SIGFPE: return "SIGFPE";
    case SIGILL: return "SIGILL";
    default: return "unknown signal";
    }
}

} // namespace crash_detail

// The context deliberately uses fixed buffers: setters allocate nothing and crash-time reads
// remain simple. A signal arriving during a setter can observe a truncated/partially updated
// value; this best-effort diagnostic trade-off avoids locks, which could deadlock after a crash.
inline void setCrashContext(std::string_view sceneName, std::string_view roomId) noexcept
{
    crash_detail::copyFixed(sceneName, crash_detail::sceneContext.data(),
                            crash_detail::sceneContext.size());
    crash_detail::copyFixed(roomId, crash_detail::roomContext.data(),
                            crash_detail::roomContext.size());
}

inline CrashContext getCrashContext()
{
    return { crash_detail::sceneContext.data(), crash_detail::roomContext.data() };
}

inline std::string crashReportFileName(std::time_t timeValue)
{
    const std::tm local = crash_detail::localTime(timeValue);
    std::ostringstream stream;
    stream << "crash_" << std::put_time(&local, "%Y-%m-%d_%H-%M-%S") << ".txt";
    return stream.str();
}

inline std::string formatCrashReport(std::string_view reason, std::string_view scene,
                                     std::string_view room,
                                     const std::vector<std::string>& stackLines,
                                     std::string_view timeText)
{
    std::ostringstream report;
    report << "nUndertale crash report\n"
           << "Time: " << timeText << '\n'
           << "Reason: " << reason << '\n'
           << "Scene: " << scene << '\n'
           << "Room: " << room << '\n'
           << "Stack:\n";
    if (stackLines.empty()) {
        report << "stack unavailable\n";
    } else {
        for (const std::string& line : stackLines) report << line << '\n';
    }
    return report.str();
}

inline std::string writeCrashReport(const std::filesystem::path& directory,
                                    std::string_view reason, std::string_view scene,
                                    std::string_view room,
                                    const std::vector<std::string>& stackLines,
                                    std::time_t timeValue) noexcept
{
    try {
        std::error_code error;
        std::filesystem::create_directories(directory, error);
        if (error || !std::filesystem::is_directory(directory, error) || error) return {};

        const std::filesystem::path path = directory / crashReportFileName(timeValue);
        std::ofstream output(path, std::ios::out | std::ios::trunc);
        if (!output) return {};
        output << formatCrashReport(reason, scene, room, stackLines,
                                    crash_detail::timeText(timeValue));
        output.close();
        if (!output) return {};
        return path.string();
    } catch (...) {
        return {};
    }
}

namespace crash_detail {

inline void writeCurrentReport(std::string_view reason) noexcept
{
    if (handlingCrash.test_and_set()) return;
    try {
        const CrashContext context = getCrashContext();
        const std::time_t now = std::time(nullptr);
        (void)writeCrashReport(reportDirectory.data(), reason, context.scene, context.room,
                               captureStack(), now);
    } catch (...) {
        // A damaged process may be unable to allocate even the best-effort report.
    }
}

inline void signalHandler(int signalNumber) noexcept
{
    writeCurrentReport(signalName(signalNumber));
    std::signal(signalNumber, SIG_DFL);
    std::raise(signalNumber);
}

[[noreturn]] inline void terminateHandler() noexcept
{
    std::string reason = "uncaught C++ exception";
    if (const std::exception_ptr exception = std::current_exception()) {
        try {
            std::rethrow_exception(exception);
        } catch (const std::exception& caught) {
            reason += ": ";
            reason += caught.what();
        } catch (...) {
            reason += ": non-standard exception";
        }
    }
    writeCurrentReport(reason);
    std::signal(SIGABRT, SIG_DFL);
    std::raise(SIGABRT);
    std::_Exit(134);
}

} // namespace crash_detail

// Reporting from a fatal signal necessarily uses facilities (filesystem, allocation and stack
// capture) that are not strictly async-signal-safe. It is best-effort and immediately restores
// and re-raises the signal so the operating system still observes the original fatal signal.
inline void installCrashHandler(const std::filesystem::path& directory = "crash_reports") noexcept
{
    try {
        crash_detail::copyFixed(directory.string(), crash_detail::reportDirectory.data(),
                                crash_detail::reportDirectory.size());
    } catch (...) {
        crash_detail::copyFixed("crash_reports", crash_detail::reportDirectory.data(),
                                crash_detail::reportDirectory.size());
    }
    std::set_terminate(crash_detail::terminateHandler);
    std::signal(SIGSEGV, crash_detail::signalHandler);
    std::signal(SIGABRT, crash_detail::signalHandler);
    std::signal(SIGFPE, crash_detail::signalHandler);
    std::signal(SIGILL, crash_detail::signalHandler);
}

} // namespace nundertale
