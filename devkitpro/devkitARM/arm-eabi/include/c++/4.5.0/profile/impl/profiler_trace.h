// -*- C++ -*-
//
// Copyright (C) 2009, 2010 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 2, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this library; see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.

// As a special exception, you may use this file as part of a free
// software library without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to
// produce an executable, this file does not by itself cause the
// resulting executable to be covered by the GNU General Public
// License.  This exception does not however invalidate any other
// reasons why the executable file might be covered by the GNU General
// Public License.

/** @file profile/impl/profiler_trace.h
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu and Silvius Rus.

#ifndef _GLIBCXX_PROFILE_PROFILER_TRACE_H
#define _GLIBCXX_PROFILE_PROFILER_TRACE_H 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#define _GLIBCXX_IMPL_UNORDERED_MAP std::_GLIBCXX_STD_PR::unordered_map
#include <unordered_map>
#else
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tr1/unordered_map>
#define _GLIBCXX_IMPL_UNORDERED_MAP std::tr1::unordered_map
#endif

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>

#if (defined _GLIBCXX_PROFILE_THREADS) && !(defined _GLIBCXX_HAVE_TLS)
#error You do not seem to have TLS support, which is required by the profile \
  mode.  If your program is not multithreaded, recompile with \
  -D_GLIBCXX_PROFILE_NO_THREADS
#endif

#if defined _GLIBCXX_PROFILE_THREADS && defined _GLIBCXX_HAVE_TLS
#include <pthread.h>
#endif

#include "profile/impl/profiler_state.h"
#include "profile/impl/profiler_node.h"

namespace __gnu_profile
{

#if defined _GLIBCXX_PROFILE_THREADS && defined _GLIBCXX_HAVE_TLS
#define _GLIBCXX_IMPL_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
typedef pthread_mutex_t __mutex_t;
/** @brief Pthread mutex wrapper.  */
_GLIBCXX_PROFILE_DEFINE_DATA(__mutex_t, __global_lock, 
                             PTHREAD_MUTEX_INITIALIZER);
inline void __lock(__mutex_t& __m) { pthread_mutex_lock(&__m); }
inline void __unlock(__mutex_t& __m) { pthread_mutex_unlock(&__m); }
#else
typedef int __mutex_t;
/** @brief Mock mutex interface.  */
#define _GLIBCXX_IMPL_MUTEX_INITIALIZER 0
_GLIBCXX_PROFILE_DEFINE_DATA(__mutex_t, __global_lock, 0);
inline void __lock(__mutex_t& __m) {}
inline void __unlock(__mutex_t& __m) {}
#endif

/** @brief Representation of a warning.  */
struct __warning_data
{
  float __magnitude;
  __stack_t __context;
  const char* __warning_id;
  const char* __warning_message;
  __warning_data();
  __warning_data(float __m, __stack_t __c, const char* __id, 
                 const char* __msg);
  bool operator>(const struct __warning_data& other) const;
};

inline __warning_data::__warning_data()
    : __magnitude(0.0), __context(NULL), __warning_id(NULL),
      __warning_message(NULL)
{
}

inline __warning_data::__warning_data(float __m, __stack_t __c, 
                                      const char* __id, const char* __msg)
    : __magnitude(__m), __context(__c), __warning_id(__id),
      __warning_message(__msg)
{
}

inline bool __warning_data::operator>(const struct __warning_data& other) const
{
  return __magnitude > other.__magnitude;
}

typedef std::_GLIBCXX_STD_PR::vector<__warning_data> __warning_vector_t;

// Defined in profiler_<diagnostic name>.h.
class __trace_hash_func;
class __trace_hashtable_size;
class __trace_map2umap;
class __trace_vector_size;
class __trace_vector_to_list;
class __trace_list_to_slist; 
class __trace_list_to_vector; 
void __trace_vector_size_init();
void __trace_hashtable_size_init();
void __trace_hash_func_init();
void __trace_vector_to_list_init();
void __trace_list_to_slist_init();  
void __trace_list_to_vector_init();  
void __trace_map_to_unordered_map_init();
void __trace_vector_size_report(FILE*, __warning_vector_t&);
void __trace_hashtable_size_report(FILE*, __warning_vector_t&);
void __trace_hash_func_report(FILE*, __warning_vector_t&);
void __trace_vector_to_list_report(FILE*, __warning_vector_t&);
void __trace_list_to_slist_report(FILE*, __warning_vector_t&); 
void __trace_list_to_vector_report(FILE*, __warning_vector_t&);
void __trace_map_to_unordered_map_report(FILE*, __warning_vector_t&);

