/*
 * Copyright (C) 2008-2017 Max Kellermann <max.kellermann@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef COMPILER_H
#define COMPILER_H

#define GCC_MAKE_VERSION(major, minor, patchlevel) ((major) * 10000 + (minor) * 100 + patchlevel)

#ifdef __GNUC__
#define GCC_VERSION GCC_MAKE_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#define GCC_VERSION 0
#endif

#ifdef __clang__
#  define CLANG_VERSION GCC_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__)
#  define CLANG_VERSION 0
#endif

/**
 * Are we building with the specified version of gcc (not clang or any
 * other compiler) or newer?
 */
#define GCC_CHECK_VERSION(major, minor) \
	(CLANG_VERSION == 0 && \
	 GCC_VERSION >= GCC_MAKE_VERSION(major, minor, 0))

/**
 * Are we building with clang (any version) or at least the specified
 * gcc version?
 */
#define CLANG_OR_GCC_VERSION(major, minor) \
	(CLANG_VERSION > 0 || GCC_CHECK_VERSION(major, minor))

/**
 * Are we building with gcc (not clang or any other compiler) and a
 * version older than the specified one?
 */
#define GCC_OLDER_THAN(major, minor) \
	(GCC_VERSION > 0 && CLANG_VERSION == 0 && \
	 GCC_VERSION < GCC_MAKE_VERSION(major, minor, 0))

#ifdef __clang__
#  if __clang_major__ < 3
#    error Sorry, your clang version is too old.  You need at least version 3.1.
#  endif
#elif defined(__GNUC__)
#  if GCC_OLDER_THAN(4,9)
#    error Sorry, your gcc version is too old.  You need at least version 4.9.
#  endif
#else
#  warning Untested compiler.  Use at your own risk!
#endif

/**
 * Are we building with the specified version of clang or newer?
 */
#define CLANG_CHECK_VERSION(major, minor) \
	(CLANG_VERSION >= GCC_MAKE_VERSION(major, minor, 0))

#if CLANG_OR_GCC_VERSION(4,0)

/* GCC 4.x */

#define gcc_const __attribute__((const))
#define gcc_deprecated __attribute__((deprecated))
#define gcc_may_alias __attribute__((may_alias))
#define gcc_malloc __attribute__((malloc))
#define gcc_noreturn __attribute__((noreturn))
#define gcc_packed __attribute__((packed))
#define gcc_printf(a,b) __attribute__((format(printf, a, b)))
#define gcc_pure __attribute__((pure))
#define gcc_sentinel __attribute__((sentinel))
#define gcc_unused __attribute__((unused))
#define gcc_warn_unused_result __attribute__((warn_unused_result))

#define gcc_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#define gcc_nonnull_all __attribute__((nonnull))

#define gcc_likely(x) __builtin_expect (!!(x), 1)
#define gcc_unlikely(x) __builtin_expect (!!(x), 0)

#define gcc_visibility_hidden __attribute__((visibility("hidden")))
#define gcc_visibility_default __attribute__((visibility("default")))

#define gcc_always_inline __attribute__((always_inline))

#else

/* generic C compiler */

#define gcc_const
#define gcc_deprecated
#define gcc_may_alias
#define gcc_malloc
#define gcc_noreturn
#define gcc_packed
#define gcc_printf(a,b)
#define gcc_pure
#define gcc_sentinel
#define gcc_unused
#define gcc_warn_unused_result

#define gcc_nonnull(...)
#define gcc_nonnull_all

#define gcc_likely(x) (x)
#define gcc_unlikely(x) (x)

#define gcc_visibility_hidden
#define gcc_visibility_default

#define gcc_always_inline inline

#endif

#if CLANG_OR_GCC_VERSION(4,3)

#define gcc_hot __attribute__((hot))
#define gcc_cold __attribute__((cold))

#else /* ! GCC_UNUSED >= 40300 */

#define gcc_hot
#define gcc_cold

#endif /* ! GCC_UNUSED >= 40300 */

#if GCC_CHECK_VERSION(4,6)
#define gcc_flatten __attribute__((flatten))
#else
#define gcc_flatten
#endif

#ifndef __cplusplus
/* plain C99 has "restrict" */
#define gcc_restrict restrict
#elif CLANG_OR_GCC_VERSION(4,0)
/* "__restrict__" is a GCC extension for C++ */
#define gcc_restrict __restrict__
#else
/* disable it on other compilers */
#define gcc_restrict
#endif

#ifndef __has_feature
  // define dummy macro for non-clang compilers
  #define __has_feature(x) 0
#endif

#if __has_feature(attribute_unused_on_fields)
#define gcc_unused_field gcc_unused
#else
#define gcc_unused_field
#endif

#if defined(__GNUC__) || defined(__clang__)
#define gcc_unreachable() __builtin_unreachable()
#else
#define gcc_unreachable()
#endif

#endif
