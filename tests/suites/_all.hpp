#pragma once

// Plug-in point for extra unit-test suites, so people can add suites in parallel
// without editing tests/test_runner.cpp.
//
// Convention: one header per backlog ID next to this file (e.g. tests/suites/tox001.hpp)
// exposing `inline void suite_<id>()`. A suite asserts and prints its own [PASS] lines
// and aborts/throws on failure, like the inline suites in test_runner.cpp.
//
// To add a new name: append a `__has_include` block below and a matching
// `#ifdef NU_HAS_SUITE_<ID>` call in runExtraSuites(). Headers that do not exist are skipped.
// This file is included after every production header, so suites can use them directly.

#if __has_include("tox001.hpp")
#include "tox001.hpp"
#define NU_HAS_SUITE_TOX001 1
#endif

#if __has_include("rad004.hpp")
#include "rad004.hpp"
#define NU_HAS_SUITE_RAD004 1
#endif

#if __has_include("enc004.hpp")
#include "enc004.hpp"
#define NU_HAS_SUITE_ENC004 1
#endif

#if __has_include("sav007.hpp")
#include "sav007.hpp"
#define NU_HAS_SUITE_SAV007 1
#endif

#if __has_include("qa014.hpp")
#include "qa014.hpp"
#define NU_HAS_SUITE_QA014 1
#endif

#if __has_include("end001.hpp")
#include "end001.hpp"
#define NU_HAS_SUITE_END001 1
#endif

#if __has_include("met003.hpp")
#include "met003.hpp"
#define NU_HAS_SUITE_MET003 1
#endif

#if __has_include("shp008.hpp")
#include "shp008.hpp"
#define NU_HAS_SUITE_SHP008 1
#endif

#if __has_include("tox007.hpp")
#include "tox007.hpp"
#define NU_HAS_SUITE_TOX007 1
#endif

#if __has_include("act008.hpp")
#include "act008.hpp"
#define NU_HAS_SUITE_ACT008 1
#endif

// Runs every suite whose header exists; returns how many ran.
inline int runExtraSuites()
{
    int ran = 0;
#ifdef NU_HAS_SUITE_TOX001
    suite_tox001();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RAD004
    suite_rad004();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_ENC004
    suite_enc004();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_SAV007
    suite_sav007();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_QA014
    suite_qa014();
    ++ran;
#endif
    return ran;
}
