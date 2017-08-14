
#include "main.h"
#include "stm32\dma.h"
#include "stm32\uart.h"
#include <cstring>
#include <cassert>
#include <cstdarg>

stm32::DMA<DMA2_Stream7_BASE> uart_tx_dma;
stm32::UART<USART1_BASE> uart1;
#define DMA_TX_BUF_SIZE 64
uint32_t tx_buffer[DMA_TX_BUF_SIZE];
uint8_t* tx_begin= (uint8_t*)tx_buffer;
uint8_t* tx_end = tx_begin + DMA_TX_BUF_SIZE;
uint8_t* tx_head = tx_begin;
uint8_t* tx_tail = tx_begin;
volatile bool uart_trasmiting = false;

static UART_HandleTypeDef txt_usart;

void tx_flush() {
	static size_t flush_count = 0;
	if(uart_trasmiting) return;
	uart_trasmiting= true;
	++flush_count;
	if(tx_head != tx_tail) {
		uint8_t* ptr = tx_tail;
		size_t count = (size_t)(tx_head < tx_tail ? tx_end - tx_tail : tx_head - tx_tail);
		printf("Sending Text size=%u\r\n", count);
		tx_tail+=count;
		if(tx_tail > tx_end) tx_tail = tx_begin;
	    /* Clear the TC flag in the SR register by writing 0 to it */
	    uart1.clear_flag(stm32::UART_FLAG::TC);
	    uart1.set_cr(stm32::UART_DMA_TX::ENABLE);
		uart_tx_dma.enable_it(stm32::DMA_IT::TC);
		uart_tx_dma.start(ptr, &USART1->TDR, count);
	}
}
extern "C" void USARTx_IRQHandler() {
	if(uart1.get_it_source(stm32::UART_IT::TC) && uart1.get_flag(stm32::UART_FLAG::TC)){
		uart1.disable_it(stm32::UART_IT::TC);
		uart1.clear_flag(stm32::UART_FLAG::TC);
		uart_trasmiting = false;
		return;
	}
	assert(0);
}
extern "C" void DMA2_Stream7_IRQHandler() {
	stm32::DMA_ERROR err = uart_tx_dma.irq_handler(
			[](uintptr_t mem) {
			// CLEAR_BIT(txt_usart.Instance->CR3, USART_CR3_DMAT);
			uart1.set_cr(stm32::UART_DMA_TX::DISABLE);
			uart1.enable_it(stm32::UART_IT::TC);
		}
	);
#if 0
	if(uart_tx_dma.get_transfer_complete_flag()){

	    /* Disable the DMA transfer for transmit request by setting the DMAT bit
	       in the UART CR3 register */
	    CLEAR_BIT(txt_usart.Instance->CR3, USART_CR3_DMAT);

	    /* Enable the UART Transmit Complete Interrupt */
	   // SET_BIT(&txt_usart, USART_CR1_TCIE);
		uart_tx_dma.disable();
		uart_tx_dma.clear_transfer_complete_flag();
		tx_flush();
		return;
	}
#endif
	assert(err == stm32::DMA_ERROR::NONE);
}


static void TXT_USART_Pin_Init() {
	  GPIO_InitTypeDef  GPIO_InitStruct;

	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* Enable GPIO TX/RX clock */
	  USARTx_TX_GPIO_CLK_ENABLE();
	  USARTx_RX_GPIO_CLK_ENABLE();


	  /* Enable USARTx clock */
	  USARTx_CLK_ENABLE();

	  /*##-2- Configure peripheral GPIO ##########################################*/
	  /* UART TX GPIO pin configuration  */
	  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	  //GPIO_InitStruct.Pull      = GPIO_PULLUP;

	  GPIO_InitStruct.Pull      = GPIO_NOPULL;
	  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	  //GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Alternate = USARTx_TX_AF;

	  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	  /* UART RX GPIO pin configuration  */
	  GPIO_InitStruct.Pin = USARTx_RX_PIN;
	  GPIO_InitStruct.Alternate = USARTx_RX_AF;

	  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

	  /*##-3- Configure the NVIC for UART ########################################*/
	  /* NVIC for USART */
	  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
	  HAL_NVIC_EnableIRQ(USARTx_IRQn);

	  HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 0, 1);
	  HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

}

