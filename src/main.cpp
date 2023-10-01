

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "./delay/delay.h"
#include "./usart/usart.h"
#include <libopencm3/stm32/usart.h>

#define RFID_CS_PORT GPIOA
#define RFID_CS_PIN GPIO4

static void rfid_setup_gpio(void) {
    // Set up RFID chip select pin
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(RFID_CS_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, RFID_CS_PIN);
    gpio_set(RFID_CS_PORT, RFID_CS_PIN);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO5 | GPIO7 );

}

static void rfid_spi_send(uint8_t data) {
    spi_xfer(SPI1, data);
    //while (!(SPI_SR(SPI1) & SPI_SR_RXNE));
    //spi_read(SPI1);
}

int main(void) {
    // Enable clocks
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_SPI1);
    usart_setup();
    char a[1024];
    
gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
				  GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);


    while (1)
	{
		int length = uart_print(a,"a is %d \r\n",10);
        uart_println(a,"length is %d",length);
        // char* ptr = "hello\r\n";
        // for (int i = 0; i < 8; i++)
		// 	usart_send_blocking(USART1, ptr[i]);
        
        gpio_set(GPIOA,GPIO12);
		delay_ms(1000);
        gpio_clear(GPIOA,GPIO12);
        delay_ms(1000);
	}

    // Set up SPI peripheral
    // spi_reset(SPI1);
    // spi_set_master_mode(SPI1);
    // spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_64);
    // spi_set_clock_polarity_1(SPI1);
    // spi_set_clock_phase_1(SPI1);
    // spi_set_dff_8bit(SPI1);
    // spi_set_full_duplex_mode(SPI1);
    // spi_enable_software_slave_management(SPI1);
    // spi_send_msb_first(SPI1);
    // spi_set_nss_high(SPI1);
    // spi_enable(SPI1);

    // // Set up GPIO pins
    // rfid_setup_gpio();

	// gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
	// while (1)
	// {
	// 	gpio_toggle(GPIOA, GPIO8);
	

	// 	gpio_clear(RFID_CS_PORT, RFID_CS_PIN);
    // 	rfid_spi_send(0x55);
    // 	rfid_spi_send(0x01);
    // 	rfid_spi_send(0xAA);
    // 	gpio_set(RFID_CS_PORT, RFID_CS_PIN);
	// 	delay_ms(1000);
	// }


  
}
