#ifndef _COMMON_H_
#define _COMMON_H_


#include "stm32f7xx_hal.h"
#include <cstdint>

#include <type_traits>
#include <cassert>

namespace stm32 {
	template<typename T,uintptr_t _ADDRESS_BASE>
	struct reg_set {
		using type = T;
		static constexpr uintptr_t ADDRESS_BASE = _ADDRESS_BASE;
		volatile type& operator=(const type v)  {
			*reinterpret_cast<volatile type*>(ADDRESS_BASE) = v;
			return *reinterpret_cast<volatile type*>(ADDRESS_BASE);
		}
		constexpr operator type() const{
			return *reinterpret_cast<volatile type*>(ADDRESS_BASE);
		}
		constexpr operator volatile type&() {
			return *reinterpret_cast<volatile type*>(ADDRESS_BASE);
		}
	};
	template<typename _ENUM> class enum_helper {
		using enum_type = std::decay_t<_ENUM>;
		static_assert(std::is_enum<enum_type>::value,"Needs to be an enum");
		using utype = typename std::underlying_type<enum_type>::type;
		utype _value;

		template<typename T,typename E=void>
		struct static_check : std::false_type {};
		template<typename T>
		struct static_check<T, std::enable_if<
			std::is_same<std::decay_t<_ENUM>,enum_type>::value ||
			std::is_arithmetic<std::decay_t<_ENUM>>::value>> : std::true_type {

		};

	public:
		constexpr enum_helper() : _value{utype{}} {}
		constexpr enum_helper(enum_type v) : _value{static_cast<utype>(v)} {}
		constexpr enum_helper(utype v) : _value{v} {}
		operator enum_type() const { return static_cast<enum_type>(_value); }

		template<typename T, typename = std::enable_if<static_check<T>::value>>
		void set(const T& v) { _value |= static_cast<utype>(v); }
		template<typename T, typename = std::enable_if<static_check<T>::value>>
		void clear(const T& v) { _value &= ~static_cast<utype>(v); }
		template<typename T, typename = std::enable_if<static_check<T>::value>>
		bool has(const T& v) const { return  (_value & static_cast<utype>(v)) == static_cast<utype>(v); }

		template<typename T>
		enum_helper& operator|=(T&& v) { set(v); return *this; }
		template<typename T, typename = std::enable_if<static_check<T>::value>>
		bool operator==(const T& v) const { return _value == static_cast<utype>(v); }

		template<typename T>
		bool operator==(const T&& v) const { return !(*this == v); }
	};

}

#endif