// Utility functions.
inline size_t __max(size_t __a, size_t __b)
{
  return __a >= __b ? __a : __b;
}

inline size_t __min(size_t __a, size_t __b)
{
  return __a <= __b ? __a : __b;
}

struct __cost_factor
{
  const char* __env_var;
  float __value;
};

typedef std::_GLIBCXX_STD_PR::vector<__cost_factor*> __cost_factor_vector;

_GLIBCXX_PROFILE_DEFINE_DATA(__trace_hash_func*, _S_hash_func, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_hashtable_size*, _S_hashtable_size, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_map2umap*, _S_map2umap, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_vector_size*, _S_vector_size, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_vector_to_list*, _S_vector_to_list, NULL);
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_list_to_slist*, _S_list_to_slist, NULL); 
_GLIBCXX_PROFILE_DEFINE_DATA(__trace_list_to_vector*, _S_list_to_vector, NULL);

_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __vector_shift_cost_factor, 
                             {"__vector_shift_cost_factor", 1.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __vector_iterate_cost_factor,
                             {"__vector_iterate_cost_factor", 1.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __vector_resize_cost_factor,
                             {"__vector_resize_cost_factor", 1.0}); 
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __list_shift_cost_factor,
                             {"__list_shift_cost_factor", 0.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __list_iterate_cost_factor,
                             {"__list_iterate_cost_factor", 10.0}); 
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __list_resize_cost_factor,
                             {"__list_resize_cost_factor", 0.0}); 
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_insert_cost_factor,
                             {"__map_insert_cost_factor", 1.5});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_erase_cost_factor,
                             {"__map_erase_cost_factor", 1.5});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_find_cost_factor,
                             {"__map_find_cost_factor", 1});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __map_iterate_cost_factor,
                             {"__map_iterate_cost_factor", 2.3});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_insert_cost_factor,
                             {"__umap_insert_cost_factor", 12.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_erase_cost_factor,
                             {"__umap_erase_cost_factor", 12.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_find_cost_factor,
                             {"__umap_find_cost_factor", 10.0});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor, __umap_iterate_cost_factor,
                             {"__umap_iterate_cost_factor", 1.7});
_GLIBCXX_PROFILE_DEFINE_DATA(__cost_factor_vector*, __cost_factors, NULL);

_GLIBCXX_PROFILE_DEFINE_DATA(const char*, _S_trace_file_name,
                             _GLIBCXX_PROFILE_TRACE_PATH_ROOT);
_GLIBCXX_PROFILE_DEFINE_DATA(size_t, _S_max_warn_count,
                             _GLIBCXX_PROFILE_MAX_WARN_COUNT);
_GLIBCXX_PROFILE_DEFINE_DATA(size_t, _S_max_stack_depth,
                             _GLIBCXX_PROFILE_MAX_STACK_DEPTH);
_GLIBCXX_PROFILE_DEFINE_DATA(size_t, _S_max_mem,
                             _GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC);

inline size_t __stack_max_depth()
{
  return _GLIBCXX_PROFILE_DATA(_S_max_stack_depth);
}

inline size_t __max_mem()
{
  return _GLIBCXX_PROFILE_DATA(_S_max_mem);
}

/** @brief Base class for all trace producers.  */
template <typename __object_info, typename __stack_info>
class __trace_base
{
 public:
  __trace_base();
  virtual ~__trace_base() {}

  void __add_object(__object_t object, __object_info __info);
  __object_info* __get_object_info(__object_t __object);
  void __retire_object(__object_t __object);
  void __write(FILE* f);
  void __collect_warnings(__warning_vector_t& __warnings);

  void __lock_object_table();
  void __lock_stack_table();
  void __unlock_object_table();
  void __unlock_stack_table();

