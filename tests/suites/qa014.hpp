#pragma once

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "core/CrashHandler.hpp"

inline void suite_qa014()
{
    using namespace nundertale;

    std::tm local{};
    local.tm_year = 126; // 2026
    local.tm_mon = 0;
    local.tm_mday = 2;
    local.tm_hour = 3;
    local.tm_min = 4;
    local.tm_sec = 5;
    local.tm_isdst = -1;
    const std::time_t fixedTime = std::mktime(&local);
    assert(fixedTime != static_cast<std::time_t>(-1));
    assert(crashReportFileName(fixedTime) == "crash_2026-01-02_03-04-05.txt");

    const std::vector<std::string> stack = { "0x00000001", "0x00000002" };
    const std::string report = formatCrashReport(
        "test failure", "OverworldScene", "ruins_r3", stack, "2026-01-02 03:04:05");
    assert(report.find("Reason: test failure") != std::string::npos);
    assert(report.find("Scene: OverworldScene") != std::string::npos);
    assert(report.find("Room: ruins_r3") != std::string::npos);
    assert(report.find("0x00000001") != std::string::npos);
    assert(report.find("0x00000002") != std::string::npos);

    setCrashContext("BattleScene", "ruins_barky");
    const CrashContext context = getCrashContext();
    assert(context.scene == "BattleScene");
    assert(context.room == "ruins_barky");

    const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path tempRoot =
        std::filesystem::temp_directory_path() / ("nundertale_qa014_" + std::to_string(unique));
    std::error_code error;
    std::filesystem::remove_all(tempRoot, error);

    const std::filesystem::path reportDirectory = tempRoot / "nested" / "crash_reports";
    const std::string written = writeCrashReport(
        reportDirectory, "unit-test crash", "MenuScene", "title", stack, fixedTime);
    assert(!written.empty());
    const std::filesystem::path writtenPath(written);
    assert(writtenPath.parent_path() == reportDirectory);
    assert(writtenPath.filename() == crashReportFileName(fixedTime));
    assert(std::filesystem::is_regular_file(writtenPath));
    std::ifstream input(writtenPath);
    std::ostringstream contents;
    contents << input.rdbuf();
    assert(contents.str().find("Reason: unit-test crash") != std::string::npos);
    input.close();

    // A regular file cannot also be created/used as a report directory.
    const std::filesystem::path notADirectory = tempRoot / "regular_file";
    {
        std::ofstream marker(notADirectory);
        marker << "occupied";
    }
    bool threw = false;
    std::string failedWrite;
    try {
        failedWrite = writeCrashReport(
            notADirectory, "must fail", "Scene", "room", stack, fixedTime);
    } catch (...) {
        threw = true;
    }
    assert(!threw);
    assert(failedWrite.empty());

    std::filesystem::remove_all(tempRoot, error);
    assert(!error);

    std::cout << "[PASS] QA-014: crash context, deterministic report formatting and safe file writing verified."
              << std::endl;
}
