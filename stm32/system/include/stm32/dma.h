#ifndef _DMA_H_
#define _DMA_H_

#include "common.h"


namespace stm32 {
	CREATE_CR_REG(DMA_CR, DMA_Stream_TypeDef, offsetof(DMA_Stream_TypeDef, CR));
	CREATE_CR_REG(DMA_FCR, DMA_Stream_TypeDef, offsetof(DMA_Stream_TypeDef, FCR));
	enum class DMA_IT :uint32_t {
		TC	=	((uint32_t)DMA_SxCR_TCIE),
		HT	=	((uint32_t)DMA_SxCR_HTIE),
		TE	=	((uint32_t)DMA_SxCR_TEIE),
		DME	=	((uint32_t)DMA_SxCR_DMEIE),
		FE	=	((uint32_t)0x00000080U),
	};
	enum class DMA_ERROR : uint32_t {
		NONE            =0x00000000U,   /*!< No error                               */
		TE             =0x00000001U,    /*!< Transfer error                         */
		FE             = 0x00000002U,    /*!< FIFO error                             */
		DME            = 0x00000004U,    /*!< Direct Mode error                      */
		TIMEOUT        = 0x00000020U,    /*!< Timeout error                          */
		PARAM          = 0x00000040U,    /*!< Parameter error                        */
		NO_XFER        = 0x00000080U,    /*!< Abort requested with no Xfer ongoing   */
		NOT_SUPPORTED = 0x00000100U    /*!< Not supported mode */
	};
	template<typename T> struct DMA_MASK : std::false_type {};
	enum class DMA_CHANNEL {
		N0	=	DMA_CHANNEL_0,
		N1	=	DMA_CHANNEL_1,
		N2	=	DMA_CHANNEL_2,
		N3	=	DMA_CHANNEL_3,
		N4	=	DMA_CHANNEL_4,
		N5	=	DMA_CHANNEL_5,
		N6	=	DMA_CHANNEL_6,
		N7	=	DMA_CHANNEL_7,
	};
	 CREATE_CR_MASK(DMA_CR, DMA_CHANNEL, DMA_SxCR_CHSEL);

	enum class DMA_DIR : uint32_t {
		PERIPH_TO_MEMORY    =     ((uint32_t)0x00000000U) ,     /*!< Peripheral to memory direction */
		MEMORY_TO_PERIPH    =     ((uint32_t)DMA_SxCR_DIR_0),  /*!< Memory to peripheral direction */
		MEMORY_TO_MEMORY    =     ((uint32_t)DMA_SxCR_DIR_1),  /*!< Memory to memory direction     */
	};
	 CREATE_CR_MASK(DMA_CR, DMA_DIR, DMA_SxCR_DIR_0|DMA_SxCR_DIR_1);

	enum class DMA_PINC : uint32_t {
		ENABLE        =((uint32_t)DMA_SxCR_PINC),  /*!< Peripheral increment mode enable  */
		DISABLE       =((uint32_t)0x00000000U),     /*!< Peripheral increment mode disable */
	};
	 CREATE_CR_MASK(DMA_CR, DMA_PINC, DMA_SxCR_PINC);

	enum class DMA_MINC : uint32_t {
		ENABLE        =((uint32_t)DMA_SxCR_MINC),  /*!< Memory increment mode enable  */
		DISABLE       =((uint32_t)0x00000000U),     /*!< Memory increment mode disable */
	};
	 CREATE_CR_MASK(DMA_CR, DMA_MINC, DMA_SxCR_MINC);



	enum class DMA_PDATAALIGN : uint32_t {
		BYTE       =((uint32_t)0x00000000U),
		HALFWORD   =((uint32_t)DMA_SxCR_PSIZE_0),
		WORD       =((uint32_t)DMA_SxCR_PSIZE_1),
	};
	 CREATE_CR_MASK(DMA_CR, DMA_PDATAALIGN, DMA_SxCR_PSIZE_0|DMA_SxCR_PSIZE_1);

