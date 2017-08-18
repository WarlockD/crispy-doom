#ifndef _UART_H_
#define _UART_H_
#include "common.h"

namespace stm32 {
	CREATE_CR_REG(UART_CR1, USART_TypeDef, offsetof(USART_TypeDef, CR1));
	CREATE_CR_REG(UART_CR2, USART_TypeDef, offsetof(USART_TypeDef, CR2));
	CREATE_CR_REG(UART_CR3, USART_TypeDef, offsetof(USART_TypeDef, CR3));

	enum class UART_CLOCKSOURCE :uint32_t
	{
		PCLK1      = 0x00U,    /*!< PCLK1 clock source  */
		PCLK2      = 0x01U,    /*!< PCLK2 clock source  */
		HSI        = 0x02U,    /*!< HSI clock source    */
		SYSCLK     = 0x04U,    /*!< SYSCLK clock source */
		LSE        = 0x08U,    /*!< LSE clock source       */
		UNDEFINED  = 0x10U     /*!< Undefined clock source */
	};
	 enum  class UART_FLAG :uint32_t {
		TEACK = ((uint32_t)0x00200000U),
		SBKF = ((uint32_t)0x00040000U),
		CMF = ((uint32_t)0x00020000U),
		BUSY = ((uint32_t)0x00010000U),
		ABRF = ((uint32_t)0x00008000U),
		ABRE = ((uint32_t)0x00004000U),
		EOBF = ((uint32_t)0x00001000U),
		RTOF = ((uint32_t)0x00000800U),
		CTS = ((uint32_t)0x00000400U),
		CTSIF = ((uint32_t)0x00000200U),
		LBDF = ((uint32_t)0x00000100U),
		TXE = ((uint32_t)0x00000080U),
		TC = ((uint32_t)0x00000040U),
		RXNE = ((uint32_t)0x00000020U),
		IDLE = ((uint32_t)0x00000010U),
		ORE = ((uint32_t)0x00000008U),
		NE = ((uint32_t)0x00000004U),
		FE = ((uint32_t)0x00000002U),
		PE = ((uint32_t)0x00000001U),
	 };
	 enum class UART_IT_CLEAR :uint32_t {
		PEF = USART_ICR_PECF,            /*!< Parity Error Clear Flag */
		FEF = USART_ICR_FECF,            /*!< Framing Error Clear Flag */
		NEF = USART_ICR_NCF,             /*!< Noise detected Clear Flag */
		OREF = USART_ICR_ORECF,           /*!< OverRun Error Clear Flag */
		IDLEF = USART_ICR_IDLECF,          /*!< IDLE line detected Clear Flag */
		TCF = USART_ICR_TCCF,            /*!< Transmission Complete Clear Flag */
		LBDF = USART_ICR_LBDCF,           /*!< LIN Break Detection Clear Flag */
		CTSF = USART_ICR_CTSCF,           /*!< CTS Interrupt Clear Flag */
		RTOF = USART_ICR_RTOCF,           /*!< Receiver Time Out Clear Flag */
		EOBF = USART_ICR_EOBCF,           /*!< End Of Block Clear Flag */
		CMF = USART_ICR_CMCF,            /*!< Character Match Clear Flag */
	 };
	 enum class UART_ERROR :uint32_t {
		 NONE      = ((uint32_t)0x00000000U),    /*!< No error            */
		 PE        = ((uint32_t)0x00000001U),    /*!< Parity error        */
		 NE        = ((uint32_t)0x00000002U),    /*!< Noise error         */
		 FE        = ((uint32_t)0x00000004U),    /*!< frame error         */
		 ORE       = ((uint32_t)0x00000008U),    /*!< Overrun error       */
		 DMA       = ((uint32_t)0x00000010U),    /*!< DMA transfer error  */
	 };

