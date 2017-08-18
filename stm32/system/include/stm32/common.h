#ifndef _COMMON_H_
#define _COMMON_H_


#include "stm32f7xx_hal.h"
#include <cstdint>

#include <type_traits>
#include <cassert>
#include <algorithm>
#include <array>
#include <utility>
#include <tuple>
#include <cassert>

namespace stm32 {


	namespace priv {
		template<uintptr_t addr, typename type = uint32_t>
		struct ioregister_t
		{
			inline type operator=(type value) { *(volatile type*)addr = value; return value; }
			inline void operator|=(type value) { *(volatile type*)addr |= value; }
			inline void operator&=(type value) { *(volatile type*)addr &= value; }
			inline operator type() { return *(volatile type*)addr; }
			template<uintptr_t _addr, typename _type>
			constexpr bool operator==(const ioregister_t<_addr,_type>& other) const { return std::is_same<type,_type>::value && _addr == addr; }
			template<uintptr_t _addr, typename _type>
			constexpr bool operator!=(const ioregister_t<_addr,_type>& other) const {return !(*this == other); }
		};
		template<uintptr_t addr, class T>
		struct iostruct_t
		{
			volatile T* operator->() { return (volatile T*)addr; }
		};
		struct CR_BASE {};
		struct CR_VALID : CR_BASE { static constexpr bool cr_valid = true; constexpr bool valid() const { return cr_valid; } };
		struct CR_INVALID : CR_BASE { static constexpr bool cr_valid = false; constexpr bool valid() const { return cr_valid; } };
		template<typename T>
		struct is_cr_base : std::conditional_t<std::is_base_of<CR_BASE,T>::value,std::true_type, std::false_type> {};
		template<typename T>
		struct is_cr_valid : std::conditional_t<is_cr_base<T>::value && T::cr_valid,std::true_type, std::false_type> {};

		struct CR_REG_BASE : CR_BASE {
			CR_REG_BASE() = delete;
			constexpr CR_REG_BASE(uint32_t offset) : _offset(offset) {}
			constexpr uint32_t offset() const { return _offset; }
		private:
			uint32_t _offset; // if we made this a volatile uint32_t, then this class would stop being constexpr
		};
		template<typename _BASE_REG_TYPE, size_t _REG_CR_OFFSET>
		struct CR_REG : public CR_REG_BASE, CR_VALID {
			//constexpr static uintptr_t REG_ADDRESS_BASE = _REG_ADDRESS_BASE;
			using reg_type = _BASE_REG_TYPE;
			constexpr static size_t REG_CR_OFFSET = _REG_CR_OFFSET;
			//constexpr static uintptr_t REG_CR_ADDRESS_BASE = REG_ADDRESS_BASE + REG_CR_OFFSET;
			//static_assert((REG_CR_ADDRESS_BASE&3) ==0, "Not word alligned");
			//using ioregister = ioregister_t<REG_CR_ADDRESS_BASE>;
			constexpr CR_REG() : CR_REG_BASE(REG_CR_OFFSET) { }
			static volatile uint32_t* cr_from_type(reg_type* reg){ return (volatile uint32_t*)( ((volatile uint8_t*)reg) + REG_CR_OFFSET); }
			static volatile uint32_t* cr_from_base(volatile uint32_t* reg){ return (volatile uint32_t*)( ((volatile uint8_t*)reg) + REG_CR_OFFSET); }
			static const volatile uint32_t* cr_from_type(const reg_type* reg){ return (const volatile uint32_t*)( ((const volatile uint8_t*)reg) + REG_CR_OFFSET); }
			static const volatile uint32_t* cr_from_base(const volatile uint32_t* reg){ return (const volatile uint32_t*)( ((const volatile uint8_t*)reg) + REG_CR_OFFSET); }
			inline static uint32_t get(const volatile uint32_t* reg) { return *cr_from_base(reg); }
			inline static void set(volatile uint32_t* reg, uint32_t value) {  *cr_from_base(reg) = value; }