	enum class DMA_MDATAALIGN : uint32_t {
		BYTE       =((uint32_t)0x00000000U),
		HALFWORD   =((uint32_t)DMA_SxCR_MSIZE_0),
		WORD       =((uint32_t)DMA_SxCR_MSIZE_1),
	};
	 CREATE_CR_MASK(DMA_CR, DMA_MDATAALIGN, DMA_SxCR_MSIZE_0|DMA_SxCR_MSIZE_1);

	enum class DMA_MODE : uint32_t {
		NORMAL       =((uint32_t)0x00000000U),
		CIRCULAR   =((uint32_t)DMA_SxCR_CIRC),
		PFCTRL       =((uint32_t)DMA_SxCR_PFCTRL),
	};
	 CREATE_CR_MASK(DMA_CR, DMA_MODE, DMA_SxCR_CIRC|DMA_SxCR_PFCTRL);

	enum class DMA_PRIORITY : uint32_t {
		LOW        =((uint32_t)0x00000000U),
		MEDIUM   =((uint32_t)DMA_SxCR_PL_0),
		HIGH        =((uint32_t)DMA_SxCR_PL_1),
		VERY_HIGH       =((uint32_t)DMA_SxCR_PL),
	};
	CREATE_CR_MASK(DMA_CR, DMA_PRIORITY, DMA_SxCR_PL_0|DMA_SxCR_PL_1|DMA_SxCR_PL);

	enum class DMA_FIFO : uint32_t {
		ENABLE        =((uint32_t)DMA_SxFCR_DMDIS),
		DISABLE       =((uint32_t)0x00000000U),
	};
	CREATE_CR_MASK(DMA_FCR, DMA_FIFO, DMA_SxFCR_DMDIS);

	enum class DMA_FIFO_THRESHOLD : uint32_t {
		ONEQUARTERFULL        =((uint32_t)0x00000000U),
		HALFFULL   =((uint32_t)DMA_SxFCR_FTH_0),
		THREEQUARTERSFULL        =((uint32_t)DMA_SxFCR_FTH_1),
		FULL       =((uint32_t)DMA_SxFCR_FTH),
	};
	 CREATE_CR_MASK(DMA_FCR, DMA_FIFO_THRESHOLD, DMA_SxFCR_FTH);

	enum class DMA_FIFO_STATUS: uint32_t {
		// these numbers are WRONG
		//DMA_SxFCR_FS_0 DMA_SxFCR_FS_1 DMA_SxFCR_FS_2
		LESSTHANONEQUARTERFULL =0,
		ONEQUARTERFULL        = 1,
		HALFFULL   = 2,
		THREEQUARTERSFULL        =3,
		EMPTY        =4,
		FULL       =5
	};

	enum class DMA_MBURST: uint32_t {
		SINGLE        =((uint32_t)0x00000000U),
		INC4   =((uint32_t)DMA_SxCR_MBURST_0),
		INC8        =((uint32_t)DMA_SxCR_MBURST_1),
		INC16       =((uint32_t)DMA_SxCR_MBURST),
	};
	 CREATE_CR_MASK(DMA_CR, DMA_MBURST, DMA_SxCR_MBURST_0|DMA_SxCR_MBURST_1|DMA_SxCR_MBURST);


	enum class DMA_PBURST: uint32_t {
		SINGLE      =((uint32_t)0x00000000U),
		INC4   		=((uint32_t)DMA_SxCR_PBURST_0),
		INC8        =((uint32_t)DMA_SxCR_PBURST_1),
		INC16       =((uint32_t)DMA_SxCR_PBURST),
	};
	 CREATE_CR_MASK(DMA_CR, DMA_PBURST, DMA_SxCR_PBURST_0|DMA_SxCR_PBURST_1|DMA_SxCR_PBURST);