static void TXT_USART_Pin_DeInit() {
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

  /*##-3- Disable the NVIC for UART ##########################################*/
  HAL_NVIC_DisableIRQ(USARTx_IRQn);
}

void dma_watch_dma() {
	size_t prevCNDTR = uart_tx_dma.get_counter();
	while(1) {
		size_t currCNDTR = uart_tx_dma.get_counter();
		if(prevCNDTR != currCNDTR){
			size_t pos = (currCNDTR < DMA_TX_BUF_SIZE) ? (DMA_TX_BUF_SIZE - currCNDTR) : 0;
			uintptr_t mem0 = uart_tx_dma.get_memory_address0();
			uintptr_t peri = uart_tx_dma.get_peripheral_address();
			printf("CNDR(%u,%u) pos=%u mem0=%u peri=%u\r\n", prevCNDTR, currCNDTR,pos,mem0,peri  );
			prevCNDTR=currCNDTR;
		}

	}
}
void dma_test_print(const char* s) {
	do {
		uint8_t* next_head = tx_head+1;
		if(next_head >= tx_end) next_head = tx_begin;
		if(next_head == tx_tail) {
			tx_flush();
			continue;
		}
		if(*s == '\0') break;
		*tx_head = *s++;
		tx_head = next_head;
	} while(1);
	tx_flush();
}
void dma_test_printf(const char* fmt,...) {
	char buffer[128]; // humm
	va_list va;
	va_start(va,fmt);
	vsnprintf(buffer,128-1,fmt,va);
	va_end(va);
	dma_test_print(buffer);
}
int TXT_Init(void)
{
	// inital usart1 using the STM32 hall driver
	// maybe go to openstm latter?
	TXT_USART_Pin_Init();
#if 0
	txt_usart.Instance = USART1;
	txt_usart.Init.BaudRate = 115200;
	//txt_usart.Init.BaudRate =57600;
	//txt_usart.Init.BaudRate =9600;
	txt_usart.Init.WordLength = UART_WORDLENGTH_8B;
	txt_usart.Init.StopBits = UART_STOPBITS_1;
	txt_usart.Init.Parity = UART_PARITY_NONE;
	txt_usart.Init.Mode = UART_MODE_TX_RX;
	txt_usart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	txt_usart.Init.OverSampling = UART_OVERSAMPLING_16;
	txt_usart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	txt_usart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	assert (HAL_UART_Init(&txt_usart) == HAL_OK);
#endif

	//test_print("Usart blocking test complete!\n\r");
	uart1.set_config(
			stm32::UART_WORDLENGTH::B8,
			stm32::UART_STOPBITS::ONE,
			stm32::UART_PARITY::NONE,
			stm32::UART_MODE::RX_TX,
			stm32:: UART_HWCONTROL::NONE,
			stm32:: UART_OVERSAMPLING::X16
	);
	uart1.set_band_rate(115200);
	uart1.enable();

	return 0;
}
void test_dma_serial(){
	TXT_Init();

	uart_tx_dma.clear_config();
	uart_tx_dma.set_config(
			stm32::DMA_DIR::MEMORY_TO_PERIPH,
			stm32::DMA_PINC::DISABLE,
			stm32::DMA_MINC::ENABLE,
			//stm32::DMA_MODE::CIRCULAR,
			stm32::DMA_MODE::NORMAL,
			stm32::DMA_PRIORITY::LOW,
			stm32::DMA_MDATAALIGN::BYTE,
			stm32::DMA_CHANNEL::N4
	);
    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
   // txt_usart.Instance->CR3 |= USART_CR3_DMAT;

	// setup uhard to do dma
    /* Clear the TC flag in the SR register by writing 0 to it */
    //__HAL_UART_CLEAR_IT(&txt_usart, UART_FLAG_TC);
    uint32_t count = 0;
    while(1){
		printf("Tick(%u)!\r\n",count);
		dma_test_printf("Usart DMA test! %u\r\n", count);
		HAL_Delay(1000);
		count++;
    }

    //dma_watch_dma();
	while(1) {

	}
}