	 /** @defgroup UART_Interrupt_definition   UART Interrupts Definition
	   *        Elements values convention: 0000ZZZZ0XXYYYYYb
	   *           - YYYYY  : Interrupt source position in the XX register (5bits)
	   *           - XX  : Interrupt source register (2bits)
	   *                 - 01: CR1 register
	   *                 - 10: CR2 register
	   *                 - 11: CR3 register
	   *           - ZZZZ  : Flag position in the ISR register(4bits)
	   * @{
	   */
	 enum  class UART_IT :uint32_t {
		 PE   = ((uint32_t)0x0028U),
		 TXE =  ((uint32_t)0x0727U),
		 TC =   ((uint32_t)0x0626U),
		 RXNE = ((uint32_t)0x0525U),
		 IDLE = ((uint32_t)0x0424U),
		 LBD =  ((uint32_t)0x0846U),
		 CTS =  ((uint32_t)0x096AU),
		 CM =   ((uint32_t)0x112EU),

/**       Elements values convention: 000000000XXYYYYYb
*           - YYYYY  : Interrupt source position in the XX register (5bits)
*           - XX  : Interrupt source register (2bits)
*                 - 01: CR1 register
*                 - 10: CR2 register
*                 - 11: CR3 register
*/
		 ERR = ((uint32_t)0x0060U),

/**       Elements values convention: 0000ZZZZ00000000b
*           - ZZZZ  : Flag position in the ISR register(4bits)
*/
		 ORE = ((uint32_t)0x0300U),
		 NE = ((uint32_t)0x0200U),
		 FE = ((uint32_t)0x0100U),
	 };
	 enum  class UART_STOPBITS :uint32_t {
		ONE = ((uint32_t)0x00000000U),
		TWO = ((uint32_t)USART_CR2_STOP_1),
	 };
	 CREATE_CR_MASK(UART_CR1, UART_STOPBITS, USART_CR2_STOP_1);

	 enum class UART_PARITY :uint32_t {
		 NONE = ((uint32_t)0x00000000U),
		 EVEN = ((uint32_t)USART_CR1_PCE),
		 ODD = ((uint32_t)USART_CR1_PCE|USART_CR1_PS),
	 };
	 CREATE_CR_MASK(UART_CR1, UART_PARITY, USART_CR1_PCE|USART_CR1_PS);

	 enum class UART_WORDLENGTH :uint32_t {
		 B7 = USART_CR1_M_1,
		 B8 = 0x0000U,
		 B9 = USART_CR1_M_0
	 };
	 CREATE_CR_MASK(UART_CR1, UART_WORDLENGTH, USART_CR1_M_1|USART_CR1_M_0);

