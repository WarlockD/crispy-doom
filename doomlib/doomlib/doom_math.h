#pragma once
#include "doom_common.hpp"
#include <cmath>


namespace doom_cpp {

	class fixed_t : public util::hash_interface {

	public:
		using value_type = int32_t;
		static constexpr size_t FRACBITS = 16;
		static constexpr size_t FRACHALF = (FRACBITS / 2);
		static constexpr value_type FRACUNIT = (1 << FRACBITS);
		static constexpr value_type FIXED_ONE = (1 << FRACBITS);
		static constexpr value_type FIXED_HALF = (1 << (FRACBITS - 1));

		static constexpr value_type max_value = std::numeric_limits<value_type>::max();
		static constexpr value_type min_value = std::numeric_limits<value_type>::min();
		static constexpr value_type FIXED_NAN = max_value;
		static constexpr value_type FIXED_DIV_ZERO = (FIXED_ONE - 1); // max value when using div the demoniator may be and not considered zero

		inline static constexpr value_type fixed_mult(value_type a, value_type b) { return (a >> FRACHALF) * (b >> FRACHALF); }
		// numerator, demoniator
		inline static constexpr value_type fixed_qdiv(value_type a, value_type b) { return (a / (b >> FRACHALF)) << FRACHALF; }
		// returns nan if demoniator is close to zero
		inline static constexpr value_type fixed_div(value_type a, value_type b) { return (b >> FRACHALF) == 0 ? FIXED_NAN : fixed_qdiv(a, b); }
		inline static constexpr value_type fixed_fpart(value_type a) { return a & (FIXED_ONE - 1); } // Return the fractional part of 'a'.
		inline static constexpr value_type fixed_rfpart(value_type a) { return FIXED_ONE - fixed_fpart(a); } // Return one minus the fractional part of 'a'.
		inline static constexpr value_type fixed_ipart(value_type a) { return a - fixed_fpart(a); } // Returns the integer (whole) part of 'a'.
		inline static constexpr value_type fixed_round(value_type a) { return fixed_ipart(a + FIXED_HALF); } // Returns the integer (whole) part of 'a'.

		inline static constexpr value_type fixed_fromfloat(float a) { return value_type(a * FIXED_ONE); }
		inline static constexpr float fixed_tofloat(value_type a) { return float(a) / FIXED_ONE; }
		static constexpr value_type fixed_trig[91] = {
			65536,  65526,  65496,  65446,  65376,
			65287,  65177,  65048,  64898,  64729,
			64540,  64332,  64104,  63856,  63589,
			63303,  62997,  62672,  62328,  61966,
			61584,  61183,  60764,  60326,  59870,
			59396,  58903,  58393,  57865,  57319,
			56756,  56175,  55578,  54963,  54332,
			53684,  53020,  52339,  51643,  50931,
			50203,  49461,  48703,  47930,  47143,
			46341,  45525,  44695,  43852,  42995,
			42126,  41243,  40348,  39441,  38521,
			37590,  36647,  35693,  34729,  33754,
			32768,  31772,  30767,  29753,  28729,
			27697,  26656,  25607,  24550,  23486,
			22415,  21336,  20252,  19161,  18064,
			16962,  15855,  14742,  13626,  12505,
			11380,  10252,   9121,   7987,   6850,
			5712,   4572,   3430,   2287,   1144,
			0
		};
		inline static constexpr value_type fixed_cos(int degrees) {// cosine accept non value 0-360

			while (degrees >= 360) degrees -= 360;
			while (degrees < 0) degrees += 360;
			if (degrees < 90) return(fixed_trig[degrees]);
			else if (degrees <= 180) return(-fixed_trig[180 - degrees]);
			else if (degrees <= 270) return(-fixed_trig[degrees - 180]);
			else return(fixed_trig[360 - degrees]);
		}
		inline static constexpr value_type fixed_sin(int degrees) { return fixed_cos(90 - degrees); }// cosine accept non value 0-360
		/* Based on 'Doing It Fast'
		By Bob Pendleton, 1993, 1997
		http://gameprogrammer.com/4-fixed.html */
		inline static constexpr value_type fixed_sqrt(value_type i) {
			if (i < FIXED_ONE) return 0;
			value_type root = 0;
			value_type next = i >> 2;
			do {
				root = next;
				next = (next + fixed_div(i, next)) >> 1;
			} while (root != next);
			return root;
		}


