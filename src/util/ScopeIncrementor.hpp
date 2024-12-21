#pragma once

template <typename T>
struct ScopeIncrement {
  T &value;
  ~ScopeIncrement() { ++value; }
};
