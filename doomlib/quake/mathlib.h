/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// mathlib.h

#ifndef _QUAKE_MATHLIB_H_
#define _QUAKE_MATHLIB_H_

#include <type_traits>
#include <algorithm>

using real_t = float; // type of float

using vec_t = real_t;

namespace math {
	template<typename T>
	struct type_limits {
		using type = T;
		using numeric_limits = std::numeric_limits<type>;

		static constexpr type min() noexcept { return numeric_limits::min(); }
		static constexpr type max() noexcept { return numeric_limits::max(); }
		static constexpr type lowest() noexcept { return numeric_limits::lowest(); }
		static constexpr type epsilon() noexcept { return numeric_limits::epsilon(); }
		static constexpr type round_error() noexcept { return numeric_limits::round_error(); }
		static constexpr type denorm_min() noexcept { return numeric_limits::denorm_min(); }
		static constexpr type infinity() noexcept { return numeric_limits::infinity(); }
		static constexpr type quiet_NaN() noexcept { return numeric_limits::quiet_NaN(); }
		static constexpr type signaling_NaN() noexcept { return numeric_limits::signaling_NaN(); }
		static constexpr bool is_signed = numeric_limits::is_signed;
		static constexpr auto digits = numeric_limits::digits;
		static constexpr auto digits10 = numeric_limits::digits10;
	};

	template<typename T>
	typename std::enable_if<std::is_arithmetic<std::decay_t<T>>::value, std::decay_t<T>>::type
		static inline constexpr  zero() { return T{ 0 }; }
	template<typename T>
	typename std::enable_if<std::is_arithmetic<std::decay_t<T>>::value, std::decay_t<T>>::type
		static inline constexpr  one() { return T{ 1 }; }


	template<typename T>
	typename std::enable_if<std::is_floating_point<std::decay_t<T>>::value, std::decay_t<T>>::type
		static inline constexpr half() { return T{ 0.5f }; }
	template<typename T>
	typename std::enable_if<std::is_floating_point<std::decay_t<T>>::value, T>::type
		static inline constexpr abs(const T& value) { return (T{} > value) ? -value : value; }
	template<typename T>
	typename std::enable_if<std::is_signed<std::decay_t<T>>::value, T>::type
		static inline constexpr abs(const T& value) { return (T{} > value) ? -value : value; }
	template<typename T>
	typename std::enable_if<std::is_floating_point<std::decay_t<T>>::value, T>::type
		static inline constexpr int_part(const T& value) {
		return (T{} > value) ? -value : value;
	}
};

template<typename VEC_TYPE>
class _vec3_t {
public:
	using type = VEC_TYPE;
	_vec3_t() : x(math::zero<type>()), y(math::zero<type>()), z(math::zero<type>()) {   }
	_vec3_t(type x, type y, type z) : x(x), y(y), z(z) {   }
	template<typename T>
	_vec3_t(const _vec3_t<T>& copy) : x(copy.x), y(copy.y), z(copy.z) {   }
	template<typename T>
	_vec3_t(_vec3_t<T>&& move) : x(move.x), y(move.y), z(move.z) {   }
	template<typename T>
	_vec3_t& operator=(const _vec3_t<T>& copy) { *this = vec3_t(copy); return *this; }
	template<typename T>
	_vec3_t& operator=(_vec3_t<T>&& move) { *this = vec3_t(move); return *this; }
	inline type& operator[](size_t i) { return data[i]; }
	inline const type& operator[](size_t i) const { return data[i]; }

	type length() const {	// FIXME
		return std::sqrt(x * x + y * y + z * z);
	}
	type normalize() {
		type len = this->length();

		if (len) {
			type ilength = 1 / len;
			data[0] *= ilength;
			data[1] *= ilength;
			data[2] *= ilength;
		}
		return len;
	}
	template<typename T>
	type normalize(_vec3_t<T>& ret) const {
		type len = this->length();
		ret = *this;
		if (len) {
			type ilength = 1 / len;
			ret[0] *= ilength;
			ret[1] *= ilength;
			ret[2] *= ilength;
		}
		return len;
	}
	union {
		struct {
			type x;
			type y;
			type z;
		};
		type data[3];
	};
};
using vec3_t = _vec3_t<float>;