		template<typename T>
		constexpr static inline T abs(T v) { return v < 0 ? -v : v; }
		constexpr static inline value_type FixedMul(value_type l, value_type r) {
			return static_cast<uint32_t>((static_cast<int64_t>(l) * static_cast<int64_t>(r)) >> FRACBITS);
		}
		constexpr static inline value_type FixedDiv(value_type l, value_type r) {
			return ((abs(l) >> 14) >= abs(r)) ?
				((l*r) < 0) ? min_value : max_value
				: (static_cast<int64_t>(l) << 16) / r;
		}
		constexpr fixed_t() : _val{} {}
		constexpr fixed_t(value_type val) : _val{ val << FRACBITS } {}
		constexpr fixed_t(float val) : _val{ fixed_fromfloat(val) } {}
		constexpr operator int32_t() const { return fixed_round(_val) >> FRACBITS; }
		constexpr operator float() const { return fixed_tofloat(_val); }
		constexpr operator bool() const { return  (_val >> FRACHALF) != 0; }
		fixed_t& operator*=(const fixed_t&r) { _val = fixed_mult(_val, r._val); return *this; }
		fixed_t& operator/=(const fixed_t&r) { _val = fixed_div(_val, r._val); return *this; }
		fixed_t& operator+=(const fixed_t&r) { _val = _val+ r._val; return *this; }
		fixed_t& operator-=(const fixed_t&r) { _val = _val-r._val; return *this; }
		fixed_t operator*(const fixed_t&r) const { fixed_t ret = *this; ret *= r; return *this; }
		fixed_t operator/(const fixed_t&r) const { fixed_t ret = *this; ret /= r; return *this; }
		fixed_t operator+(const fixed_t&r) const { fixed_t ret = *this; ret += r; return *this; }
		fixed_t operator-(const fixed_t&r) const { fixed_t ret = *this; ret -= r; return *this; }
		size_t hash() const { return _val; }
		bool operator==(const fixed_t&r) const { return _val == r._val; }
		bool operator!=(const fixed_t&r) const { return _val != r._val; }
		bool operator>=(const fixed_t&r) const { return _val >= r._val; }
		bool operator<=(const fixed_t&r) const { return _val <= r._val; }
		bool operator<(const fixed_t&r) const { return _val < r._val; }
		bool operator>(const fixed_t&r) const { return _val > r._val; }
		inline fixed_t round() const noexcept { return fixed_round(_val); }
		inline fixed_t ceil() const noexcept { return _val < value_type{} || fixed_ipart(_val) == _val ? fixed_ipart(_val + FIXED_ONE):  fixed_ipart(_val - FIXED_ONE);}
		inline fixed_t floor() const noexcept { return _val < value_type{} || fixed_ipart(_val) == _val ? fixed_ipart(_val + FIXED_ONE) : fixed_ipart(_val - FIXED_ONE); }
		inline fixed_t trunc() const noexcept { return fixed_ipart(_val); }
	private:
		value_type _val;
	};
	static constexpr size_t FINEANGLES = 8192;
	static constexpr size_t FINEMASK = (FINEANGLES - 1);
	static constexpr size_t ANGLETOFINESHIFT = 19; 	// 0x100000000 to 0x2000


	// Effective size is 10240.
	extern const fixed_t finesine[5 * FINEANGLES / 4];

	// Re-use data, is just PI/2 pahse shift.
	extern const fixed_t *finecosine;

	// Effective size is 4096.
	extern const fixed_t finetangent[FINEANGLES / 2];

	// Gamma correction tables.
	extern const uint8_t gammatable[5][256];

	// Binary Angle Measument, BAM.
	static constexpr size_t ANG45 = 0x20000000;
	static constexpr size_t ANG90 = 0x40000000;
	static constexpr size_t ANG180 = 0x80000000; 	
	static constexpr size_t ANG270 = 0xc0000000;
	static constexpr size_t ANG_MAX = 0xffffffff;
	static constexpr size_t ANG1 = (ANG45 / 45);
	static constexpr size_t ANG60 = (ANG180 / 3);

	// Heretic code uses this definition as though it represents one 
	// degree, but it is not!  This is actually ~1.40 degrees.
	static constexpr size_t ANG1_X = 0x01000000;
	static constexpr size_t SLOPERANGE = 2048;
	static constexpr size_t SLOPEBITS = 11;
	static constexpr size_t DBITS = (fixed_t::FRACBITS - SLOPEBITS);

	typedef unsigned angle_t;


	// Effective size is 2049;
	// The +1 size is to handle the case when x==y
	//  without additional checking.
	extern const angle_t tantoangle[SLOPERANGE + 1];


	// Utility function,
	//  called by R_PointToAngle.
	int SlopeDiv(unsigned int num, unsigned int den);
}
namespace std {
	static inline doom_cpp::fixed_t round(doom_cpp::fixed_t v) noexcept { return v.round(); }
	static inline doom_cpp::fixed_t ceil(doom_cpp::fixed_t v) noexcept { return v.ceil(); }
	static inline doom_cpp::fixed_t floor(doom_cpp::fixed_t v) noexcept { return v.floor(); }
	static inline doom_cpp::fixed_t trunc(doom_cpp::fixed_t v) noexcept { return v.trunc(); }
};
