#pragma once

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_0(x) DEFER_2(x, __COUNTER__)
#define DEFER(f) Defer DEFER_0(__deferred)([&]() { f; }) // need C++17 template deduction guide

template <typename F>
struct Defer {
  F f;
  Defer(F f) : f(f) {}
  ~Defer() { f(); }
};