#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include "tiny_common.h"

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


namespace priv {
	template<size_t LAST_BIT>
	struct bit_type {
		using type = typename std::conditional<LAST_BIT == 0, void,
			typename std::conditional<LAST_BIT <= 8, uint8_t,
			typename std::conditional<LAST_BIT <= 16, uint16_t,
			typename std::conditional<LAST_BIT <= 32, uint32_t,
			typename std::conditional<LAST_BIT <= 64, uint64_t,
			void>::type>::type>::type>::type>::type;
	};
	template<size_t SHIFT, size_t SIZE = 1>
	class bit_traits {
		using value_type = typename bit_type<SHIFT + SIZE>::type;
		static constexpr size_t size = SIZE;
		static constexpr size_t shift = SHIFT;
	};

	template<typename T, size_t SHIFT, size_t SIZE>
	class bit_field {
	public:
		static_assert(sizeof(typename bit_type<SHIFT + SIZE>::type) <= sizeof(T), "Type to small");
		using value_type = T;
		static constexpr size_t size = SIZE;
		static constexpr size_t shift = SHIFT;
		static constexpr value_type mask = (1u << shift) - 1u;
		static constexpr value_type set_bits(value_type value) { return ((value << shift) & mask); }
		static constexpr value_type clear_bits(value_type value) { return (value & ~(mask << shift)); }
		static constexpr value_type get_bits(value_type value) { return  (value_ >> shift) & mask; }
		template <class T2>
		bit_field &operator=(T2 value) {
			value_ = clear_bits(value_) | set_bits(value); ;
			return *this;
		}
		operator value_type() const { return get_bits(value_); }
		explicit operator bool() const { return value_ & (mask << shift); }
		bit_field &operator++() { return *this = *this + 1; }
		value_type operator++(int) { T r = *this; ++*this; return r; }
		bit_field &operator--() { return *this = *this - 1; }
		value_type operator--(int) { T r = *this; --*this; return r; }
	private:
		value_type& value_;
	};

	template<typename TT>
	class bit_field<TT,0U, 0U> {
	public:
		using value_type = TT;
		static constexpr size_t size = 0;
		static constexpr size_t shift = 0;
		
		template<typename T>static constexpr T set_bits(T value) { return value; }
		template<typename T>static constexpr T clear_bits(T value) { return value; }
		template<typename T>static constexpr T get_bits(T value) { return 0; }

		template <class T2>
		bit_field &operator=(T2 value) { return *this; }

		operator value_type() const { return 0; }
		explicit operator bool() const { return false; }
		bit_field &operator++() { return *this; }
		value_type operator++(int) { return *this; }
		bit_field &operator--() { return *this; }
		value_type operator--(int) { return *this; }
	};

};

template<size_t SHIFT, size_t SIZE>
using color_component = priv::bit_traits<SHIFT, SIZE>;




template <typename RED_COMP, typename GREEN_COMP, typename BLUE_COMP, typename ALPHA_COMP = color_component<0,0>>
class color {
	static constexpr size_t total_bits = RED_COMP::size + GREEN_COMP::size + BLUE_COMP::size + ALPHA_COMP::size;
	using value_type = priv::bit_type<total_bits>;
	using red_type = priv::bit_field<value_type, RED_COMP::shift, RED_COMP::size>;
	using green_type = priv::bit_field<value_type, GREEN_COMP::shift, GREEN_COMP::size>;
	using blue_type = priv::bit_field<value_type, BLUE_COMP::shift, BLUE_COMP::size>;
	using alpha_type = priv::bit_field<value_type, ALPHA_COMP::shift, ALPHA_COMP::size>;
	
	const red_type red() const { return _value; }
	const green_type green() const { return _value; }
	const blue_type blue() const { return _value; }
	const blue_type alpha() const { return _value; }
	red_type red()  { return _value; }
	green_type green()  { return _value; }
	blue_type blue()  { return _value; }
	blue_type alpha()  { return _value; }
	constexpr bool has_alpha() const { return alpha_type::size != 0; }
	constexpr size_t size() const { return sizeof(value_type); }
	const uint8_t* data() const { return reinterpret_cast<uint8_t*>(&_value); }

	color() : : _value(alpha_type::set(td::numeric_limits<ALPHA>::max())) {}
	template<typename RED, typename GREEN, typename BLUE,typename ALPHA>
	color(RED r, GREEN g, BLUE b, ALPHA a = std::numeric_limits<ALPHA>::max()) : _value(red_type::set(r) | green_type::set(g) | blue_type::set(b) | alpha_type::set(a)) {}
	template<typename RED, typename GREEN, typename BLUE, typename ALPHA>
	color(const color<RED,GREEN,BLUE,ALPHA>& copy) : _value(red_type::set(copy.red()) | green_type::set(copy.green()) | blue_type::set(copy.blue()) | alpha_type::set(copy.alpha())) {}
private:
	value_type _value;
};

// this builds a 16 bit color value in 5.5.5 format (1-bit alpha mode)
#define _RGB16BIT555(r,g,b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))

// this builds a 16 bit color value in 5.6.5 format (green dominate mode)
#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))

// this builds a 24 bit color value in 8.8.8 format 
#define _RGB24BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) )

// this builds a 32 bit color value in A.8.8.8 format (8-bit alpha mode)
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

using color_rgb1555 = color<color_component<10, 5>, color_component<5, 5>, color_component<0, 5>, color_component<15, 1>>;
using color_rgb565 = color<color_component<11, 5>, color_component<5, 6>, color_component<0, 5>>;
using color_rgb888 = color<color_component<16, 8>, color_component<8, 8>, color_component<0, 8>>;
using color_rgb8888 = color<color_component<16, 8>, color_component<8, 8>, color_component<0, 8>, color_component<24, 8>>;


template <typename RED_COMP, typename GREEN_COMP, typename BLUE_COMP, typename ALPHA_COMP,typename F>
constexpr static inline color<RED_COMP, GREEN_COMP, BLUE_COMP, ALPHA_COMP> 
	operator*(const color<RED_COMP, GREEN_COMP, BLUE_COMP, ALPHA_COMP> & c, F intensity) {
	using color_type = color<RED_COMP, GREEN_COMP, BLUE_COMP, ALPHA_COMP>;
	using value_type = typename color_type::value_type;
	intensity = (intensity > F{ 1 } ? F{ 1 } : (intensity < F{ 0 } ? F{ 0 } : intensity));
	color_type ret(priv::convert<F, value_type>(c.red() * intensity), 
		priv::convert<F, value_type>(c.green() * intensity), 
		priv::convert<F, value_type>(c.blue() * intensity), 
		c.alpha());
	return ret;
}

using color_t = color_rgb8888;

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

