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

// Round 6 (Ruins) suites: one per synergy task, tests/suites/ruNN.hpp exposing suite_ruNN().
#if __has_include("ru01.hpp")
#include "ru01.hpp"
#define NU_HAS_SUITE_RU01 1
#endif

#if __has_include("ru02.hpp")
#include "ru02.hpp"
#define NU_HAS_SUITE_RU02 1
#endif

#if __has_include("ru03.hpp")
#include "ru03.hpp"
#define NU_HAS_SUITE_RU03 1
#endif

#if __has_include("ru04.hpp")
#include "ru04.hpp"
#define NU_HAS_SUITE_RU04 1
#endif

#if __has_include("ru05.hpp")
#include "ru05.hpp"
#define NU_HAS_SUITE_RU05 1
#endif

#if __has_include("ru06.hpp")
#include "ru06.hpp"
#define NU_HAS_SUITE_RU06 1
#endif

#if __has_include("ru07.hpp")
#include "ru07.hpp"
#define NU_HAS_SUITE_RU07 1
#endif

#if __has_include("ru08.hpp")
#include "ru08.hpp"
#define NU_HAS_SUITE_RU08 1
#endif

#if __has_include("ru09.hpp")
#include "ru09.hpp"
#define NU_HAS_SUITE_RU09 1
#endif

#if __has_include("ru10.hpp")
#include "ru10.hpp"
#define NU_HAS_SUITE_RU10 1
#endif

#if __has_include("ru11.hpp")
#include "ru11.hpp"
#define NU_HAS_SUITE_RU11 1
#endif

#if __has_include("ru12.hpp")
#include "ru12.hpp"
#define NU_HAS_SUITE_RU12 1
#endif

#if __has_include("ru13.hpp")
#include "ru13.hpp"
#define NU_HAS_SUITE_RU13 1
#endif

#if __has_include("ru14.hpp")
#include "ru14.hpp"
#define NU_HAS_SUITE_RU14 1
#endif

#if __has_include("ru15.hpp")
#include "ru15.hpp"
#define NU_HAS_SUITE_RU15 1
#endif

#if __has_include("ru16.hpp")
#include "ru16.hpp"
#define NU_HAS_SUITE_RU16 1
#endif

#if __has_include("ru17.hpp")
#include "ru17.hpp"
#define NU_HAS_SUITE_RU17 1
#endif

#if __has_include("ru18.hpp")
#include "ru18.hpp"
#define NU_HAS_SUITE_RU18 1
#endif

#if __has_include("ru19.hpp")
#include "ru19.hpp"
#define NU_HAS_SUITE_RU19 1
#endif

#if __has_include("ru20.hpp")
#include "ru20.hpp"
#define NU_HAS_SUITE_RU20 1
#endif

#if __has_include("ru21.hpp")
#include "ru21.hpp"
#define NU_HAS_SUITE_RU21 1
#endif

#if __has_include("ru22.hpp")
#include "ru22.hpp"
#define NU_HAS_SUITE_RU22 1
#endif

#if __has_include("ru23.hpp")
#include "ru23.hpp"
#define NU_HAS_SUITE_RU23 1
#endif

#if __has_include("ru24.hpp")
#include "ru24.hpp"
#define NU_HAS_SUITE_RU24 1
#endif

#if __has_include("ru25.hpp")
#include "ru25.hpp"
#define NU_HAS_SUITE_RU25 1
#endif

#if __has_include("ru26.hpp")
#include "ru26.hpp"
#define NU_HAS_SUITE_RU26 1
#endif

#if __has_include("ru27.hpp")
#include "ru27.hpp"
#define NU_HAS_SUITE_RU27 1
#endif

#if __has_include("ru28.hpp")
#include "ru28.hpp"
#define NU_HAS_SUITE_RU28 1
#endif

#if __has_include("ru29.hpp")
#include "ru29.hpp"
#define NU_HAS_SUITE_RU29 1
#endif

#if __has_include("ru30.hpp")
#include "ru30.hpp"
#define NU_HAS_SUITE_RU30 1
#endif

#if __has_include("ru31.hpp")
#include "ru31.hpp"
#define NU_HAS_SUITE_RU31 1
#endif

#if __has_include("ru32.hpp")
#include "ru32.hpp"
#define NU_HAS_SUITE_RU32 1
#endif

#if __has_include("ru33.hpp")
#include "ru33.hpp"
#define NU_HAS_SUITE_RU33 1
#endif

#if __has_include("ru34.hpp")
#include "ru34.hpp"
#define NU_HAS_SUITE_RU34 1
#endif

#if __has_include("ru35.hpp")
#include "ru35.hpp"
#define NU_HAS_SUITE_RU35 1
#endif

#if __has_include("ru36.hpp")
#include "ru36.hpp"
#define NU_HAS_SUITE_RU36 1
#endif

#if __has_include("ru37.hpp")
#include "ru37.hpp"
#define NU_HAS_SUITE_RU37 1
#endif

#if __has_include("ru38.hpp")
#include "ru38.hpp"
#define NU_HAS_SUITE_RU38 1
#endif

#if __has_include("ru39.hpp")
#include "ru39.hpp"
#define NU_HAS_SUITE_RU39 1
#endif

#if __has_include("ru40.hpp")
#include "ru40.hpp"
#define NU_HAS_SUITE_RU40 1
#endif

#if __has_include("ru41.hpp")
#include "ru41.hpp"
#define NU_HAS_SUITE_RU41 1
#endif

