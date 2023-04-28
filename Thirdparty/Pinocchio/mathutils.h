/*  This file is part of the Pinocchio automatic rigging library.
    Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MATHUTILS_H_INCLUDED
#define MATHUTILS_H_INCLUDED

#include <math.h>
//#include <functional>
#include <string>
#include "Pinocchio.h"

#ifdef _WIN32
#pragma warning(disable:4355)
#pragma warning(disable:4996)
#pragma warning(disable:4251)
#endif //_WIN32

namespace nsPinocchio {
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline int ROUND(double x) { return (int)(x + 0.5); }
inline int SIGN(double x) { return (x > 0.) ? 1 : -1; }
template<class T> T SQR(const T & x) { return x * x; }
template<class T> T CUBE(const T & x) { return x * x * x; }
template<class T> T QUAD(const T & x) { return SQR(SQR(x)); }

//TODO Needs a proper rewrite to replace unary/binary.
// Ripped from functional.
template <class _Arg, class _Result>
struct unary_function { // base class for unary functions
    using argument_type = _Arg;
    using result_type   = _Result;
};

template <class _Arg1, class _Arg2, class _Result>
struct binary_function { // base class for binary functions
    using first_argument_type  = _Arg1;
    using second_argument_type = _Arg2;
    using result_type          = _Result;
};

template <class _Fn>
class binder2nd : public unary_function<typename _Fn::first_argument_type,
                      typename _Fn::result_type> { // functor adapter _Func(left, stored)
public:
    using _Base         = unary_function<typename _Fn::first_argument_type, typename _Fn::result_type>;
    using argument_type = typename _Base::argument_type;
    using result_type   = typename _Base::result_type;

    binder2nd(const _Fn& _Func, const typename _Fn::second_argument_type& _Right) : op(_Func), value(_Right) {}

    result_type operator()(const argument_type& _Left) const {
        return op(_Left, value);
    }

    result_type operator()(argument_type& _Left) const {
        return op(_Left, value);
    }

protected:
    _Fn op;
    typename _Fn::second_argument_type value; // the right operand
};

template <class _Fn, class _Ty>
_NODISCARD binder2nd<_Fn> bind2nd(const _Fn& _Func, const _Ty& _Right) {
    typename _Fn::second_argument_type _Val(_Right);
    return binder2nd<_Fn>(_Func, _Val);
}
// End Ripped from functional.

template <class T> class maximum : public binary_function<T, T, T>
{
public:
    T operator()(const T &a1, const T &a2) const { return max(a1, a2); }
};

template <class T> class minimum : public binary_function<T, T, T>
{
public:
    T operator()(const T &a1, const T &a2) const { return min(a1, a2); }
};

template <class A1, class A2, class R> class myMult : public binary_function<A1, A2, R>
{
public:
    R operator()(const A1 &a1, const A2 &a2) const { return a1 * a2; }
};

template <class A1, class A2, class R> class myDiv : public binary_function<A1, A2, R>
{
public:
    R operator()(const A1 &a1, const A2 &a2) const { return a1 / a2; }
};

template <class T> class ident : public unary_function<T, T>
{
public:
    T operator()(const T &v) const { return v; }
};

template<class T> class construct
{
public:
    typedef T result_type;
    template<class From> T operator()(const From &x) const { return T(x); }
};

#ifdef _WIN32
inline bool operator==(const string &s1, const string &s2) { return s1.compare(s2) == 0; }
inline bool operator<(const string &s1, const string &s2) { return s1.compare(s2) < 0; }
#endif
}
#endif //MATHUTILS_H_INCLUDED