			inline static uint32_t get(const reg_type* reg) { return get(cr_from_type(reg)); }
			inline static void set(reg_type* reg, uint32_t value) { set(cr_from_type(reg),value); }
		};
		template<typename T>
		struct is_cr_reg : std::conditional_t<std::is_base_of<CR_REG_BASE,T>::value,std::true_type, std::false_type> {};

		template<typename REG1, typename REG2>
		struct is_same_reg : std::conditional_t<std::is_same<std::decay_t<REG1>,std::decay_t<REG2>>::value,std::true_type, std::false_type> {
			static_assert(is_cr_reg<REG1>::value, "First template avlue not a reg");
			static_assert(is_cr_reg<REG2>::value, "Second template avlue not a reg");
		};
		struct CR_REG_MASK_BASE {
			CR_REG_MASK_BASE() = delete;
			constexpr CR_REG_MASK_BASE(uint32_t mask) : _mask(mask) {}
			constexpr uint32_t mask() const { return _mask; }
		private:
			uint32_t _mask;
		};

		template<typename REG, uint32_t _MASK>
		struct CR_REG_MASK : CR_REG_MASK_BASE  {
			using reg_type = REG;
			static constexpr uint32_t cr_mask = _MASK;
			CR_REG_MASK() : CR_REG_MASK_BASE(cr_mask) {}
		};
		template<typename T>
		struct is_cr_reg_mask : std::conditional_t<std::is_base_of<CR_REG_MASK_BASE,T>::value,std::true_type, std::false_type> {};

		template<typename ENUM> struct CR_INFO : CR_INVALID {
			using enum_type = void;
			using mask_type = CR_INVALID;
			using reg_type = CR_INVALID;
		};
		template<typename ENUM, typename REG>
		struct enum_match_reg {
			using enum_type = std::decay_t<ENUM>;
			using reg_type = std::decay_t<REG>;
			static_assert(std::is_enum<enum_type>::value, "Not an enum");
			static_assert(is_cr_valid<CR_INFO<enum_type>>::value, "Enum is not assigned to a reg");
			static_assert(is_cr_reg<reg_type>::value, "Reg is not valid");
			using info = CR_INFO<enum_type>;
			static constexpr bool value = is_same_reg<typename info::reg_type, reg_type>::value;
			static constexpr uint32_t mask = value ? info::mask_type::cr_mask : 0;
		};
#define CR_MASK_HELPER_STOP_ON_INVALID
		template<typename _REG>
		struct CR_MASK_HELPER {
			static_assert(is_cr_reg<_REG>::value, "First template avlue not a reg");
			using reg= _REG;
			using reg_type = typename reg::reg_type;

			template<typename ENUM>
			static constexpr uint32_t mask(ENUM&& v) {
				(void)v;
				using info =  enum_match_reg<ENUM,reg>;
				return info::value ? info::mask : 0;
			}
			template<typename ENUM, typename ... Args>
			static constexpr uint32_t mask(ENUM&& v, Args&&... args) { return mask(v) | mask(std::forward<Args>(args)...); }

			template<typename ENUM>
			static constexpr uint32_t flag(ENUM&& v) {
				using info =  enum_match_reg<ENUM,reg>;
				return info::value ? static_cast<uint32_t>(v) : 0;
			}
			template<typename ENUM, typename ... Args>
			static constexpr uint32_t flag(ENUM&& v, Args&&... args) { return flag(v) | flag(std::forward<Args>(args)...); }