template<typename A, typename B, typename R = A>
static inline _vec3_t<R> operator+(const _vec3_t<A>& a, const _vec3_t<B>& b) {
	return _vec3_t<R>(a.x + b.x, a.y + b.y, a.z + b.z);
}
template<typename A, typename B, typename R = A>
static inline _vec3_t<R> operator-(const _vec3_t<A>& a, const _vec3_t<B>& b) {
	return _vec3_t<R>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template<typename A, typename B, typename R>
static inline void CrossProduct(const _vec3_t<A>& v1, _vec3_t<B>& v2, _vec3_t<R>& cross)
{
	cross = _vec3_t<R>(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
}

template<typename A,typename R=A>
static inline _vec3_t<R> operator~(const _vec3_t<A>& a) { return _vec3_t<R>(~a[0], ~a[1], ~a[2] ); }


template<typename A, typename B, typename R>
static inline void VectorScale(const _vec3_t<A>& in, const B scale, _vec3_t<R>& out){
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

template<typename A, typename B, typename R=A>
static inline _vec3_t<R> operator*(const _vec3_t<A>& a, const B& value) { 
	_vec3_t<R> ret;
	VectorScale(a, value, ret);
	return ret;
}

template<typename VEC_TYPE>
class _vec5_t {
public:
	using vec_type = VEC_TYPE;
	_vec5_t() : x(math::zero<vec_type>()), y(math::zero<vec_type>()), z(math::zero<vec_type>()) , u(math::zero<vec_type>()), v(math::zero<vec_type>()) {}
	inline vec_type& operator[](size_t i) { return data[i]; }
	inline const vec_type& operator[](size_t i) const { return data[i]; }
	union {
		struct {
			vec_type x;
			vec_type y;
			vec_type z;
			vec_type u;
			vec_type v;
		};
		vec_type data[5];
	};
};
using vec5_t = _vec5_t<float>;
//typedef vec_t vec3_t[3];
//typedef vec_t vec5_t[5];
using fixed4_t = int;
using fixed8_t = int;
using fixed16_t = int;


#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

struct mplane_s;

EXTERN  vec3_t vec3_origin;
EXTERN 	int nanmask;

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}

EXTERN_CPP void VectorMA (vec3_t veca, float scale, vec3_t vecb, vec3_t vecc);
#if 0
EXTERN_CPP vec_t _DotProduct (vec3_t v1, vec3_t v2);
EXTERN_CPP void _VectorSubtract (vec3_t veca, vec3_t vecb, vec3_t out);
EXTERN_CPP void _VectorAdd (vec3_t veca, vec3_t vecb, vec3_t out);
EXTERN_CPP void _VectorCopy (vec3_t in, vec3_t out);
#endif
EXTERN_CPP int VectorCompare (vec3_t v1, vec3_t v2);
EXTERN_CPP vec_t Length (vec3_t v);
EXTERN_CPP void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross);
EXTERN_CPP float VectorNormalize (vec3_t v);		// returns vector length
EXTERN_CPP void VectorInverse (vec3_t v);
EXTERN_CPP void VectorScale (vec3_t in, vec_t scale, vec3_t out);
EXTERN_CPP int Q_log2(int val);

EXTERN_CPP void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3]);
EXTERN_CPP void R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4]);

EXTERN_CPP void FloorDivMod (double numer, double denom, int *quotient, int *rem);
EXTERN_CPP fixed16_t Invert24To16(fixed16_t val);
EXTERN_CPP int GreatestCommonDivisor (int i1, int i2);

EXTERN_CPP void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
EXTERN_CPP int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct mplane_s *plane);
EXTERN_CPP float	anglemod(float a);


// used to handle time

// implmentation dependent

class quake_time_t {
public:
	static constexpr uint32_t NSEC = 1000000000U;
	static constexpr uint32_t USEC = 1000000U;
	static constexpr uint32_t MSEC = 1000U;