 private:
  __mutex_t __object_table_lock;
  __mutex_t __stack_table_lock;
  typedef _GLIBCXX_IMPL_UNORDERED_MAP<__object_t, 
                                      __object_info> __object_table_t;
  typedef _GLIBCXX_IMPL_UNORDERED_MAP<__stack_t, __stack_info, __stack_hash, 
                                      __stack_hash> __stack_table_t;
  __object_table_t __object_table;
  __stack_table_t __stack_table;
  size_t __stack_table_byte_size;

 protected:
  const char* __id;
};

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__collect_warnings(
    __warning_vector_t& __warnings)
{
  typename __stack_table_t::iterator __i = __stack_table.begin();
  for ( ; __i != __stack_table.end(); ++__i )
  {
    __warnings.push_back(__warning_data((*__i).second.__magnitude(), 
                                        (*__i).first, 
                                        __id,
                                        (*__i).second.__advice()));
  }
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__lock_object_table()
{
  __lock(this->__object_table_lock);
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__lock_stack_table()
{
  __lock(this->__stack_table_lock);
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__unlock_object_table()
{
  __unlock(this->__object_table_lock);
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__unlock_stack_table()
{
  __unlock(this->__stack_table_lock);
}

template <typename __object_info, typename __stack_info>
__trace_base<__object_info, __stack_info>::__trace_base()
{
  // Do not pick the initial size too large, as we don't know which diagnostics
  // are more active.
  __object_table.rehash(10000);
  __stack_table.rehash(10000);
  __stack_table_byte_size = 0;
  __id = NULL;
  __object_table_lock = __stack_table_lock = _GLIBCXX_IMPL_MUTEX_INITIALIZER;
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__add_object(
    __object_t __object, __object_info __info)
{
  if (__max_mem() == 0 
      || __object_table.size() * sizeof(__object_info) <= __max_mem()) {
    __lock_object_table();
    __object_table.insert(
        typename __object_table_t::value_type(__object, __info));
    __unlock_object_table();
  }
}

template <typename __object_info, typename __stack_info>
__object_info* __trace_base<__object_info, __stack_info>::__get_object_info(
    __object_t __object)
{
  // XXX: Revisit this to see if we can decrease mutex spans.
  // Without this mutex, the object table could be rehashed during an
  // insertion on another thread, which could result in a segfault.
  __lock_object_table();
  typename __object_table_t::iterator __object_it = 
      __object_table.find(__object);
  if (__object_it == __object_table.end()){
    __unlock_object_table();
    return NULL;
  } else {
    __unlock_object_table();
    return &__object_it->second;
  }
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__retire_object(
    __object_t __object)
{
  __lock_object_table();
  __lock_stack_table();
  typename __object_table_t::iterator __object_it =
      __object_table.find(__object);
  if (__object_it != __object_table.end()){
    const __object_info& __info = __object_it->second;
    const __stack_t& __stack = __info.__stack();
    typename __stack_table_t::iterator __stack_it = 
        __stack_table.find(__stack);
    if (__stack_it == __stack_table.end()) {
      // First occurence of this call context.
      if (__max_mem() == 0 || __stack_table_byte_size < __max_mem()) {
        __stack_table_byte_size += 
            (sizeof(__instruction_address_t) * __size(__stack)
             + sizeof(__stack) + sizeof(__stack_info));
        __stack_table.insert(make_pair(__stack, __stack_info(__info)));
      }
    } else {
      // Merge object info into info summary for this call context.
      __stack_it->second.__merge(__info);
      delete __stack;
    }
    __object_table.erase(__object);
  }
  __unlock_stack_table();
  __unlock_object_table();
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__write(FILE* __f)
{
  typename __stack_table_t::iterator __it;

  for (__it = __stack_table.begin(); __it != __stack_table.end(); __it++) {
    if (__it->second.__is_valid()) {
      fprintf(__f, __id);
      fprintf(__f, "|");
      __gnu_profile::__write(__f, __it->first);
      fprintf(__f, "|");
      __it->second.__write(__f);
    }
  }
}

inline size_t __env_to_size_t(const char* __env_var, size_t __default_value)
{
  char* __env_value = getenv(__env_var);
  if (__env_value) {
    long int __converted_value = strtol(__env_value, NULL, 10);
    if (errno || __converted_value < 0) {
      fprintf(stderr, "Bad value for environment variable '%s'.\n", __env_var);
      abort();
    } else {
      return static_cast<size_t>(__converted_value);
    }
  } else {
    return __default_value;
  }
}

inline void __set_max_stack_trace_depth()
{
  _GLIBCXX_PROFILE_DATA(_S_max_stack_depth) = __env_to_size_t(
      _GLIBCXX_PROFILE_MAX_STACK_DEPTH_ENV_VAR,
      _GLIBCXX_PROFILE_DATA(_S_max_stack_depth));
}

inline void __set_max_mem()
{
  _GLIBCXX_PROFILE_DATA(_S_max_mem) = __env_to_size_t(
      _GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC_ENV_VAR,
      _GLIBCXX_PROFILE_DATA(_S_max_mem));
}

inline int __log_magnitude(float f)
{
  const float log_base = 10.0;
  int result = 0;
  int sign = 1;
  if (f < 0) {
    f = -f;
    sign = -1;
  }
  while (f > log_base) {
    ++result;
    f /= 10.0;
  }
  return sign * result;
}

struct __warn
{
  FILE* __file;
  __warn(FILE* __f) { __file = __f; }
  void operator() (const __warning_data& __info)
  {
    fprintf(__file,  __info.__warning_id);
    fprintf(__file, ": improvement = %d", __log_magnitude(__info.__magnitude));
    fprintf(__file, ": call stack = ");
    __gnu_profile::__write(__file, __info.__context);
    fprintf(__file, ": advice = %s\n", __info.__warning_message);
    free(
        const_cast<void*>(
            reinterpret_cast<const void*>(__info.__warning_message)));
  }
};

inline FILE* __open_output_file(const char* extension)
{
  // The path is made of _S_trace_file_name + "." + extension.
  size_t root_len = strlen(_GLIBCXX_PROFILE_DATA(_S_trace_file_name));
  size_t ext_len = strlen(extension);
  char* file_name = new char[root_len + 1 + ext_len + 1];
  memcpy(file_name, _GLIBCXX_PROFILE_DATA(_S_trace_file_name), root_len);
  *(file_name + root_len) = '.';
  memcpy(file_name + root_len + 1, extension, ext_len + 1);
  FILE* out_file = fopen(file_name, "w");
  if (out_file) {
    return out_file;
  } else {
    fprintf(stderr, "Could not open trace file '%s'.\n", file_name);
    abort();
  }
}

/** @brief Final report method, registered with @b atexit.
 *
 * This can also be called directly by user code, including signal handlers.
 * It is protected against deadlocks by the reentrance guard in profiler.h.
 * However, when called from a signal handler that triggers while within
 * __gnu_profile (under the guarded zone), no output will be produced.
 */
inline void __report(void)
{
  __lock(_GLIBCXX_PROFILE_DATA(__global_lock));

  __warning_vector_t __warnings;

  FILE* __raw_file = __open_output_file("raw");
  __trace_vector_size_report(__raw_file, __warnings);
  __trace_hashtable_size_report(__raw_file, __warnings);
  __trace_hash_func_report(__raw_file, __warnings);
  __trace_vector_to_list_report(__raw_file, __warnings);
  __trace_list_to_slist_report(__raw_file, __warnings);
  __trace_list_to_vector_report(__raw_file, __warnings);
  __trace_map_to_unordered_map_report(__raw_file, __warnings);
  fclose(__raw_file);

  // Sort data by magnitude.
  // XXX: instead of sorting, should collect only top N for better performance.
  size_t __cutoff = __min(_GLIBCXX_PROFILE_DATA(_S_max_warn_count),
                          __warnings.size());

  std::sort(__warnings.begin(), __warnings.end(),
            std::greater<__warning_vector_t::value_type>());
  __warnings.resize(__cutoff);

  FILE* __warn_file = __open_output_file("txt");
  std::for_each(__warnings.begin(), __warnings.end(), __warn(__warn_file));
  fclose(__warn_file);

  __unlock(_GLIBCXX_PROFILE_DATA(__global_lock));
}

inline void __set_trace_path()
{
  char* __env_trace_file_name = getenv(_GLIBCXX_PROFILE_TRACE_ENV_VAR);

  if (__env_trace_file_name) { 
    _GLIBCXX_PROFILE_DATA(_S_trace_file_name) = __env_trace_file_name; 
  }

  // Make sure early that we can create the trace file.
  fclose(__open_output_file("txt"));
}

inline void __set_max_warn_count()
{
  char* __env_max_warn_count_str = getenv(
      _GLIBCXX_PROFILE_MAX_WARN_COUNT_ENV_VAR);

  if (__env_max_warn_count_str) {
    _GLIBCXX_PROFILE_DATA(_S_max_warn_count) = static_cast<size_t>(
        atoi(__env_max_warn_count_str));
  }
}

inline void __read_cost_factors()
{
  std::string __conf_file_name(_GLIBCXX_PROFILE_DATA(_S_trace_file_name));
  __conf_file_name += ".conf";

  std::ifstream __conf_file(__conf_file_name.c_str());

  if (__conf_file.is_open()) {
    std::string __line;

    while (getline(__conf_file, __line)) {
      std::string::size_type __i = __line.find_first_not_of(" \t\n\v");

      if (__line.length() <= 0 || __line[__i] == '#') {
        // Skip empty lines or comments.
        continue;
      }

      // Trim.
      __line.erase(std::remove(__line.begin(), __line.end(), ' '), 
                   __line.end());
      std::string::size_type __pos = __line.find("=");
      std::string __factor_name = __line.substr(0, __pos);
      std::string::size_type __end = __line.find_first_of(";\n");
      std::string __factor_value = __line.substr(__pos + 1, __end - __pos);

      setenv(__factor_name.c_str(), __factor_value.c_str(), 0);
    }
  } 
}

struct __cost_factor_writer
{
  FILE* __file;
  __cost_factor_writer(FILE* __f) : __file(__f) {}
  void operator() (const __cost_factor* __factor)
  {
    fprintf(__file, "%s = %f\n", __factor->__env_var, __factor->__value);
  }
};

inline void __write_cost_factors()
{
  FILE* __file = __open_output_file("conf.out");
  std::for_each(_GLIBCXX_PROFILE_DATA(__cost_factors)->begin(),
                _GLIBCXX_PROFILE_DATA(__cost_factors)->end(),
                __cost_factor_writer(__file));
  fclose(__file);
}

struct __cost_factor_setter
{
  void operator() (__cost_factor* __factor)
  {
    char* __env_cost_factor;
    if (__env_cost_factor = getenv(__factor->__env_var))
      __factor->__value = atof(__env_cost_factor);
  }
};

inline void __set_cost_factors()
{
  _GLIBCXX_PROFILE_DATA(__cost_factors) = new __cost_factor_vector;
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__vector_shift_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__vector_iterate_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__vector_resize_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__list_shift_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__list_iterate_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__list_resize_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_insert_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_erase_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_find_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__map_iterate_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_insert_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_erase_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_find_cost_factor));
  _GLIBCXX_PROFILE_DATA(__cost_factors)->push_back(
      &_GLIBCXX_PROFILE_DATA(__umap_iterate_cost_factor));
  std::for_each(_GLIBCXX_PROFILE_DATA(__cost_factors)->begin(),
                _GLIBCXX_PROFILE_DATA(__cost_factors)->end(),
                __cost_factor_setter());
}

inline void __profcxx_init_unconditional()
{
  __lock(_GLIBCXX_PROFILE_DATA(__global_lock));

  if (__is_invalid()) {

    __set_max_warn_count();

    if (_GLIBCXX_PROFILE_DATA(_S_max_warn_count) == 0) {

      __turn_off();

    } else {

      __set_max_stack_trace_depth();
      __set_max_mem();
      __set_trace_path();
      __read_cost_factors(); 
      __set_cost_factors();
      __write_cost_factors();

      __trace_vector_size_init();
      __trace_hashtable_size_init();
      __trace_hash_func_init();
      __trace_vector_to_list_init();
      __trace_list_to_slist_init(); 
      __trace_list_to_vector_init();
      __trace_map_to_unordered_map_init();

      atexit(__report);

      __turn_on();

    }
  }

  __unlock(_GLIBCXX_PROFILE_DATA(__global_lock));
}

/** @brief This function must be called by each instrumentation point.
 *
 * The common path is inlined fully.
 */
inline bool __profcxx_init(void)
{
  if (__is_invalid()) {
    __profcxx_init_unconditional();
  }

  return __is_on();
}

} // namespace __gnu_profile

#endif /* _GLIBCXX_PROFILE_PROFILER_TRACE_H */
