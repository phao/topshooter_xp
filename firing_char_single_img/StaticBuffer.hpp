#ifndef STATIC_BUFFER_HPP
#define STATIC_BUFFER_HPP

#include <cstddef>
#include <new>
#include <cassert>
#include <utility>
#include <type_traits>

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
    : used(0)
  {}

  StaticBuffer(const StaticBuffer&) = delete;
  StaticBuffer& operator=(const StaticBuffer&) = delete;

  StaticBuffer(StaticBuffer&& src) {
    T *p_this, *p_src;
    p_this = data();
    p_src = src.data();
    for (size_t i = 0; i < src.used; i++) {
      new (p_this) T(std::move(*p_src));
      ++p_this;
      ++p_src;
    }
    used = src.used;
    src.used = 0;
  }

  StaticBuffer& operator=(StaticBuffer&& src) {
    if (&src != this) {
      T *p_this, *p_src;
      p_this = data();
      p_src = src.data();
      for (size_t i = 0; i < src.used; i++) {
        *p_this = std::move(*p_src);
        ++p_this;
        ++p_src;
      }
      used = src.used;
      src.used = 0;
    }
    return *this;
  }

  template<typename ...Ts>
  void
  emplace_back(Ts ...args) {
    assert(used < N);
    new (data() + used) T(std::forward<Ts>(args)...);
    ++used;
  }

  T*
  data() {
    return reinterpret_cast<T*>(&store);
  }

  typedef T (*ArrayPtr)[N];

  ArrayPtr
  array_data() {
    return reinterpret_cast<ArrayPtr>(&store);
  }

  ~StaticBuffer() {
    T *p = data();
    for (size_t i = 0; i < used; i++) {
      p[i].~T();
    }
  }

private:
  typedef
  typename std::aligned_storage<sizeof (T[N]), alignof (T)>::type
  StoreType;

  StoreType store;
  size_t used;
};

}

#endif