			template<typename ... Args>
			static inline void set_many(reg_type* addr, Args&& ... args) {
				uint32_t tmp = reg::get(addr);
				tmp &= ~mask(std::forward<Args>(args)...);
				tmp |= flag(std::forward<Args>(args)...);
				reg::set(addr,tmp);
			}
			template<typename ENUM>
			static inline void set_single(reg_type* addr, const ENUM& v) {
				using info =  enum_match_reg<ENUM,reg>;
				uint32_t tmp = reg::get(addr);
				tmp &= ~info::mask;
				tmp |= static_cast<uint32_t>(v);
				reg::set(addr,tmp);
			}
			template<typename ENUM>
			static inline ENUM get_single(const reg_type* addr) {
				using info =  enum_match_reg<ENUM,reg>;
				return static_cast<ENUM>(reg::get(addr) & info::mask);
			}

		};


	};
#define CREATE_CR_REG(NAME, REG_TYPE, OFFSET) using NAME = priv::CR_REG<REG_TYPE,OFFSET>;
#define CREATE_CR_MASK(REG,ENUM,MASK) namespace priv { template<> struct CR_INFO<ENUM> : CR_VALID { using reg_type = REG; using mask_type = CR_REG_MASK<REG,(MASK)>; }; }


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
	template<typename T, bool _FORWARD>
		class circle_iterator {
			static constexpr bool __is_const = std::is_const<T>::value;
			using __value_type = std::remove_cv<T>;
			using __const_value_type =  std::add_const_t<__value_type>;

		public:
			using self_type = circle_iterator<T,_FORWARD>;
			using value_type = std::conditional_t<__is_const, __const_value_type, __value_type>;
			using pointer = value_type*;
			using reference = value_type&;
			using const_pointer = const __value_type*;
			using const_reference = const __value_type&;
			using difference_type = std::ptrdiff_t;
			using size_type = std::size_t;
		private:
			// the while loop is a hack  I don't expect it to run more than once, but just in case
			// also this all should optimize cleanly?
			inline void __forward(difference_type n) { _current+=n; while(_current >= _end) _current -= _end-_begin;   }
			inline void __backard(difference_type n) { _current-=n; while(_current < _begin) _current += _end-_begin;   }
			inline void __forward() { if(++_current == _end) _current = _begin;   }
			inline void __backard() { if(_current == _begin) _current = _end; --_current;  }
			inline void _forward(difference_type n){ if(_FORWARD) _forward(n); else _backard(n); }
			inline void _backard(difference_type n){ if(_FORWARD) _backard(n); else _forward(n); }
			inline void _forward(){ if(_FORWARD) _forward(); else _backard(); }
			inline void _backard(){ if(_FORWARD) _backard(); else _forward(); }
		public:
			circle_iterator() :_begin(nullptr), _end(nullptr), _current(nullptr) {}
			circle_iterator(pointer* begin, pointer* end,  pointer* current) : _begin(begin), _end(end), _current(current) {}
			circle_iterator(pointer* data, size_type size, size_type pos) : circle_iterator(data,data+size,data + pos) {}
			circle_iterator(const circle_iterator<__value_type,_FORWARD>& other) : circle_iterator(other._data, other._size,other._pos) {}

			self_type &operator++() {  _forward(); return *this; }
			self_type operator++(int) { self_type tmp(*this); ++(*this); return tmp; }
			self_type &operator--() {  _backard();  return *this; }
			self_type operator--(int) { self_type tmp(*this); --(*this); return tmp; }
			self_type operator+(difference_type n) { self_type tmp(*this); _forward(n); return tmp; }
			self_type &operator+=(difference_type n) { __forward(n); return *this; }
			self_type operator-(difference_type n) { self_type tmp(*this); _backard(n); return tmp; }
			self_type &operator-=(difference_type n) { _backard(n); return *this; }

