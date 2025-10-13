/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once
// #include <sys/mman.h>  // mmap/munmap

#include "memtracer/common.h"

using namespace memtracer;
#if defined(__linux__)
#define MT_THROW __THROW
#else
#define MT_THROW
#endif

extern malloc_func_t orig_malloc;
extern free_func_t   orig_free;
extern mmap_func_t   orig_mmap;
extern munmap_func_t orig_munmap;

extern "C" mt_visible void *malloc(size_t size);
extern "C" mt_visible void *calloc(size_t nelem, size_t size);
extern "C" mt_visible void *realloc(void *ptr, size_t size);
extern "C" mt_visible void  free(void *ptr);
extern "C" mt_visible void  cfree(void *ptr);
extern "C" mt_visible void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) MT_THROW;
extern "C" mt_visible int   munmap(void *addr, size_t length) MT_THROW;
extern "C" mt_visible char *strdup(const char *s) MT_THROW;
extern "C" mt_visible char *strndup(const char *s, size_t n) MT_THROW;

mt_visible void *operator new(std::size_t size);
mt_visible void *operator new[](std::size_t size);
mt_visible void *operator new(std::size_t size, const std::nothrow_t &) noexcept;
mt_visible void *operator new[](std::size_t size, const std::nothrow_t &) noexcept;
mt_visible void  operator delete(void *ptr) noexcept;
mt_visible void  operator delete[](void *ptr) noexcept;
mt_visible void  operator delete(void *ptr, const std::nothrow_t &) noexcept;
mt_visible void  operator delete[](void *ptr, const std::nothrow_t &) noexcept;
mt_visible void  operator delete(void *ptr, std::size_t size) noexcept;
mt_visible void  operator delete[](void *ptr, std::size_t size) noexcept;

// unsupported libc functions, for simpler memory tracking.
extern "C" mt_visible char *realpath(const char *fname, char *resolved_name) MT_THROW;
extern "C" mt_visible void *memalign(size_t alignment, size_t size) MT_THROW;
extern "C" mt_visible void *valloc(size_t size) MT_THROW;
extern "C" mt_visible void *pvalloc(size_t size) MT_THROW;
extern "C" mt_visible int   posix_memalign(void **memptr, size_t alignment, size_t size) MT_THROW;

#ifdef __linux__
extern "C" mt_visible int      brk(void *addr) MT_THROW;
extern "C" mt_visible void    *sbrk(intptr_t increment) MT_THROW;
extern "C" mt_visible long int syscall(long int __sysno, ...) MT_THROW;
#elif defined(__MACH__)
extern "C" mt_visible void *brk(const void *addr) MT_THROW;
extern "C" mt_visible void *sbrk(int increment) MT_THROW;
extern "C" mt_visible int   syscall(int __sysno, ...) MT_THROW;
#endif

// forword libc interface declarations
#if defined(__GLIBC__) && defined(__linux__)
extern "C" mt_visible void *__libc_malloc(size_t size);
extern "C" mt_visible void *__libc_calloc(size_t nmemb, size_t size);
extern "C" mt_visible void *__libc_realloc(void *ptr, size_t size);
extern "C" mt_visible void  __libc_free(void *ptr);
extern "C" mt_visible void  __libc_cfree(void *ptr);
extern "C" mt_visible void *__libc_valloc(size_t size);
extern "C" mt_visible void *__libc_pvalloc(size_t size);
extern "C" mt_visible void *__libc_memalign(size_t alignment, size_t size);
extern "C" mt_visible int   __posix_memalign(void **memptr, size_t alignment, size_t size);
#endif