	template<uintptr_t _BASE_ADDRESS>
	struct DMA_TRAITS {
		static_assert(
				_BASE_ADDRESS == DMA1_Stream0_BASE ||
				_BASE_ADDRESS == DMA1_Stream1_BASE ||
				_BASE_ADDRESS == DMA1_Stream2_BASE ||
				_BASE_ADDRESS == DMA1_Stream3_BASE ||
				_BASE_ADDRESS == DMA1_Stream4_BASE ||
				_BASE_ADDRESS == DMA1_Stream5_BASE ||
				_BASE_ADDRESS == DMA1_Stream6_BASE ||
				_BASE_ADDRESS == DMA1_Stream7_BASE ||
				_BASE_ADDRESS == DMA2_Stream0_BASE ||
				_BASE_ADDRESS == DMA2_Stream1_BASE ||
				_BASE_ADDRESS == DMA2_Stream2_BASE ||
				_BASE_ADDRESS == DMA2_Stream3_BASE ||
				_BASE_ADDRESS == DMA2_Stream4_BASE ||
				_BASE_ADDRESS == DMA2_Stream5_BASE ||
				_BASE_ADDRESS == DMA2_Stream6_BASE ||
				_BASE_ADDRESS == DMA2_Stream7_BASE,
				"Bad DMA base address!"
		);
		static constexpr uintptr_t DMA_BASE_ADDRESS =
				_BASE_ADDRESS == DMA1_Stream0_BASE ||
				_BASE_ADDRESS == DMA1_Stream1_BASE ||
				_BASE_ADDRESS == DMA1_Stream2_BASE ||
				_BASE_ADDRESS == DMA1_Stream3_BASE ||
				_BASE_ADDRESS == DMA1_Stream4_BASE ||
				_BASE_ADDRESS == DMA1_Stream5_BASE ||
				_BASE_ADDRESS == DMA1_Stream6_BASE ||
				_BASE_ADDRESS == DMA1_Stream7_BASE ? DMA1_BASE : DMA2_BASE;
		static constexpr uint32_t DMA_NUMBER = DMA_BASE_ADDRESS == DMA1_BASE ? 1: 2;

		static constexpr uintptr_t BASE_ADDRESS = _BASE_ADDRESS;
		static constexpr uint32_t STREAM_NUMBER = ((_BASE_ADDRESS & 0xFFU) - 16U) / 24U;
		static constexpr uint32_t STREAM_INDEX =    STREAM_NUMBER == 0 ? 0U :
													STREAM_NUMBER == 1 ? 6U :
													STREAM_NUMBER == 2 ? 16U :
													STREAM_NUMBER == 3 ? 22U :
													STREAM_NUMBER == 4 ? 0U :
													STREAM_NUMBER == 5 ? 6U :
													STREAM_NUMBER == 6 ? 16U : 22U;
		static constexpr uintptr_t BASE_ISR_ADDRESS =
				STREAM_INDEX > 3U ?
					    /* return pointer to HISR and HIFCR */
						(((uintptr_t)BASE_ADDRESS & (uint32_t)(~0x3FFU)) + 4U):
						// return low pointer
					    ((uintptr_t)BASE_ADDRESS & (uint32_t)(~0x3FFU));

		static constexpr uintptr_t BASE_IFCR_ADDRESS = BASE_ISR_ADDRESS + sizeof(uint32_t);
		constexpr static uint8_t DMA_Flags_Bit_Pos[4] = { 0, 6, 16, 22 };
		constexpr static IRQn_Type DMA_IRQs[2][8] = {
			{
				DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn, DMA1_Stream3_IRQn,
				DMA1_Stream4_IRQn, DMA1_Stream5_IRQn, DMA1_Stream6_IRQn, DMA1_Stream7_IRQn
			},
			{
				DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn, DMA2_Stream3_IRQn,
				DMA2_Stream4_IRQn, DMA2_Stream5_IRQn, DMA2_Stream6_IRQn, DMA2_Stream7_IRQn
			}
		};
		constexpr static IRQn_Type IRQ_NUMBER = DMA_IRQs[DMA_NUMBER-1][STREAM_NUMBER];