	 enum class UART_HWCONTROL:uint32_t  {
		NONE = ((uint32_t)0x00000000U),
		RTS =   ((uint32_t)USART_CR3_RTSE),
		CTS =  ((uint32_t)USART_CR3_CTSE),
		RTS_CTS= ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE))
	 };
	 CREATE_CR_MASK(UART_CR3, UART_HWCONTROL, USART_CR3_RTSE|USART_CR3_CTSE);

	 enum class UART_MODE :uint32_t {
		RX =  ((uint32_t)USART_CR1_RE),
		TX =   ((uint32_t)USART_CR1_TE),
		RX_TX =  ((uint32_t)(USART_CR1_TE |USART_CR1_RE)),
	 };
	 CREATE_CR_MASK(UART_CR1, UART_MODE, USART_CR1_TE |USART_CR1_RE);


	 enum class UART_STATE :uint32_t {
		 DISABLE  =  0x00000000U,
		 ENABLE =   USART_CR1_UE
	 };
	 CREATE_CR_MASK(UART_CR1, UART_STATE, USART_CR1_UE);

	 enum class UART_OVERSAMPLING :uint32_t {
		 X16  =  0x00000000U,
		 X8 =   USART_CR1_OVER8
	 };
	 CREATE_CR_MASK(UART_CR1, UART_OVERSAMPLING, USART_CR1_OVER8);

	 enum class UART_RECEIVER_TIMEOUT :uint32_t {
		 DISABLE  =  0x00000000U,
		ENABLE =   USART_CR2_RTOEN
	 };
	 CREATE_CR_MASK(UART_CR2, UART_RECEIVER_TIMEOUT, USART_CR2_RTOEN);

	 enum class UART_LIN :uint32_t {
		 DISABLE  =  0x00000000U,
		ENABLE =   USART_CR2_LINEN
	 };
	 CREATE_CR_MASK(UART_CR2, UART_LIN, USART_CR2_LINEN);

	 enum class UART_LINBREAKDETECTLENGTH :uint32_t {
		 B10  =  0x00000000U,
		B11 =   USART_CR2_LBDL
	 };
	 CREATE_CR_MASK(UART_CR2, UART_LINBREAKDETECTLENGTH, USART_CR2_LBDL);

	 enum class UART_DMA_TX :uint32_t {
		 DISABLE  =  0x00000000U,
		 ENABLE =   USART_CR3_DMAT
	 };
	 CREATE_CR_MASK(UART_CR3, UART_DMA_TX,  USART_CR3_DMAT);

	 enum class UART_DMA_RX :uint32_t {
		 DISABLE  =  0x00000000U,
		 ENABLE =   USART_CR3_DMAR
	 };
	 CREATE_CR_MASK(UART_CR3, UART_DMA_RX, USART_CR3_DMAR);


	 template<uintptr_t _UART_BASE>
	 struct UART_TRAITS : std::false_type { };

	 template<>
	 struct UART_TRAITS<USART1_BASE> : std::true_type {
		static constexpr uintptr_t UART_BASE = USART1_BASE;
		 static constexpr uintptr_t ISR_BASE_ADDRESS = UART_BASE + 0x1C;
		static constexpr uint32_t RCC_USARTCLKSOURCE_PCLK2  = 0x00000000U;
		static constexpr uint32_t RCC_USARTCLKSOURCE_SYSCLK = RCC_DCKCFGR2_USART1SEL_0;
		static constexpr uint32_t RCC_USARTCLKSOURCE_HSI    = RCC_DCKCFGR2_USART1SEL_1;
		static constexpr uint32_t RCC_USARTCLKSOURCE_LSE    = RCC_DCKCFGR2_USART1SEL;
		// this might need to go into rcc.h
		static UART_CLOCKSOURCE get_clocksource(){
			switch(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART1SEL)) {
				case RCC_USARTCLKSOURCE_PCLK2: return UART_CLOCKSOURCE::PCLK2;
				case RCC_USARTCLKSOURCE_SYSCLK: return UART_CLOCKSOURCE::SYSCLK;
				case RCC_USARTCLKSOURCE_HSI: return UART_CLOCKSOURCE::HSI;
				case RCC_USARTCLKSOURCE_LSE: return UART_CLOCKSOURCE::LSE;
			}
			return UART_CLOCKSOURCE::UNDEFINED;
		}
	 };


	 template<uintptr_t _UART_BASE>
	 class UART{
		 static_assert(UART_TRAITS<_UART_BASE>::value, "Invalid Usart");
		 using traits = UART_TRAITS<_UART_BASE>;
			using cr1_helper = priv::CR_MASK_HELPER<UART_CR1>;
			using cr2_helper = priv::CR_MASK_HELPER<UART_CR2>;
			using cr3_helper = priv::CR_MASK_HELPER<UART_CR3>;



		 /** @brief  BRR division operation to set BRR register with LPUART
			   * @param  _PCLK_: LPUART clock
			   * @param  _BAUD_: Baud rate set by the user
			   * @retval Division result
			   */
			static constexpr uint32_t DIV_LPUART(uint32_t _PCLK_, uint32_t _BAUD_)  { return ((((_PCLK_)*256)+((_BAUD_)/2))/((_BAUD_))); }
			static constexpr uint32_t DIV_SAMPLING8(uint32_t _PCLK_, uint32_t _BAUD_)  { return ((((_PCLK_)*2)+((_BAUD_)/2))/((_BAUD_))); }
			static constexpr uint32_t DIV_SAMPLING16(uint32_t _PCLK_, uint32_t _BAUD_)  { return  ((((_PCLK_))+((_BAUD_)/2))/((_BAUD_))); }

			 static constexpr uint32_t IT_MASK =  0x001FU;
			 USART_TypeDef* _USART;
			 uint32_t _data_mask;
			 bool WaitOnFlagUntilTimeout(UART_FLAG flag, bool status, uint32_t tickstart, uint32_t timeout)
			 {
			   /* Wait until flag is set */
				 while(get_flag(flag) == status){
					 if(timeout == 0U || (HAL_GetTick()-tickstart) >= timeout){
				         /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
					    //CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
					   //CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
						 return true;
					 }
				 }
				 return false;
			 }
			 uint32_t MASK_COMPUTATION() const {
				 switch(get_cr<UART_WORDLENGTH>()){
				 case UART_WORDLENGTH::B7:
					 return get_cr<UART_PARITY>() == UART_PARITY::NONE ? 0x007F : 0x003F;
				 case UART_WORDLENGTH::B9:
					 return get_cr<UART_PARITY>() == UART_PARITY::NONE ? 0x01FF : 0x00FF;
				 case UART_WORDLENGTH::B8:
					 return get_cr<UART_PARITY>() == UART_PARITY::NONE ? 0x00FF : 0x007F;
				 }
				 assert(0); // should never get here
				 return 0xFF;
			 }

			 public:
		  static constexpr uintptr_t UART_BASE=_UART_BASE;
		 constexpr UART() : _USART{ (USART_TypeDef*)UART_BASE }, _data_mask{MASK_COMPUTATION()} {}

#if 0
			template<typename T>
			typename std::enable_if<UART_CONFIG_MASK<T>::value,T>::type
			constexpr get_config() const {
				using cmask = UART_CONFIG_MASK<T>;
				return cmask::cr_reg == 1 ? static_cast<T>(_USART->CR1 & cr_mask<1>(T{})) :
								cmask::cr_reg == 2 ? static_cast<T>(_USART->CR2 & cr_mask<2>(T{})) :
										static_cast<T>(_USART->CR3 & cr_mask<2>(T{})) ;
			}
#endif

		template<typename T>
		T  get_cr() const {
			if(priv::enum_match_reg<T, UART_CR1>::value)
				return cr1_helper::get_single<T>(_USART);
			else if(priv::enum_match_reg<T, UART_CR2>::value)
				return cr2_helper::get_single<T>(_USART);
			else if(priv::enum_match_reg<T, UART_CR3>::value)
				return cr3_helper::get_single<T>(_USART);
			else return T{};
		}
		template<typename ENUM>
		void set_cr(ENUM e) {
			if(priv::enum_match_reg<ENUM, UART_CR1>::value)
				cr1_helper::set_single(_USART,e);
			else if(priv::enum_match_reg<ENUM, UART_CR2>::value)
				cr2_helper::set_single(_USART,e);
			else if(priv::enum_match_reg<ENUM, UART_CR3>::value)
				cr3_helper::set_single(_USART,e);
		}


		template<typename ... Args>
		void set_config(Args... args){
			disable();
			cr3_helper::set_many(_USART,std::forward<Args>(args)...);
			cr2_helper::set_many(_USART,std::forward<Args>(args)...);
			cr1_helper::set_many(_USART,std::forward<Args>(args)...);
		}
		uint32_t read_raw() { return _USART->RDR & _data_mask; }
		void write_raw(uint32_t d) { _USART->TDR = d &_data_mask; }



		int put(int value) {
			if(!get_flag(UART_FLAG::TXE)) return -1;
			write_raw((uint32_t)value);
			return value;
		}

		int get() {
			if(!get_flag(UART_FLAG::RXNE)) return -1;
			return (int)read_raw();
		}
		void write(const char* s) {
			while(*s) {
				if(put(*s) == -1) continue;
				++s;
			}
		}
		void write(const uint8_t* data,size_t len) {
			while(len) {
				if(put(*data) == -1) continue;
				++data;
				--len;
			}
		}
		void read(const uint8_t* data,size_t len){
			while(len){
				int d = get();
				if(d == -1) continue;
				*data++ = (uint8_t)d;
				--len;
			}
		}
		// need some way to reget band rate
		bool set_band_rate(uint32_t band_rate) {
			UART_CLOCKSOURCE clocksource = traits::get_clocksource();
			if(clocksource == UART_CLOCKSOURCE::UNDEFINED) return false;
			uint16_t usartdiv  = 0x0000U;
			uint16_t brrtemp   = 0x0000U;
			  /* Check UART Over Sampling to set Baud Rate Register */
		  if (get_cr<UART_OVERSAMPLING>() == UART_OVERSAMPLING::X8)
		  {
			switch (clocksource)
			{
			case UART_CLOCKSOURCE::PCLK1:
				usartdiv = (uint16_t)DIV_SAMPLING8(HAL_RCC_GetPCLK1Freq(), band_rate);
			  break;
			case UART_CLOCKSOURCE::PCLK2:
				usartdiv = (uint16_t)DIV_SAMPLING8(HAL_RCC_GetPCLK2Freq(), band_rate);
			  break;
			case UART_CLOCKSOURCE::HSI:
				usartdiv = (uint16_t)DIV_SAMPLING8(HSI_VALUE, band_rate);
			  break;
			case UART_CLOCKSOURCE::SYSCLK:
				usartdiv = (uint16_t)DIV_SAMPLING8(HAL_RCC_GetSysClockFreq(), band_rate);
			  break;
			case UART_CLOCKSOURCE::LSE:
				usartdiv = (uint16_t)DIV_SAMPLING8(LSE_VALUE, band_rate);
			  break;
			  case UART_CLOCKSOURCE::UNDEFINED:
				  return false;
			}
			brrtemp = usartdiv & 0xFFF0U;
			brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);

		  }
		  else
		  {
			switch (clocksource)
			{
			case UART_CLOCKSOURCE::PCLK1:
				brrtemp = (uint16_t)DIV_SAMPLING16(HAL_RCC_GetPCLK1Freq(), band_rate);
			  break;
			case UART_CLOCKSOURCE::PCLK2:
				brrtemp = (uint16_t)DIV_SAMPLING16(HAL_RCC_GetPCLK2Freq(), band_rate);
			  break;
			case UART_CLOCKSOURCE::HSI:
				brrtemp = (uint16_t)DIV_SAMPLING16(HSI_VALUE, band_rate);
			  break;
			case UART_CLOCKSOURCE::SYSCLK:
				brrtemp = (uint16_t)DIV_SAMPLING16(HAL_RCC_GetSysClockFreq(), band_rate);
			  break;
			case UART_CLOCKSOURCE::LSE:
				brrtemp = (uint16_t)DIV_SAMPLING16(LSE_VALUE, band_rate);
			  break;
			  case UART_CLOCKSOURCE::UNDEFINED:
				  return false;
			}
		  }
		  _USART->BRR = brrtemp;
		  return true;
		}
		// void clear_flag(UART_IT_CLEAR v) { _USART->ICR = static_cast<uint32_t>(v); }
		 void clear_flag(UART_IT_CLEAR v) { _USART->ICR = static_cast<uint32_t>(v); }
		 inline constexpr bool get_flag(const UART_FLAG f) const {
			 // if we do it this way we save one instruction
			 // that is ldr _USART, ldr ISR, ldr value
			 // to ldr ISR_BASE_ADDRESS, ldr value ... maybe try O1?
			// return (_USART->ISR  & static_cast<uint32_t>(f)) == static_cast<uint32_t>(f);
			 return ( *((volatile uint32_t*)traits::ISR_BASE_ADDRESS)  & static_cast<uint32_t>(f)) == static_cast<uint32_t>(f);

		 }
		 void enable_it(UART_IT v) {
			 uint32_t isr = (1U << (static_cast<uint32_t>(v) & IT_MASK));
			 switch(static_cast<uint8_t>(v) >> 5U){
			 case 1:
				 _USART->CR1 |= isr;
				 break;
			 case 2:
				 _USART->CR2 |= isr;
				 break;
			 default:
				 _USART->CR3 |= isr;
				 break;
		 }
		 }
		 void disable_it(UART_IT v) {
			 uint32_t isr = (1U << (static_cast<uint32_t>(v) & IT_MASK));
			 switch(static_cast<uint8_t>(v) >> 5U){
			 case 1:
				 _USART->CR1 &= ~(isr);
				 break;
			 case 2:
				 _USART->CR2 &= ~(isr);
				 break;
			 default:
				 _USART->CR3 &= ~(isr);
				 break;
		 }
		 }
		 bool get_it(UART_IT v) const { return (_USART->ISR & (1U << ((static_cast<uint32_t>(v))>> 0x08))) != 0; }
		 bool get_it_source(UART_IT v) const {
			 uint32_t isr = (1U << (static_cast<uint32_t>(v) & IT_MASK));
			 switch(static_cast<uint8_t>(v) >> 5U){
			 case 1:
				 return (_USART->CR1 & isr)==isr;
				 break;
			 case 2:
				 return (_USART->CR2 & isr)==isr;
				 break;
			 default:
				 return (_USART->CR3 & isr)==isr;
				 break;
			 }
		 }
		 void enable() {  _USART->CR1 |= USART_CR1_UE; }
		 void disable() {  _USART->CR1 &= ~USART_CR1_UE; }
		 void async_mode() {
			  /* In asynchronous mode, the following bits must be kept cleared:
			  - LINEN and CLKEN bits in the USART_CR2 register,
			  - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
			  CLEAR_BIT(_USART->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
			  CLEAR_BIT(_USART->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
		 }

			template<typename RX_FUNCTION, typename TX_FUNCTION, typename TC_FUNCTION>
				UART_ERROR irq_handler(RX_FUNCTION&& rx_fnc, TX_FUNCTION&& tx_fnc,TC_FUNCTION&& tc_fnc) {
					uint32_t isrflags   = READ_REG(_USART->ISR);
					uint32_t cr1its     = READ_REG(_USART->CR1);
					uint32_t cr3its     = READ_REG(_USART->CR3);
					uint32_t errorflags= (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
					enum_helper<UART_ERROR> err;
					if(errorflags !=0) {
					    /* UART parity error interrupt occurred -------------------------------------*/
					    if(((isrflags & USART_ISR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
					    {
					    	clear_flag(UART_IT_CLEAR::PEF);
					    	err|= UART_ERROR::PE;
					    }

					    /* UART frame error interrupt occurred --------------------------------------*/
					    if(((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
					    {
					    	clear_flag(UART_IT_CLEAR::FEF);
					    	err|= UART_ERROR::FE;
					    }

					    /* UART noise error interrupt occurred --------------------------------------*/
					    if(((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
					    {
					    	clear_flag(UART_IT_CLEAR::NEF);
					    	err|= UART_ERROR::NE;
					    }

					    /* UART Over-Run interrupt occurred -----------------------------------------*/
					    if(((isrflags & USART_ISR_ORE) != RESET) &&
					       (((cr1its & USART_CR1_RXNEIE) != RESET) || ((cr3its & USART_CR3_EIE) != RESET)))
					    {
					    	clear_flag(UART_IT_CLEAR::OREF);
					    	err|= UART_ERROR::ORE;
					    }
					    if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
					    {
							if(!rx_fnc(read_raw())) {
							    /* Disable the UART Parity Error Interrupt and RXNE interrupt*/
							      CLEAR_BIT(_USART->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

							      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
							      CLEAR_BIT(_USART->CR3, USART_CR3_EIE);
							}
					    }
					} else {
					    /* UART in mode Receiver ---------------------------------------------------*/
					    if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
					    {
							if(!rx_fnc(read_raw())) {
							    /* Disable the UART Parity Error Interrupt and RXNE interrupt*/
							      CLEAR_BIT(_USART->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

							      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
							      CLEAR_BIT(_USART->CR3, USART_CR3_EIE);
							}
					    }
						// no errros do do handlers
						  /* UART in mode Transmitter ------------------------------------------------*/
					    else  if(((isrflags & USART_ISR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
						  {
					    	int value = tx_fnc();
					    	if(value == -1){
					    		CLEAR_BIT(_USART->CR1, (USART_CR1_TXEIE));
					    		SET_BIT(_USART->CR1, (USART_CR1_TCIE));
					    	} else {
					    		write_raw(value);
					    	}
						  } else if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
						  {
							  tc_fnc();
							  CLEAR_BIT(_USART->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
						  }
					}
					 return err;
			}
	 };
}
#endif