#if __has_include("ru42.hpp")
#include "ru42.hpp"
#define NU_HAS_SUITE_RU42 1
#endif

#if __has_include("ru43.hpp")
#include "ru43.hpp"
#define NU_HAS_SUITE_RU43 1
#endif

#if __has_include("ru44.hpp")
#include "ru44.hpp"
#define NU_HAS_SUITE_RU44 1
#endif

#if __has_include("ru45.hpp")
#include "ru45.hpp"
#define NU_HAS_SUITE_RU45 1
#endif

#if __has_include("ru46.hpp")
#include "ru46.hpp"
#define NU_HAS_SUITE_RU46 1
#endif

#if __has_include("ru47.hpp")
#include "ru47.hpp"
#define NU_HAS_SUITE_RU47 1
#endif

#if __has_include("ru48.hpp")
#include "ru48.hpp"
#define NU_HAS_SUITE_RU48 1
#endif

#if __has_include("ru49.hpp")
#include "ru49.hpp"
#define NU_HAS_SUITE_RU49 1
#endif

#if __has_include("ru50.hpp")
#include "ru50.hpp"
#define NU_HAS_SUITE_RU50 1
#endif

#if __has_include("ru51.hpp")
#include "ru51.hpp"
#define NU_HAS_SUITE_RU51 1
#endif

#if __has_include("ru52.hpp")
#include "ru52.hpp"
#define NU_HAS_SUITE_RU52 1
#endif

#if __has_include("ru53.hpp")
#include "ru53.hpp"
#define NU_HAS_SUITE_RU53 1
#endif

#if __has_include("ru54.hpp")
#include "ru54.hpp"
#define NU_HAS_SUITE_RU54 1
#endif

#if __has_include("ru55.hpp")
#include "ru55.hpp"
#define NU_HAS_SUITE_RU55 1
#endif

#if __has_include("ru56.hpp")
#include "ru56.hpp"
#define NU_HAS_SUITE_RU56 1
#endif

#if __has_include("ru57.hpp")
#include "ru57.hpp"
#define NU_HAS_SUITE_RU57 1
#endif

#if __has_include("ru58.hpp")
#include "ru58.hpp"
#define NU_HAS_SUITE_RU58 1
#endif

#if __has_include("ru59.hpp")
#include "ru59.hpp"
#define NU_HAS_SUITE_RU59 1
#endif

#if __has_include("ru60.hpp")
#include "ru60.hpp"
#define NU_HAS_SUITE_RU60 1
#endif

#if __has_include("ru61.hpp")
#include "ru61.hpp"
#define NU_HAS_SUITE_RU61 1
#endif

#if __has_include("ru62.hpp")
#include "ru62.hpp"
#define NU_HAS_SUITE_RU62 1
#endif

#if __has_include("ru63.hpp")
#include "ru63.hpp"
#define NU_HAS_SUITE_RU63 1
#endif

#if __has_include("ru64.hpp")
#include "ru64.hpp"
#define NU_HAS_SUITE_RU64 1
#endif

#if __has_include("ru65.hpp")
#include "ru65.hpp"
#define NU_HAS_SUITE_RU65 1
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
#ifdef NU_HAS_SUITE_END001
    suite_end001();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_TOX007
    suite_tox007();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_ACT008
    suite_act008();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU01
    suite_ru01();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU02
    suite_ru02();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU03
    suite_ru03();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU04
    suite_ru04();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU05
    suite_ru05();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU06
    suite_ru06();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU07
    suite_ru07();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU08
    suite_ru08();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU09
    suite_ru09();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU10
    suite_ru10();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU11
    suite_ru11();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU12
    suite_ru12();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU13
    suite_ru13();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU14
    suite_ru14();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU15
    suite_ru15();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU16
    suite_ru16();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU17
    suite_ru17();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU18
    suite_ru18();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU19
    suite_ru19();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU20
    suite_ru20();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU21
    suite_ru21();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU22
    suite_ru22();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU23
    suite_ru23();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU24
    suite_ru24();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU25
    suite_ru25();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU26
    suite_ru26();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU27
    suite_ru27();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU28
    suite_ru28();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU29
    suite_ru29();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU30
    suite_ru30();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU31
    suite_ru31();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU32
    suite_ru32();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU33
    suite_ru33();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU34
    suite_ru34();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU35
    suite_ru35();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU36
    suite_ru36();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU37
    suite_ru37();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU38
    suite_ru38();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU39
    suite_ru39();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU40
    suite_ru40();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU41
    suite_ru41();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU42
    suite_ru42();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU43
    suite_ru43();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU44
    suite_ru44();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU45
    suite_ru45();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU46
    suite_ru46();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU47
    suite_ru47();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU48
    suite_ru48();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU49
    suite_ru49();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU50
    suite_ru50();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU51
    suite_ru51();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU52
    suite_ru52();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU53
    suite_ru53();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU54
    suite_ru54();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU55
    suite_ru55();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU56
    suite_ru56();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU57
    suite_ru57();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU58
    suite_ru58();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU59
    suite_ru59();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU60
    suite_ru60();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU61
    suite_ru61();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU62
    suite_ru62();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU63
    suite_ru63();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU64
    suite_ru64();
    ++ran;
#endif
#ifdef NU_HAS_SUITE_RU65
    suite_ru65();
    ++ran;
#endif
    return ran;
}
