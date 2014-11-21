/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2010, Atmel Corporation

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "hardware.h"
#include "board.h"
#include "arch/at91_dbgu.h"
#include "arch/at91_usart.h"

#ifndef USART_BASE
#define USART_BASE	AT91C_BASE_DBGU
#endif

#ifndef USART1_BASE
#define USART1_BASE	AT91C_BASE_USART1
#endif

static inline void write_dbgu(unsigned int offset, const unsigned int value)
{
	writel(value, offset + USART_BASE);
}

static inline unsigned int read_dbgu(unsigned int offset)
{
	return readl(offset + USART_BASE);
}

static inline void write_usart1(unsigned int offset, const unsigned int value)
{
	writel(value, offset + USART1_BASE);
}

static inline unsigned int read_usart1(unsigned int offset)
{
	return readl(offset + USART1_BASE);
}

void dbgu_init(unsigned int baudrate)
{
	/* Disable interrupts */
	write_dbgu(DBGU_IDR, -1);

	/* Reset the receiver and transmitter */
	write_dbgu(DBGU_CR, AT91C_DBGU_RSTRX
				| AT91C_DBGU_RSTTX
				| AT91C_DBGU_RXDIS
				| AT91C_DBGU_TXDIS);

	/* Configure the baudrate */
	write_dbgu(DBGU_BRGR, baudrate);

	/* Configure USART in Asynchronous mode */
	write_dbgu(DBGU_MR, AT91C_DBGU_PAR_NONE
				| AT91C_DBGU_CHMODE_NORMAL
				| AT91C_DBGU_CHRL_8BIT
				| AT91C_DBGU_NBSTOP_1BIT);

	/* Enable RX and Tx */
	write_dbgu(DBGU_CR, AT91C_DBGU_RXEN | AT91C_DBGU_TXEN);
}

void usart1_init(unsigned int baudrate)
{
	/* Disable interrupts */
	write_usart1(USART_IDR, -1);

	/* Reset the receiver and transmitter */
	write_usart1(USART_CR, AT91C_USART_RSTRX
				| AT91C_USART_RSTTX
				| AT91C_USART_RXDIS
				| AT91C_USART_TXDIS);

	/* Configure the baudrate */
	write_usart1(USART_BRGR, baudrate);

	/* Configure USART in Asynchronous mode */
	write_usart1(USART_MR, AT91C_USART_PAR_NONE
				| AT91C_USART_CHMODE_NORMAL
				| AT91C_USART_CHRL_8BIT
				| AT91C_USART_NBSTOP_1BIT);

	/* Enable RX and Tx */
	write_usart1(USART_CR, AT91C_DBGU_RXEN | AT91C_DBGU_TXEN);
}

static void dbgu_putc(const char c)
{
	while (!(read_dbgu(DBGU_CSR) & AT91C_DBGU_TXRDY))
		;

	write_dbgu(DBGU_THR, c);
}

void dbgu_puts(const char *ptr)
{
	int i = 0;

	while (ptr[i] != '\0') {
		if (ptr[i] == '\n')
			dbgu_putc('\r');
		dbgu_putc(ptr[i]);
		i++;
	}
}

char dbgu_getc(void)
{
	while (!(read_dbgu(DBGU_CSR) & AT91C_DBGU_RXRDY))
		;

	return (char)read_dbgu(DBGU_RHR);
}

// --------------
static void usart1_putc(const char c)
{
	while (!(read_usart1(USART_CSR) & AT91C_USART_TXRDY))
		;

	write_usart1(DBGU_THR, c);
}

void usart1_puts(const char *ptr)
{
	int i = 0;

	while (ptr[i] != '\0') {
		if (ptr[i] == '\n')
			usart1_putc('\r');
		usart1_putc(ptr[i]);
		i++;
	}
}

char usart1_getc(void)
{
	while (!(read_usart1(USART_CSR) & AT91C_USART_RXRDY))
		;

	return (char)read_usart1(USART_RHR);
}