		static constexpr uint32_t FE_FLAG =
					(BASE_ADDRESS == DMA1_Stream0_BASE)? DMA_FLAG_FEIF0_4 :
					(BASE_ADDRESS == DMA2_Stream0_BASE)? DMA_FLAG_FEIF0_4 :
					(BASE_ADDRESS == DMA1_Stream4_BASE)? DMA_FLAG_FEIF0_4 :
					(BASE_ADDRESS == DMA2_Stream4_BASE)? DMA_FLAG_FEIF0_4 :
					(BASE_ADDRESS == DMA1_Stream1_BASE)? DMA_FLAG_FEIF1_5 :
					(BASE_ADDRESS == DMA2_Stream1_BASE)? DMA_FLAG_FEIF1_5 :
					(BASE_ADDRESS == DMA1_Stream5_BASE)? DMA_FLAG_FEIF1_5 :
					(BASE_ADDRESS == DMA2_Stream5_BASE)? DMA_FLAG_FEIF1_5 :
					(BASE_ADDRESS == DMA1_Stream2_BASE)? DMA_FLAG_FEIF2_6 :
					(BASE_ADDRESS == DMA2_Stream2_BASE)? DMA_FLAG_FEIF2_6 :
					(BASE_ADDRESS == DMA1_Stream6_BASE)? DMA_FLAG_FEIF2_6 :
					(BASE_ADDRESS == DMA2_Stream6_BASE)? DMA_FLAG_FEIF2_6 : DMA_FLAG_FEIF3_7;
		static constexpr uint32_t TC_FLAG =
			(BASE_ADDRESS == DMA1_Stream0_BASE)? DMA_FLAG_TCIF0_4 :
			(BASE_ADDRESS == DMA2_Stream0_BASE)? DMA_FLAG_TCIF0_4 :
			(BASE_ADDRESS == DMA1_Stream4_BASE)? DMA_FLAG_TCIF0_4 :
			(BASE_ADDRESS == DMA2_Stream4_BASE)? DMA_FLAG_TCIF0_4 :
			(BASE_ADDRESS == DMA1_Stream1_BASE)? DMA_FLAG_TCIF1_5 :
			(BASE_ADDRESS == DMA2_Stream1_BASE)? DMA_FLAG_TCIF1_5 :
			(BASE_ADDRESS == DMA1_Stream5_BASE)? DMA_FLAG_TCIF1_5 :
			(BASE_ADDRESS == DMA2_Stream5_BASE)? DMA_FLAG_TCIF1_5 :
			(BASE_ADDRESS == DMA1_Stream2_BASE)? DMA_FLAG_TCIF2_6 :
			(BASE_ADDRESS == DMA2_Stream2_BASE)? DMA_FLAG_TCIF2_6 :
			(BASE_ADDRESS == DMA1_Stream6_BASE)? DMA_FLAG_TCIF2_6 :
			(BASE_ADDRESS == DMA2_Stream6_BASE)? DMA_FLAG_TCIF2_6 : DMA_FLAG_TCIF3_7;
		static constexpr uint32_t HT_FLAG =
				(BASE_ADDRESS == DMA1_Stream0_BASE)? DMA_FLAG_HTIF0_4 :
				(BASE_ADDRESS == DMA2_Stream0_BASE)? DMA_FLAG_HTIF0_4 :
				(BASE_ADDRESS == DMA1_Stream4_BASE)? DMA_FLAG_HTIF0_4 :
				(BASE_ADDRESS == DMA2_Stream4_BASE)? DMA_FLAG_HTIF0_4 :
				(BASE_ADDRESS == DMA1_Stream1_BASE)? DMA_FLAG_HTIF1_5 :
				(BASE_ADDRESS == DMA2_Stream1_BASE)? DMA_FLAG_HTIF1_5 :
				(BASE_ADDRESS == DMA1_Stream5_BASE)? DMA_FLAG_HTIF1_5 :
				(BASE_ADDRESS == DMA2_Stream5_BASE)? DMA_FLAG_HTIF1_5 :
				(BASE_ADDRESS == DMA1_Stream2_BASE)? DMA_FLAG_HTIF2_6 :
				(BASE_ADDRESS == DMA2_Stream2_BASE)? DMA_FLAG_HTIF2_6 :
				(BASE_ADDRESS == DMA1_Stream6_BASE)? DMA_FLAG_HTIF2_6 :
				(BASE_ADDRESS == DMA2_Stream6_BASE)? DMA_FLAG_HTIF2_6 : DMA_FLAG_HTIF3_7;
		static constexpr uint32_t TE_FLAG =
				(BASE_ADDRESS == DMA1_Stream0_BASE)? DMA_FLAG_TEIF0_4 :
				(BASE_ADDRESS == DMA2_Stream0_BASE)? DMA_FLAG_TEIF0_4 :
				(BASE_ADDRESS == DMA1_Stream4_BASE)? DMA_FLAG_TEIF0_4 :
				(BASE_ADDRESS == DMA2_Stream4_BASE)? DMA_FLAG_TEIF0_4 :
				(BASE_ADDRESS == DMA1_Stream1_BASE)? DMA_FLAG_TEIF1_5 :
				(BASE_ADDRESS == DMA2_Stream1_BASE)? DMA_FLAG_TEIF1_5 :
				(BASE_ADDRESS == DMA1_Stream5_BASE)? DMA_FLAG_TEIF1_5 :
				(BASE_ADDRESS == DMA2_Stream5_BASE)? DMA_FLAG_TEIF1_5 :
				(BASE_ADDRESS == DMA1_Stream2_BASE)? DMA_FLAG_TEIF2_6 :
				(BASE_ADDRESS == DMA2_Stream2_BASE)? DMA_FLAG_TEIF2_6 :
				(BASE_ADDRESS == DMA1_Stream6_BASE)? DMA_FLAG_TEIF2_6 :
				(BASE_ADDRESS == DMA2_Stream6_BASE)? DMA_FLAG_TEIF2_6 : DMA_FLAG_TEIF3_7;
		static constexpr uint32_t DME_FLAG =
				(BASE_ADDRESS == DMA1_Stream0_BASE)? DMA_FLAG_DMEIF0_4 :
				(BASE_ADDRESS == DMA2_Stream0_BASE)? DMA_FLAG_DMEIF0_4 :
				(BASE_ADDRESS == DMA1_Stream4_BASE)? DMA_FLAG_DMEIF0_4 :
				(BASE_ADDRESS == DMA2_Stream4_BASE)? DMA_FLAG_DMEIF0_4 :
				(BASE_ADDRESS == DMA1_Stream1_BASE)? DMA_FLAG_DMEIF1_5 :
				(BASE_ADDRESS == DMA2_Stream1_BASE)? DMA_FLAG_DMEIF1_5 :
				(BASE_ADDRESS == DMA1_Stream5_BASE)? DMA_FLAG_DMEIF1_5 :
				(BASE_ADDRESS == DMA2_Stream5_BASE)? DMA_FLAG_DMEIF1_5 :
				(BASE_ADDRESS == DMA1_Stream2_BASE)? DMA_FLAG_DMEIF2_6 :
				(BASE_ADDRESS == DMA2_Stream2_BASE)? DMA_FLAG_DMEIF2_6 :
				(BASE_ADDRESS == DMA1_Stream6_BASE)? DMA_FLAG_DMEIF2_6 :
				(BASE_ADDRESS == DMA2_Stream6_BASE)? DMA_FLAG_DMEIF2_6 : DMA_FLAG_DMEIF3_7;
		enum class DMA_FLAG :uint32_t {
			NONE = 0U,
			FE = FE_FLAG,
			TC= TC_FLAG,
			HT = HT_FLAG,
			TE =TE_FLAG,
			DME = DME_FLAG,
			ALL = FE_FLAG | TC_FLAG | HT_FLAG | DME_FLAG
		};
		inline static DMA_Stream_TypeDef* get_stream() { return (DMA_Stream_TypeDef*)BASE_ADDRESS; }
		inline static uint32_t get_isr() { return *((volatile uint32_t*)BASE_ISR_ADDRESS); }
		inline static void set_ifcr(uint32_t v) {  *((volatile uint32_t*)BASE_ISR_ADDRESS) = v; }
		inline static void clear_flags() { set_ifcr(static_cast<uint32_t>(DMA_FLAG::ALL)); }
		inline static void set_inq_priority(uint32_t preemption){
			HAL_NVIC_DisableIRQ(IRQ_NUMBER);
			HAL_NVIC_SetPriority(IRQ_NUMBER, preemption, STREAM_NUMBER);
		}
		inline static void enable_clock() {
			if (DMA_NUMBER == 1)
				RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
			else
				RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
		}
		inline static void disable_clock() {
			if (DMA_NUMBER == 1)
				RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA1EN;
			else
				RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2EN;
		}
		inline static void enable_interrupts() {
			auto DMA_Stream = get_stream();
			clear_flags();
			/* Enable IRQ */
			HAL_NVIC_EnableIRQ(IRQ_NUMBER);

			/* Enable DMA stream interrupts */
			DMA_Stream->CR |= DMA_SxCR_TCIE | DMA_SxCR_HTIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE;
			DMA_Stream->FCR |= DMA_SxFCR_FEIE;
		}
		inline static void disable_irq() {
			auto DMA_Stream = get_stream();
			clear_flags();
			HAL_NVIC_DisableIRQ(IRQ_NUMBER);
			/* Disable DMA stream interrupts */
			DMA_Stream->CR &= ~(DMA_SxCR_TCIE  | DMA_SxCR_HTIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE);
			DMA_Stream->FCR &= DMA_SxFCR_FEIE;
		}
		inline static bool get_flag(uint32_t isr, DMA_FLAG flag) {
			return (isr & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
		}
		inline static bool get_flag(DMA_FLAG flag) {
			return (get_isr()  & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
		}
		inline static void clear_flag(DMA_FLAG flag) { set_ifcr(static_cast<uint32_t>(flag)); }
	};

	template<size_t _SIZE>
	class DMA_STATIC_BUFFER {
		static constexpr size_t BYTE_SIZE = _SIZE;
		static constexpr size_t WORD_SIZE = (_SIZE +3) & ~3;
		uint32_t _buffer[WORD_SIZE];
	public:
		DMA_STATIC_BUFFER() {}
		uint8_t* data()  { return (uint8_t*)_buffer;  }
		const uint8_t* data() const { return (uint8_t*)_buffer;  }
		constexpr size_t size() const { return BYTE_SIZE; }
		uint8_t* begin() { return data(); }
		uint8_t* end() { return data()+size(); }
		const uint8_t* begin() const { return data(); }
		const uint8_t* end() const{ return data()+size();  }
		uint8_t operator[](size_t i) const { return data()[i]; }
		uint8_t& operator[](size_t i)  { return data()[i]; }
	};

	template<uintptr_t _BASE_ADDRESS>
	class DMA {
		using traits = DMA_TRAITS<_BASE_ADDRESS>;
		using DMA_FLAG = typename traits::DMA_FLAG;
		/* Offsets for bits */


		using cr_helper = priv::CR_MASK_HELPER<DMA_CR>;
		using fcr_helper = priv::CR_MASK_HELPER<DMA_FCR>;
		DMA_Stream_TypeDef* _DMA;
		// from the hal stuff

		template<typename T, typename E=void>
		struct _cast_foo {};
		template<typename T>
		struct _cast_foo<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
			constexpr uintptr_t operator()(T v) { return reinterpret_cast <uintptr_t>(v); }
		};
		template<typename T>
		struct _cast_foo<T, typename std::enable_if<!std::is_pointer<T>::value>::type> {
			constexpr uintptr_t operator()(T v) { return static_cast <uintptr_t>(v); }
		};
		void test_aligment(DMA_MDATAALIGN a, uintptr_t addr){
			assert(
					(a == DMA_MDATAALIGN::HALFWORD) ? ((addr & 0x2) == 0) :
					(a == DMA_MDATAALIGN::WORD) ? ((addr & 0x3) == 0) : addr
			);
		}
		void test_aligment(DMA_PDATAALIGN a, uintptr_t addr){
				assert(
						(a == DMA_PDATAALIGN::HALFWORD) ? ((addr & 0x2) == 0) :
						(a == DMA_PDATAALIGN::WORD) ? ((addr & 0x3) == 0) : addr
				);
			}
	public:
		template<typename ENUM>
		ENUM  get_cr() const {
			if(priv::enum_match_reg<ENUM, DMA_CR>::value)
				return cr_helper::get_single<ENUM>(_DMA);
			else if(priv::enum_match_reg<ENUM, DMA_FCR>::value)
				return fcr_helper::get_single<ENUM>(_DMA);
			else return ENUM{};
		}
		template<typename ENUM>
		void set_cr(ENUM e) {
			if(priv::enum_match_reg<ENUM, DMA_CR>::value)
				 cr_helper::set_single<ENUM>(_DMA,e);
			else if(priv::enum_match_reg<ENUM, DMA_FCR>::value)
				 fcr_helper::set_single<ENUM>(_DMA,e);
		}
		void clear_flag(DMA_FLAG f) { traits::clear_flag(f); }
		bool get_flag(DMA_FLAG f) const { return traits::get_flag(f); }
		void clear_config() {
			disable();


			 uint32_t tmp = _DMA->CR;
			  /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits */
			  tmp &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST |
			                      DMA_SxCR_PL    | DMA_SxCR_MSIZE  | DMA_SxCR_PSIZE  |
			                      DMA_SxCR_MINC  | DMA_SxCR_PINC   | DMA_SxCR_CIRC   |
			                      DMA_SxCR_DIR   | DMA_SxCR_CT     | DMA_SxCR_DBM));
			  _DMA->CR = tmp;
			  tmp = _DMA->FCR;
			  /* Clear Direct mode and FIFO threshold bits */
			  tmp &= (uint32_t)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);
			  _DMA->FCR = tmp;
		}

		//void config() { } // clear config
		template<typename... Args>
		void set_config(Args... args){
			disable();
			cr_helper::set_many(_DMA,std::forward<Args>(args)...);
			fcr_helper::set_many(_DMA,std::forward<Args>(args)...);
			clear_all_it();
		}
		DMA() : _DMA { (DMA_Stream_TypeDef*)traits::BASE_ADDRESS } {}
		void init() {
			traits::enable_clock();
			traits::set_inq_priority(2);
		}
		bool is_enabled() const { return (_DMA->CR & DMA_SxCR_EN) != 0; }
		void enable() { _DMA->CR |=  DMA_SxCR_EN; }
		void disable() { _DMA->CR &=  ~DMA_SxCR_EN; }
		DMA_FIFO_STATUS get_fifo_status() const { return _DMA->FCR & DMA_SxFCR_FS; }
		void clear_all_it() { clear_flag(DMA_FLAG::ALL); }
		void enable_it(DMA_IT irq) {
			if(irq != DMA_IT::FE)
				_DMA->CR |= (uint32_t)irq;
			else
				_DMA->FCR |= (uint32_t)irq;
		}
		void disable_it(DMA_IT irq) {
			if(irq != DMA_IT::FE)
				_DMA->CR &= ~(uint32_t)irq;
			else
				_DMA->FCR &= ~(uint32_t)irq;
		}
		bool get_it_source(DMA_IT irq) const {
			return irq != DMA_IT::FE ?
				(_DMA->CR & (uint32_t)irq) != 0 :
				(_DMA->FCR & (uint32_t)irq) !=0;
		}
		inline void set_counter(uint32_t len) { _DMA->NDTR = len; }
		inline uint32_t get_counter() const { return _DMA->NDTR;}

		void set_peripheral_address(uintptr_t addr) {
			test_aligment(get_cr<DMA_PDATAALIGN>(),addr);
			_DMA->PAR = addr;
		}
		void set_memory_address0(uintptr_t addr) {
			test_aligment(get_cr<DMA_MDATAALIGN>(),addr);
			assert((addr & 0x3) == 0);
			_DMA->M0AR = addr;
		}
		void set_memory_address1(uintptr_t addr) {
			test_aligment(get_cr<DMA_MDATAALIGN>(),addr);
			assert((addr & 0x3) == 0);
			_DMA->M1AR = addr;
		}
		uintptr_t get_peripheral_address() const { return _DMA->PAR; }
		uintptr_t get_memory_address0() const { return _DMA->M0AR; }
		uintptr_t get_memory_address1() const { return _DMA->M1AR; }
		template<typename T, typename TT>
		void start(T _src, TT _dest, size_t len) {
			uintptr_t src = _cast_foo<T>{}(_src);
			uintptr_t dest = _cast_foo<TT>{}(_dest);
			 /* Clear DBM bit */


			 _DMA->CR &= (uint32_t)(~DMA_SxCR_DBM);

			  /* Configure DMA Stream data length */
			 _DMA->NDTR = len;

			  /* Peripheral to Memory */
			  if(get_cr<DMA_DIR>() == DMA_DIR::MEMORY_TO_PERIPH)
			  {
					test_aligment(get_cr<DMA_PDATAALIGN>(),dest);
					test_aligment(get_cr<DMA_MDATAALIGN>(),src);
			    /* Configure DMA Stream destination address */
				  _DMA->PAR = dest;

			    /* Configure DMA Stream source address */
				  _DMA->M0AR = src;
			  }
			  /* Memory to Peripheral */
			  else
			  {
					test_aligment(get_cr<DMA_PDATAALIGN>(),src);
					test_aligment(get_cr<DMA_MDATAALIGN>(),dest);
			    /* Configure DMA Stream source address */
				  _DMA->PAR = src;

			    /* Configure DMA Stream destination address */
				  _DMA->M0AR = dest;
			  }
			  enable();
		}

		template<typename COMPLETE_FUNCTION, typename HALF_COMPLETE_FUNCTION>
		DMA_ERROR irq_handler(COMPLETE_FUNCTION tx_complete, HALF_COMPLETE_FUNCTION half_tx_complete) {
			  enum_helper<DMA_ERROR> ret;
			  /* Transfer Error Interrupt management ***************************************/
			  if (get_flag(DMA_FLAG::TE) && get_it_source(DMA_IT::TE)) {
			      /* Disable the transfer error interrupt */
				  disable_it(DMA_IT::TE);
				  clear_flag(DMA_FLAG::TE);
				  ret.set(DMA_ERROR::TE);
			  }
			  if (get_flag(DMA_FLAG::FE) && get_it_source(DMA_IT::FE)) {
				  clear_flag(DMA_FLAG::FE);
				  ret.set(DMA_ERROR::FE);
			  }
			  if (get_flag(DMA_FLAG::DME) && get_it_source(DMA_IT::DME)) {
				  clear_flag(DMA_FLAG::DME);
				  ret.set(DMA_ERROR::DME);
			  }
			  if (get_flag(DMA_FLAG::HT) && get_it_source(DMA_IT::HT)) {
				  clear_flag(DMA_FLAG::HT);
				  if((_DMA->CR & DMA_SxCR_DBM) !=0) { 		  // multi buffering mode
					  if((_DMA->CR & DMA_SxCR_CT) !=0) // memory 0
						  half_tx_complete(_DMA->M0AR);
					  else
						  half_tx_complete(_DMA->M1AR);
				  } else {
				        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
					    if((_DMA->CR & DMA_SxCR_CIRC) == 0)
							  disable_it(DMA_IT::HT);
					    half_tx_complete(_DMA->M0AR);
				  }
			  }

			  /* Transfer Complete Interrupt management ***********************************/
			  if (get_flag(DMA_FLAG::TC) && get_it_source(DMA_IT::TC)) {
				  clear_flag(DMA_FLAG::TC);
				  if((_DMA->CR & DMA_SxCR_DBM) !=0) { 		  // multi buffering mode
					  if((_DMA->CR & DMA_SxCR_CT) !=0) // memory 0
						  tx_complete(_DMA->M0AR);
					  else
						  tx_complete(_DMA->M1AR);
				  } else {
				        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
					    if((_DMA->CR & DMA_SxCR_CIRC) == 0)
							  disable_it(DMA_IT::TC);
					    tx_complete(_DMA->M0AR);
				  }
			  }
			  if(ret != DMA_ERROR::NONE)   /* manage error case */
			  {
				  if(ret.has(DMA_ERROR::TE))
			    {
					  disable();
					  while(is_enabled()) {} // do a time out here
			    }
			  }
			  return ret;
		}
		template<typename COMPLETE_FUNCTION>
		DMA_ERROR irq_handler(COMPLETE_FUNCTION tx_complete){
			return irq_handler(tx_complete, [](uintptr_t){});
		}
		DMA_ERROR irq_handler(){
			return irq_handler([](uintptr_t){}, [](uintptr_t){});
		}
	};




}
#endif