			template<typename TT>
			typename std::enable_if<std::is_same<std::remove_cv_t<TT>, std::remove_cv_t<T>>::value,bool>::type
			 operator==(const circle_iterator<TT,_FORWARD>& other) const { return _current == other._current; }
			template<typename TT>
			typename std::enable_if<std::is_same<std::remove_cv_t<TT>, std::remove_cv_t<T>>::value,bool>::type
			 operator!=(const circle_iterator<TT,_FORWARD>& other) const { return _current != other._current; }
			template<typename TT>
			typename std::enable_if<std::is_same<std::remove_cv_t<TT>, std::remove_cv_t<T>>::value,bool>::type
			 operator<=(const circle_iterator<TT,_FORWARD>& other) const { return _current <= other._current; }
			template<typename TT>
			typename std::enable_if<std::is_same<std::remove_cv_t<TT>, std::remove_cv_t<T>>::value,bool>::type
			 operator>=(const circle_iterator<TT,_FORWARD>& other) const { return _current >= other._current; }
			template<typename TT>
			typename std::enable_if<std::is_same<std::remove_cv_t<TT>, std::remove_cv_t<T>>::value,bool>::type
			 operator<(const circle_iterator<TT,_FORWARD>& other) const { return _current != other._current; }
			template<typename TT>
			typename std::enable_if<std::is_same<std::remove_cv_t<TT>, std::remove_cv_t<T>>::value,bool>::type
			 operator>(const circle_iterator<TT,_FORWARD>& other) const { return _current > other._current; }

			reference operator*()  { return *_current; }
			const_reference operator*()  const { return *_current; }
			pointer operator->() { return _current; }
			const_pointer operator->() const { return _current; }
		private:
			friend circle_iterator<__const_value_type,_FORWARD>;
			friend circle_iterator<__value_type,_FORWARD>;
			pointer* _begin;
			pointer _end;
			pointer* _current;
	};
	template<typename T>
	struct data_traits {
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
	};


	template<typename _ELM_TYPE, size_t _COUNT>
	class array_t  {
	public:
		using self_type = array_t<_ELM_TYPE,_COUNT>;
		using traits = data_traits<_ELM_TYPE>;
		using value_type = typename traits::value_type;
		using pointer = typename traits::pointer;
		using reference = typename traits::reference;
		using const_pointer = typename traits::const_pointer;
		using const_reference = typename traits::const_reference;
		using difference_type = typename traits::difference_type;
		using size_type = typename traits::size_type;
		using iterator = pointer;
		using const_iterator = const_pointer;
		array_t()  {}
		pointer data() { return _data; }
		const_pointer data() const { return _data; }
		size_type capacity() const { return _COUNT; }
		size_type size() const { return _COUNT; }
		reference at(size_type i) { return _data[i]; }
		const_reference at(size_type i) const { return _data[i]; }
		reference operator[](size_type i) { return _data[i]; }
		const_reference operator[](size_type i) const { return _data[i]; }
		iterator begin() { return _data[0]; }
		iterator end() { return &_data[_COUNT]; }
		const_iterator begin() const { return &_data[0]; }
		const_iterator end() const { return &_data[_COUNT]; }
	private:
		value_type _data[_COUNT];
	};

	template<typename _ELM_TYPE>
	class data_t  {
	public:
		using self_type = data_t<_ELM_TYPE>;
		using traits = data_traits<_ELM_TYPE>;
		using value_type = typename traits::value_type;
		using pointer = typename traits::pointer;
		using reference = typename traits::reference;
		using const_pointer = typename traits::const_pointer;
		using const_reference = typename traits::const_reference;
		using difference_type = typename traits::difference_type;
		using size_type = typename traits::size_type;
		using iterator = pointer;
		using const_iterator = const_pointer;

		data_t() : _data(nullptr), _capacity(0) {}
		data_t(pointer data, size_type capacity) : _data(data), _capacity(capacity) {}
		template<size_type _SIZE>
		data_t(value_type (&array)[_SIZE]) : _data(array), _capacity(_SIZE) {}
		template<size_type COUNT>
		data_t(array_t<_ELM_TYPE,COUNT>& array) : _data(array.data()), _capacity(array.capacity()) {}

