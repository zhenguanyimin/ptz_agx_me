#ifndef __HELPER_DEF_H__
#define __HELPER_DEF_H__

#include <functional>
#define HELPER_BEGIN_NAMESPACE(x)  namespace x {
#define HELPER_END_NAMESPACE(x)    }
#define HELPER_USING_NAMESPACE(x)  using namespace x

#define HELPER_DEPRECATED(msg) __declspec(deprecated(msg))


// Taken from google-protobuf stubs/common.h
//
// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.

//
// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

// When you upcast (that is, cast a pointer from type Foo to type
// SuperclassOfFoo), it's fine to use implicit_cast<>, since upcasts
// always succeed.  When you downcast (that is, cast a pointer from
// type Foo to type SubclassOfFoo), static_cast<> isn't safe, because
// how do you know the pointer is really of type SubclassOfFoo?  It
// could be a bare Foo, or of type DifferentSubclassOfFoo.  Thus,
// when you downcast, you should use this macro.  In debug mode, we
// use dynamic_cast<> to double-check the downcast is legal (we die
// if it's not).  In normal mode, we do the efficient static_cast<>
// instead.  Thus, it's important to test in debug mode to make sure
// the cast is legal!
//    This is the only place in the code we should use dynamic_cast<>.
// In particular, you SHOULDN'T be using dynamic_cast<> in order to
// do RTTI (eg code like this:
//    if (dynamic_cast<Subclass1>(foo)) HandleASubclass1Object(foo);
//    if (dynamic_cast<Subclass2>(foo)) HandleASubclass2Object(foo);
// You should design the code some other way not to need this.

template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers
{
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
  return static_cast<To>(f);
}

#define IS_POWER_OF_TWO(x) ((x) != 0 && (((x) & ((x)-1)) == 0))
#define PAD_SIZE(s) (((s)+3)&~3)

#define HBYTE8B(x) ((x&0xf0)>>4)
#define LBYTE8B(x) (x&0xf)
#define R8B(h,l) ((h<<4)|l)

#define HBYTE16B(x) ((x&0xff00)>>8)
#define LBYTE16B(x) (x&0xff)
#define R16B(h,l) ((h<<8)|l)

#define HBYTE32B(x) ((x&0xffff0000)>>16)
#define LBYTE32B(x) (x&0xffff)
#define R32B(h,l) ((h<<16)|l)

#define HBYTE64B(x) ((static_cast<uint64_t>(x)&0xffffffff00000000)>>32)
#define LBYTE64B(x) (static_cast<uint64_t>(x)&0xffffffff)
#define R64B(h,l) ((static_cast<uint64_t>(h)<<32)|l)

//指定的某一位数置1
#define SET_BIT(x, y)  (x |= (1 << y))
//指定的某一位数置0
#define RESET_BIT(x, y)  (x &= ~(1 << y))
//指定的某一位数取反
#define REVERSE_BIT(x, y)  (x ^= (1 << y))
//获取的某一位的值
#define GET_BIT(x, y)   ((x) >> (y)& 1)

HELPER_BEGIN_NAMESPACE(HELPER)

/// <summary>
/// std map 容器元素删除
/// </summary>
/// <typeparam name="Map"></typeparam>
/// <typeparam name="Predicate"></typeparam>
/// <param name="m"></param>
/// <param name="p"></param>
template<typename Map, typename Predicate>
void map_erase_if(Map& m, Predicate const& p)
{
	for (auto it = m.begin(); it != m.end();)
	{
		if (p(*it)) it = m.erase(it);
		else ++it;
	}
}

/// <summary>
/// RAII资源释放
/// </summary>
class scope_exit {
public:
	explicit scope_exit(std::function<void()> cb)
		: cb_(cb){}
	~scope_exit(){
		if (cb_)
			cb_();
	}
private:
	std::function<void()> cb_;
};

HELPER_BEGIN_NAMESPACE(LOCK_FLAG)
// LOCK PROPERTIES
struct __adopt_lock_t {};// indicates adopt lock
struct __defer_lock_t {};// indicates defer lock
const __adopt_lock_t lock_adopt;
const __defer_lock_t lock_defer;

struct __adopt_read_lock_t{};
struct __adopt_write_lock_t{};
struct __defer_read_lock_t{};
struct __defer_write_lock_t{};
const __adopt_read_lock_t lock_adopt_read;
const __adopt_write_lock_t lock_adopt_write;
const __defer_read_lock_t lock_defer_read;
const __defer_write_lock_t lock_defer_write;
HELPER_END_NAMESPACE(LOCK_FLAG)
HELPER_END_NAMESPACE(HELPER)

#endif