	constexpr quake_time_t() : _sec(0), _usec(0) {}
	constexpr quake_time_t(int32_t sec, int32_t usec) : _sec(sec), _usec(usec) {}
	constexpr quake_time_t(double fsec) :
		_sec(static_cast<int32_t>(fsec)),
		_usec(static_cast<int32_t>(fsec - static_cast<double>(_sec)* USEC + 0.5e-6)) {
		if (_usec >= USEC) {
			_sec += _usec / USEC;
			_usec %= USEC;
		}
	}
	operator double() const {
		return (_sec * 1.0 + _usec * 1.0e-6); 
	}
	constexpr inline int32_t sec() const { return _sec; }
	constexpr inline int32_t usec() const { return _usec; }
	constexpr inline bool operator==(const quake_time_t& r) { return sec() == r.sec() && usec() == r.usec(); }
	constexpr inline bool operator!=(const quake_time_t& r) { return !(*this == r); }
	constexpr inline bool operator<(const quake_time_t& r) const { return (sec() == r.sec()) ? usec() < r.usec() : sec() < r.sec(); }
	constexpr inline bool operator>(const quake_time_t& r) const { return (sec() == r.sec()) ? usec() > r.usec() : sec() > r.sec(); }
	inline bool operator<=(const quake_time_t& r) const { return *this == r || *this < r; }
	inline bool operator>=(const quake_time_t& r) const { return *this == r || *this > r; }
	quake_time_t& operator+=(const quake_time_t& r) {
		_sec += r.sec();
		_usec += r.usec();
		if (_usec >= USEC) { ++_sec; _usec -= USEC; }
		return *this;
	}
	quake_time_t& operator-=(const quake_time_t& r) {
		_sec += r.sec();
		if (_usec < r.usec()) { --_sec; _usec += USEC; }
		_usec -= r.usec();
		return *this;
	}
	inline quake_time_t operator-(const quake_time_t& r) const {
		quake_time_t tmp_tv(*this);
		tmp_tv -= r;
		return tmp_tv;
	}
	inline quake_time_t operator+(const quake_time_t& r) const {
		quake_time_t tmp_tv(*this);
		tmp_tv += r;
		return tmp_tv;
	}
	inline quake_time_t operator*(int32_t r) const {
		uint32_t tmp_sec, tmp_usec;
		tmp_usec = _usec * r;
		tmp_sec = _sec * r + tmp_usec / USEC;
		tmp_usec %= USEC;
		return quake_time_t(tmp_sec, tmp_usec);
	}
	inline quake_time_t operator*(const double& r) const {
		return quake_time_t(static_cast<double>(*this) * r);
	}
	inline quake_time_t operator/(int32_t r) const {
		return quake_time_t(_sec / r, ((_sec % r) * USEC + _usec) / r);
	}
	inline quake_time_t operator/(const double& r) const {
		return quake_time_t(static_cast<double>(*this) / r);
	}
	//constexpr static quake_time_t MAX = quake_time_t(std::numeric_limits<int32_t>::max(), USEC - 1);
	//constexpr static quake_time_t MIN = quake_time_t(std::numeric_limits<int32_t>::min(), -(USEC - 1));
public:
	int32_t _sec;
	int32_t _usec;
};

#if 0
static inline quake_time_t operator*(int32_t l, const quake_time_t& r) { return r * l; }

static inline  quake_time_t operator*(const double& l, const quake_time_t& r) { return r * l; }
static inline constexpr quake_time_t operator-(const quake_time_t& r) {
	return quake_time_t(-r.sec(), -r.usec());
}

#endif

namespace math {
	//template<>
	static inline constexpr  quake_time_t zero() { return quake_time_t(0, 0); }
	static inline constexpr  quake_time_t one() { return quake_time_t(1, 0);}
	static inline constexpr  quake_time_t half() { return quake_time_t(0, quake_time_t::USEC / 2);}

};


#define BOX_ON_PLANE_SIDE(emins, emaxs, p)	\
	(((p)->type < 3)?						\
	(										\
		((p)->dist <= (emins)[(p)->type])?	\
			1								\
		:									\
		(									\
			((p)->dist >= (emaxs)[(p)->type])?\
				2							\
			:								\
				3							\
		)									\
	)										\
	:										\
		BoxOnPlaneSide( (emins), (emaxs), (p)))


#endif