		size_type capacity() const { return _capacity; }
		size_type size() const { return _capacity; }
		reference at(size_type i) { return _data[i]; }
		const_reference at(size_type i) const { return _data[i]; }
		reference operator[](size_type i) { return _data[i]; }
		reference operator[](size_type i) const { return _data[i]; }
		iterator begin() { return &_data[0]; }
		iterator end() { return &_data[_capacity]; }
		const_iterator begin() const { return &_data[0]; }
		const_iterator end() const { return &_data[_capacity]; }
	private:
		pointer _data;
		size_type _capacity;
	};



	template<typename _DATA_TYPE>
	class buffer_t   {
	public:
		using self_type = buffer_t<_DATA_TYPE>;
		using data_type = _DATA_TYPE;
		using value_type = typename data_type::value_type;
		using pointer = typename data_type::pointer;
		using reference = typename data_type::reference;
		using const_pointer = typename data_type::const_pointer;
		using const_reference = typename data_type::const_reference;
		using difference_type = typename data_type::difference_type;
		using size_type = typename data_type::size_type;
		using iterator = typename data_type::iterator;
		using const_iterator = typename data_type::const_iterator;

		buffer_t() : _data() , _pos(0) {}
		template<typename = std::enable_if<std::is_constructible<_DATA_TYPE,pointer,size_type>::value>>
		buffer_t(pointer buffer, size_type size) : _data(buffer, size), _pos(0) {}
		template<size_t _SIZE,typename = std::enable_if<std::is_constructible<_DATA_TYPE,value_type (&)[_SIZE]>::value>>
		buffer_t(value_type (&buffer)[_SIZE]) : _data(buffer, size), _pos(0) {}
		buffer_t(data_type&& data) : _data(data), _pos(0) {}
		buffer_t(const data_type& data) : _data(data), _pos(0) {}



		size_type capacity() const { return _data.size(); }
		pointer data() { return _data.data(); }
		const_pointer data() const { return _data.data(); }

		reference front()  { return _data[0]; }
		reference back() { return _data[_pos]; }
		const_reference front() const { return _data[0]; }
		const_reference back() const { return _data[_pos]; }

		void clear() { _pos = 0; }
		size_type size() const { return _pos; }
		bool empty() const { return _pos == 0; }

		iterator begin() { return _data.begin(); }
		iterator end() { return _data.begin() + _pos; }
		const_iterator begin() const { return _data.begin(); }
		const_iterator end() const { return _data.begin() + _pos; }

		reference at(size_t i)  { assert(i < size()); return _data[i]; }
		const_reference at(size_t i) const { assert(i < size()); return _data[i]; }

		const_reference operator[](size_t i) const { return _data.at(i); }
		reference operator[](size_t i)  { return _data.at(i); }

		bool operator==(const self_type& r) const { return size() == r.size() && std::equal(begin(),end(),r.begin()); }
		bool operator!=(const self_type&  r) const { return !(*this == r); }

		void push_back(value_type c) { if(_pos == size()) return; _data[_pos++] = c;  }
		void pop_back() { if(_pos > 0) --_pos;   }

		size_type write(const_pointer data, size_type count){
			count = std::min(capacity()-size(),count); // to_write
			if(count > 0) {
				std::copy(data,data+count,end());
				_pos+=count;
			}
			return count;
		}
		size_type read(pointer data, size_type count){
			if(_pos ==0 || count ==0) return 0;
			count = std::min(count,size());
			std::copy(begin(),begin()+count,data);
			if(_pos >0) std::copy(begin()+count,end(),begin()); // move data back
			_pos -=count;
			return count;
		}
	private:
		data_type _data;
		size_type _pos;
	};

	template<typename T, size_t COUNT>
	using buffer_array_t = buffer_t<array_t<T,COUNT>>;
	template<typename T, size_t COUNT>
	using buffer_data_t = buffer_t<data_t<T>>;

