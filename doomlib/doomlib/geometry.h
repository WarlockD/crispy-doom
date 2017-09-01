#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include "tiny_common.h"
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include <array>

template<size_t DimCols,size_t DimRows,typename T> class mat;
namespace priv {
	template<typename A, typename B>
	static constexpr inline B _convert(A a, std::true_type, std::false_type) { return static_cast<B>(static_cast<A>(0.5) + a); }
	template<typename A, typename B>
	static constexpr inline B _convert(A a, std::false_type, std::true_type) { return static_cast<B>(a); }
	template<typename A, typename B>
	static constexpr inline B _convert(A a, std::false_type, std::false_type) { return static_cast<B>(a); }
	template<typename A, typename B>
	static constexpr inline B _convert(A a, std::true_type, std::true_type) { return static_cast<B>(a); }
	template<typename A, typename B>
	static constexpr inline B convert(A a) { return _convert<A,B>(a, std::is_floating_point<A>(), std::is_floating_point<B>()); }

	template<size_t N, typename A, typename B>
	static constexpr inline B _get_array(const std::array<A, N>& a, size_t i) { return a.size() < i ? convert<A,B>(a[i]) : B{}; }

	template<typename A, size_t AN, typename B, size_t BN,typename F, std::size_t...i>
	static constexpr inline std::array<B,BN> get_array(const std::array<A, AN>& a, std::integer_sequence<std::size_t, i...>) {
		return std::array<B, BN> { { _get_array(a,i)... } };
	}
	template<typename A, size_t AN, typename B, size_t BN>
	static constexpr inline std::array<B, BN> convert_array(const std::array<A, AN>& a) {
		return get_array(a, std::make_index_sequence<AN>);
	}

}
namespace vector_ops {
	// https://codereview.stackexchange.com/questions/97962/c-vector-that-uses-expression-templates-technique-to-increase-performance-of-m
	// humm intresting idea
	template <size_t DIM, typename T>
	class Vec
	{

	public:
		using array_type = std::array<T, DIM>;
		using size_type = size_t;
		using value_type = T;
		template<typename ARRAYB>
		inline static constexpr array_type _clone(const ARRAYB& in) {
			array_type out;
			size_t i = 0;
			for(;i < in.size() && i < out.size();i++) {
				out[i] = in[i];
				if (i == out.size()) return out;
			}
			while (i != out.size()) { out[i++] = value_type{}; }
			return out;
		}
		constexpr size_type size() const { return DIM; }
		template<typename F>
		constexpr Vec(std::initializer_list<F> e)  { 
			size_t i = 0;
			for (const auto& ev : e) {
				_data[i++] = ev;
				if (i == size()) return;
			}
			while (i != size()) { _data[i++] = value_type{}; }
		}
		template<typename VecOperation>
		constexpr Vec(const VecOperation& vo) : _data(_clone(vo)) { }

		constexpr value_type operator[](size_type i) const { return _data[i]; }
		value_type& operator[](size_type i) { return _data[i]; }
	private:

		array_type _data;
	};
	template<typename T1, typename T2>
	struct VecSum
	{
		const T1& t1;
		const T2& t2;
		constexpr size_t size() const { return std::min(t1.size(), t2.size()); }
		auto operator[](typename T2::size_type i) const { return t1[i] + t2[i]; }
	};
	template<typename T1, typename T2>
	struct VecDiff
	{
		const T1& t1;
		const T2& t2;
		constexpr size_t size() const { return std::min(t1.size(), t2.size()); }
		auto operator[](typename T2::size_type i) const { return t1[i] - t2[i]; }
	};
	template<typename T1, typename T2>
	auto operator+(const T1& t1, const T2& t2)  -> VecSum<T1, T2> { return { t1, t2 }; }
	template<typename T1, typename T2>
	auto operator-(const T1& t1, const T2& t2)  -> VecDiff<T1, T2> { return { t1, t2 }; }
}

