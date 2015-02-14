#ifndef STATIC_BUFFER_HPP
#define STATIC_BUFFER_HPP

#include <cstdint>
#include <cstdlib>
#include <new>
#include <cassert>
#include <utility>

namespace GAME {

/**
 * This class has a very specific use. It's for when you want to construct an
 * array piece by piece by constructing individual elements.
 *
 * There is basically one element construction function, called emplace_at.
 * It takes an index and constructor parameters to use. It'll then construct
 * the element in place.
 *
 * You cannot copy a StaticBuffer, but you can move one. To move one, it's
 * assumed that all the elements of the underlying array CAN BE MOVED. So even
 * if you declare a static buffer of 10 T elements, but you only initialize 7
 * of them, moving the static buffer will attempt to move all of the 10 T
 * elements.
 */
template<typename T, size_t N>
class StaticBuffer {
public:
  StaticBuffer()
  {}

  StaticBuffer(const StaticBuffer&) = delete;
  StaticBuffer& operator=(const StaticBuffer&) = delete;

  StaticBuffer(StaticBuffer&& src) {
    *this = std::move(src);
  }

  StaticBuffer& operator=(StaticBuffer&& src) {
    if (&src != this) {
      T *p_this, *p_src;
      p_this = ptr_data();
      p_src = src.ptr_data();
      for (size_t i = 0; i < N; i++) {
        *p_this = std::move(*p_src);
        p_this++;
        p_src++;
      }
    }
    return *this;
  }

  template<typename ...Ts>
  void
  emplace_at(size_t at, Ts ...args) {
    new (ptr_data() + at) T(std::forward<Ts...>(args...));
  }

  T*
  ptr_data() {
    return reinterpret_cast<T*>(store);
  }

  typedef T (*array_ptr)[N];

  array_ptr
  array_data() {
    return reinterpret_cast<array_ptr>(store);
  }

  ~StaticBuffer() {
    T* p = ptr_data();
    for (size_t i = 0; i < N; i++) {
      p[i].~T();
    }
  }

private:
  uint8_t store[sizeof (T) * N];
};

}

#endif