	template<typename _DATA_TYPE>
	class circle_t  {
	    void _inc_tail() {
		    if (++_tail == capacity()) _tail = 0;
	    }
	    void _inc_head(){
			if (++_head == _data.size()) _head = 0;
	    }
	public:
		using self_type = circle_t<_DATA_TYPE>;
		using data_type = _DATA_TYPE;
		using value_type = typename data_type::value_type;
		using pointer = typename data_type::pointer;
		using reference = typename data_type::reference;
		using const_pointer = typename data_type::const_pointer;
		using const_reference = typename data_type::const_reference;
		using difference_type = typename data_type::difference_type;
		using size_type = typename data_type::size_type;
		using iterator = circle_iterator<value_type,false>;
		using const_iterator = circle_iterator<const value_type,false>;

		circle_t() : _data() , _head(0),_tail(0) {}

		template<typename = std::enable_if<std::is_constructible<_DATA_TYPE,pointer,size_type>::value>>
				circle_t(pointer buffer, size_type size) : data_type(buffer, size), _head(0),_tail(0)   {}
		template<size_t _SIZE,typename = std::enable_if<std::is_constructible<_DATA_TYPE,value_type (&)[_SIZE]>::value>>
				circle_t(value_type (&buffer)[_SIZE]) : data_type(buffer, size), _head(0),_tail(0)   {}
		circle_t(data_type&& data) : data_type(data), _head(0),_tail(0)   {}
		circle_t(const data_type& data) : data_type(data), _head(0),_tail(0)   {}

		size_type capacity() const { return _data.size(); }
		pointer data() { return _data.data(); }
		const_pointer data() const { return _data.data(); }

		iterator begin()  { return iterator(_data.begin(),_data.end(), _data.begin()+_tail); }
		iterator end()  { return iterator(_data.begin(),_data.end(), _data.begin() + (_head==0 ? capacity() : _head )-1); }
		const_iterator begin() const  { return iterator(_data.begin(),_data.end(), _data.begin()+_tail); }
		const_iterator end() const { return iterator(_data.begin(),_data.end(), _data.begin()+ (_head==0 ? capacity() : _head )-1); }

		void clear() { _head = _tail  = 0; }
		bool empty() const { return _head == _tail; }
		bool full() const { return (_head + 1) == _tail || (_head == capacity() && _tail == 0); }
		size_type size() const {  return (_head > _tail) ? _head- _tail : capacity() - _tail - _head; }
		const_reference top() const { return _data[_tail]; }
	    void push(const_reference item) {
	    	if (full())  return; // we just fail
	    	_inc_head();
	    	_data[_head] = item;

	    }
	    void pop() {
			if(empty()) return;
			_inc_tail();
	    }

	    // use to find the size of the start of the first continuous section
	    size_type continuious_size() const { return (_head > _tail) ? _head- _tail : capacity()- _tail; }
	    pointer continuious_ptr_begin() { return _data.data() + _tail; }
	    const_pointer continuious_ptr_begin() const { return _data.data() + _tail; }
	    pointer continuious_ptr_end() { return  continuious_ptr_begin() +  continuious_size(); }
	    const_pointer continuious_ptr_end() const { return continuious_ptr_begin() +  continuious_size(); }
	    // giving write or read a null pointer will advance the _tail or _head without copying anything
		size_type write(const_pointer data_, size_type count){
			if(count == 0 || full()) return 0;
			count = std::min(count, capacity() -size());
			if (count <= (capacity() - _head)) {
				  if(data_) std::copy_n(data_, count,data()+_head);
				  _head+=count;
				  if(_head == capacity()) _head = 0;
			} else {
				  size_type size_1 = (capacity() - _head);
				  size_type size_2 = count - size_1;
				  if(data_) {
					  std::copy_n(data_,size_1, data()+_head);
					  data_ += size_1;
					  std::copy_n(data_,size_2, data());
				  }
				  _head = size_2;
			}
			return count;
		}
		size_type read(pointer data_, size_type count){
			 if (count == 0 || empty()) return 0;
			  count = std::min(count, size());
			  // Read in a single step
			  if (count <= (capacity() - _tail)) {
				  if(data_) std::copy_n(data()+_tail,count, data_);
				  _tail+=count;
				  if(_tail == capacity()) _tail = 0;
			  } else { // Read in two steps
				  size_type size_1 = (capacity() - _tail);
				  size_type size_2 = count - size_1;
				  if(data_) {
					  std::copy_n(data()+_tail,size_1, data_);
					  data_ += size_1;
					  std::copy_n(data()+size_2,size_2, data_);
				  }
				  _tail = size_2;
			  }
			  return count;
		}
	private:
		data_type _data;
		size_type _head;
		size_type _tail;
	};