template <size_t DIM, typename T> struct vec {
	using value_type = T;
   constexpr vec() { for (size_t i=DIM; i--; data[i] = T()); }
 //  template<typename ... Args>
//   constexpr vec(Args&& ... args) : data{ std::forward<Args>(args)... } {}
   template<size_t UN, typename U>
   constexpr vec(const vec<UN, U>& v) : data{ convert_array<U,UN,T,DIM>(v.data) } { std::round }

   constexpr      T& operator[](const size_t i)       {  return data[i]; }
	constexpr const T& operator[](const size_t i) const {  return data[i]; }
	std::array<value_type, DIM> data;
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T> struct vec<2,T> {
	using value_type = T;
	constexpr vec() : x(T()), y(T()) {}

	template<typename U> constexpr vec(U X, U Y) : x(priv::convert<U, T>(X)), y(priv::convert<U, T>(Y)) {}
	//template <class U, typename> constexpr vec<2, T>(const vec<2, U> &v);

	template <class U>
	constexpr vec(const vec<2, U> &v) : x(priv::convert<U, T>(v.x)), y(priv::convert<U, T>(v.y)) {}

	constexpr      T& operator[](const size_t i) { return data[i]; }
	constexpr const T& operator[](const size_t i) const { return data[i]; }
	union {
		struct { T x, y; };
		std::array<value_type, 2> data;
	};
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T> struct vec<3,T> {
	using value_type = T;
	constexpr vec() : x(T()), y(T()), z(T()) {}
	constexpr vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
	template <class U> 
	constexpr vec<3,T>(const vec<3,U> &v) : x(static_cast<const T>(v.x)), y(static_cast<const T>(v.y)),  z(static_cast<const T>(v.z)) {}
	constexpr      T& operator[](const size_t i) { return data[i]; }
	constexpr const T& operator[](const size_t i) const { return data[i]; }
	constexpr T norm() { return std::sqrt(x*x+y*y+z*z); }
	constexpr vec<3,T> & normalize(T l=1) { *this = (*this)*(l/norm()); return *this; }
	union {
		struct {
			union {
				vec<2, T> xy;
				struct { T x; T y; };
			};
			T z;
		};
		std::array<value_type, 3> data;
	};
};
struct color_rgba {

};
template <size_t DIM, typename T> struct color {
	using value_type = T;
	static constexpr size_t dim = DIM;
	constexpr color() { for (size_t i = DIM; i--; data[i] = T()); }
	//  template<typename ... Args>
	//   constexpr vec(Args&& ... args) : data{ std::forward<Args>(args)... } {}
	template<size_t UN, typename U>
	constexpr color(const color<UN, U>& v) : data{ convert_array<U,UN,T,DIM>(v.data) } {}

	constexpr      T& operator[](const size_t i) { return data[i]; }
	constexpr const T& operator[](const size_t i) const { return data[i]; }
	std::array<T, DIM> data;
};

// this is indexed color
template<> struct color<4, uint8_t> {
	using value_type = uint8_t;
	static constexpr size_t dim = 4;
	constexpr color() : r(0),g(0),b(0), a(0xFF) {}
	constexpr color(value_type R, value_type G, value_type B, value_type A=0xFF) : r(R), g(G), b(B), a(A) {}
	constexpr      value_type& operator[](const size_t i) { return data[i]; }
	constexpr const value_type& operator[](const size_t i) const { return data[i]; }
	union {
		uint32_t number;
		value_type data[4];
		struct {
			value_type r;
			value_type g;
			value_type b;
			value_type a;
		};
	};
};


template <size_t DIM, typename T,typename F>
constexpr static inline color<DIM,T> operator*(const color<DIM, T>& c, F intensity) {
	color<DIM, T> ret;
	intensity = (intensity > F{ 1 } ? F{ 1 } : (intensity < F{ 0 } ? F{ 0 } : intensity));
	for (size_t i = 0; i<DIM; i++) ret.data[i] = priv::convert<F,T>(c.data[i] * intensity);
	return ret;
}
using color_t = color<4, uint8_t>;

#if 0
template <typename T> struct vec<4, T> {
	constexpr vec() : x(T()), y(T()), z(T()) , w(T()) {}
	constexpr vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z),w(W) {}
	template <class U> 
	constexpr vec<4, T>(const vec<3, U> &v) : x(static_cast<const T>(v.x)), y(static_cast<const T>(v.y)) , z(static_cast<const T>(v.z)), w(static_cast<const T>(v.w)) {}
	constexpr T& operator[](const size_t i) { assert(i<DIM); return data[i]; }
	constexpr const T& operator[](const size_t i) const { assert(i<DIM); return data[i]; }
	constexpr float norm() { return std::sqrt(x*x + y*y + z*z + w*w); }
	constexpr vec<4, T> & normalize(T l = 1) { *this = (*this)*(l / norm()); return *this; }
	union {
		struct {
			union {
				struct {
					union {
						vec<2, T> xy;
						struct { T x; T y };
					};
					T z;
				};
				vec<3, T> xyz;
			};
			T w;
		};
		T data[4];
	};
};

#endif
/////////////////////////////////////////////////////////////////////////////////

template<size_t DIM,typename T> constexpr static inline T operator*(const vec<DIM,T>& lhs, const vec<DIM,T>& rhs) {
    T ret = T();
    for (size_t i=DIM; i--; ret+=lhs[i]*rhs[i]);
    return ret;
}


template<size_t DIM,typename T> constexpr static inline vec<DIM,T> operator+(vec<DIM,T> lhs, const vec<DIM,T>& rhs) {
    for (size_t i=DIM; i--; lhs[i]+=rhs[i]);
    return lhs;
}

template<size_t DIM,typename T> constexpr static inline vec<DIM,T> operator-(vec<DIM,T> lhs, const vec<DIM,T>& rhs) {
    for (size_t i=DIM; i--; lhs[i]-=rhs[i]);
    return lhs;
}

template<size_t DIM,typename T,typename U> constexpr static inline vec<DIM,T> operator*(vec<DIM,T> lhs, const U& rhs) {
    for (size_t i=DIM; i--; lhs[i]*=rhs);
    return lhs;
}

template<size_t DIM,typename T,typename U> constexpr static inline vec<DIM,T> operator/(vec<DIM,T> lhs, const U& rhs) {
    for (size_t i=DIM; i--; lhs[i]/=rhs);
    return lhs;
}

template<size_t LEN,size_t DIM,typename T> constexpr static inline vec<LEN,T> embed(const vec<DIM,T> &v, T fill=1) {
    vec<LEN,T> ret;
    for (size_t i=LEN; i--; ret[i]=(i<DIM?v[i]:fill));
    return ret;
}

template<size_t LEN,size_t DIM, typename T> constexpr static inline vec<LEN,T> proj(const vec<DIM,T> &v) {
    vec<LEN,T> ret;
    for (size_t i=LEN; i--; ret[i]=v[i]);
    return ret;
}

template <typename T> constexpr static inline vec<3,T> cross(vec<3,T> v1, vec<3,T> v2) {
    return vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, vec<DIM,T>& v) {
    for(unsigned int i=0; i<DIM; i++) {
        out << v[i] << " " ;
    }
    return out ;
}

/////////////////////////////////////////////////////////////////////////////////

template<size_t DIM,typename T> struct dt {
    static T det(const mat<DIM,DIM,T>& src) {
        T ret=0;
        for (size_t i=DIM; i--; ret += src[0][i]*src.cofactor(0,i));
        return ret;
    }
};

template<typename T> struct dt<1,T> {
    static T det(const mat<1,1,T>& src) {
        return src[0][0];
    }
};

/////////////////////////////////////////////////////////////////////////////////

template<size_t DimRows,size_t DimCols,typename T> class mat {

public:
	using value_type = T;
	using row_type = vec<DimCols, value_type>;
	static constexpr size_t dim_rows = DimRows;
	static constexpr size_t dim_cols = DimCols;
    mat() {}

    vec<DimCols,T>& operator[] (const size_t idx) {
        assert(idx<DimRows);
        return rows[idx];
    }

    const vec<DimCols,T>& operator[] (const size_t idx) const {
        assert(idx<DimRows);
        return rows[idx];
    }

    vec<DimRows,T> col(const size_t idx) const {
        assert(idx<DimCols);
        vec<DimRows,T> ret;
        for (size_t i=DimRows; i--; ret[i]=rows[i][idx]);
        return ret;
    }

    void set_col(size_t idx, vec<DimRows,T> v) {
        assert(idx<DimCols);
        for (size_t i=DimRows; i--; rows[i][idx]=v[i]);
    }

    static mat<DimRows,DimCols,T> identity() {
        mat<DimRows,DimCols,T> ret;
        for (size_t i=DimRows; i--; )
            for (size_t j=DimCols;j--; ret[i][j]=(i==j));
        return ret;
    }

    T det() const {
        return dt<DimCols,T>::det(*this);
    }

    mat<DimRows-1,DimCols-1,T> get_minor(size_t row, size_t col) const {
        mat<DimRows-1,DimCols-1,T> ret;
        for (size_t i=DimRows-1; i--; )
            for (size_t j=DimCols-1;j--; ret[i][j]=rows[i<row?i:i+1][j<col?j:j+1]);
        return ret;
    }

    T cofactor(size_t row, size_t col) const {
        return get_minor(row,col).det()*((row+col)%2 ? -1 : 1);
    }

    mat<DimRows,DimCols,T> adjugate() const {
        mat<DimRows,DimCols,T> ret;
        for (size_t i=DimRows; i--; )
            for (size_t j=DimCols; j--; ret[i][j]=cofactor(i,j));
        return ret;
    }

    mat<DimRows,DimCols,T> invert_transpose() {
        mat<DimRows,DimCols,T> ret = adjugate();
        T tmp = ret[0]*rows[0];
        return ret/tmp;
    }

    mat<DimRows,DimCols,T> invert() {
        return invert_transpose().transpose();
    }

    mat<DimCols,DimRows,T> transpose() {
        mat<DimCols,DimRows,T> ret;
        for (size_t i=DimCols; i--; ret[i]=this->col(i));
        return ret;
    }
private:
	row_type rows[DimRows];
};

/////////////////////////////////////////////////////////////////////////////////

template<size_t DimRows,size_t DimCols,typename T> vec<DimRows,T> operator*(const mat<DimRows,DimCols,T>& lhs, const vec<DimCols,T>& rhs) {
    vec<DimRows,T> ret;
    for (size_t i=DimRows; i--; ret[i]=lhs[i]*rhs);
    return ret;
}

template<size_t R1,size_t C1,size_t C2,typename T>mat<R1,C2,T> operator*(const mat<R1,C1,T>& lhs, const mat<C1,C2,T>& rhs) {
    mat<R1,C2,T> result;
    for (size_t i=R1; i--; )
        for (size_t j=C2; j--; result[i][j]=lhs[i]*rhs.col(j));
    return result;
}

template<size_t DimRows,size_t DimCols,typename T>mat<DimCols,DimRows,T> operator/(mat<DimRows,DimCols,T> lhs, const T& rhs) {
    for (size_t i=DimRows; i--; lhs[i]=lhs[i]/rhs);
    return lhs;
}

template <size_t DimRows,size_t DimCols,class T> std::ostream& operator<<(std::ostream& out, mat<DimRows,DimCols,T>& m) {
    for (size_t i=0; i<DimRows; i++) out << m[i] << std::endl;
    return out;
}

/////////////////////////////////////////////////////////////////////////////////

typedef vec<2,  float> Vec2f;
typedef vec<2,  int>   Vec2i;
typedef vec<3,  float> Vec3f;
typedef vec<3,  int>   Vec3i;
typedef vec<4,  float> Vec4f;
typedef mat<4,4,float> Matrix;
#endif //__GEOMETRY_H__