	template<typename T, size_t COUNT>
	using circle_array_t = circle_t<array_t<T,COUNT>>;
	template<typename T, size_t COUNT>
	using circle_data_t = circle_t<data_t<T>>;
#if 0

	typedef struct {
		uint8_t* ptr;
		size_t size;
		size_t pos;
	}buffer_t;

	typedef struct {
		uint8_t* ptr;
		size_t size;
		size_t head;
		size_t tail;
	}circle_t;

	inline static void circle_clear(circle_t* circle){
		circle->head = circle->tail = 0;
	}
	inline static void circle_init(circle_t* circle, uint8_t* ptr, size_t size){
		circle->ptr = ptr;
		circle->size  = size;
		circle_clear(circle);
	}
	inline static bool circle_empty(circle_t* circle) { return circle->head == circle->tail; }
	inline static bool circle_putc(circle_t* circle, uint8_t c){
		volatile size_t head = circle->head;
		circle->ptr[head] = c;
		if(++head > circle->size) head = 0;
		circle->head = head;
		return circle->head == circle->tail;
	}

	inline static size_t circle_puts(circle_t* circle, const char* str) {
		size_t cnt=0;
		while(*str) { if(circle_putc(circle,*str++)) break;++cnt; }
		return cnt;
	}
	inline static size_t circle_write(circle_t* circle, const uint8_t* data,size_t len) {
		size_t cnt=0;
		while(len--) { if(circle_putc(circle,*data++)) break;++cnt; }
		return cnt;
	}
	inline static size_t circle_number(circle_t* circle, int num) {
		char nbuf[9];
		return circle_puts(circle,itoa(num,nbuf,10));
	}

	inline static bool circle_getc(circle_t* circle, uint8_t* c){
		if(circle_empty(circle)) return false;
		volatile size_t tail = circle->tail;
		*c = circle->ptr[tail];
		if(++tail > circle->size) tail = 0;
		circle->tail = tail;
		return true;
	}
	inline static void buffer_init(buffer_t* buffer, uint8_t* ptr, size_t size){
		buffer->ptr = ptr;
		buffer->size  = size;
		buffer->pos = 0;
	}
	inline static void buffer_clear(buffer_t* buffer) {
		buffer->pos = 0;
	}
	inline static void buffer_putc(buffer_t* buffer, uint8_t c) {
		assert(buffer->size > buffer->pos);
		buffer->ptr[buffer->pos++] = c;
	}

	inline static void buffer_puts(buffer_t* buffer,const char* str) {
		while(*str) buffer_putc(buffer,*str++);
	}
	inline static void buffer_write(buffer_t* buffer,const uint8_t* data,size_t len) {
		while(len--) buffer_putc(buffer,*data++);
	}
	inline static void buffer_number(buffer_t* buffer, int num) {
		char nbuf[8];
		buffer_puts(buffer,itoa(num,nbuf,10));
	#if 0
		// stupid simple
		if(num == 0) buffer_putc(buffer,'0');
		if(num > 999) {
			buffer_putc(buffer,num%1000 + '0');
			num/=1000;
		}
		if(num > 99) {
			buffer_putc(buffer,num%100 + '0');
			num/=100;
		}
		if(num > 9) {
			buffer_putc(buffer,num%10+ '0');
			num/=10;
		}
		if(num > 0){
			buffer_putc(buffer,num+ '0');
		}
	#endif
	}
}

#endif

}

#endif
