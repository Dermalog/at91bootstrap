/*
 * at91_usart.h
 *
 *  Created on: 20.11.2014
 *      Author: ak
 */

#ifndef AT91_USART_H_
#define AT91_USART_H_

/* *** Register offset in AT91S_USART structure ***/
#define USART_CR	0x00	/* Control Register */
#define USART_MR	0x04	/* Mode Register */
#define USART_IER	0x08	/* Interrupt Enable Register */
#define USART_IDR	0x0C	/* Interrupt Disable Register */
#define USART_IMR	0x10	/* Interrupt Mask Register */
#define USART_CSR	0x14	/* Channel Status Register */
#define USART_RHR	0x18	/* Receiver Holding Register */
#define USART_THR	0x1C	/* Transmitter Holding Register */
#define USART_BRGR	0x20	/* Baud Rate Generator Register */
#define USART_CIDR	0x40	/* Chip ID Register */
#define USART_EXID	0x44	/* Chip ID Extension Register */
#define USART_FNTR	0x48	/* Force NTRST Register */

/* -------- USART_CR : (USART Offset: 0x0) USART Control Register --------*/
#define AT91C_USART_RSTRX	(0x1UL << 2)
#define AT91C_USART_RSTTX	(0x1UL << 3)
#define AT91C_USART_RXEN	(0x1UL << 4)
#define AT91C_USART_RXDIS	(0x1UL << 5)
#define AT91C_USART_TXEN	(0x1UL << 6)
#define AT91C_USART_TXDIS	(0x1UL << 7)
#define AT91C_USART_RSTSTA	(0x1UL << 8)

/* -------- USART_MR : (USART Offset: 0x4) USART Mode Register --------*/
#define AT91C_USART_CHRL		(0x3UL << 6)
#define		AT91C_USART_CHRL_5BIT			(0x0UL << 6)
#define		AT91C_USART_CHRL_6BIT			(0x1UL << 6)
#define		AT91C_USART_CHRL_7BIT			(0x2UL << 6)
#define		AT91C_USART_CHRL_8BIT			(0x3UL << 6)
#define AT91C_USART_PAR		(0x7UL << 9)
#define 	AT91C_USART_PAR_EVEN			(0x0UL << 9)
#define 	AT91C_USART_PAR_ODD				(0x1UL << 9)
#define 	AT91C_USART_PAR_SPACE			(0x2UL << 9)
#define 	AT91C_USART_PAR_MARK			(0x3UL << 9)
#define 	AT91C_USART_PAR_NONE			(0x4UL << 9)
#define AT91C_USART_NBSTOP	(0x3UL << 12)
#define		AT91C_USART_NBSTOP_1BIT			(0x0UL << 12)
#define		AT91C_USART_NBSTOP_1_5BIT		(0x1UL << 12)
#define		AT91C_USART_NBSTOP_2BIT			(0x2UL << 12)
#define AT91C_USART_CHMODE	(0x3UL << 14)
#define 	AT91C_USART_CHMODE_NORMAL		(0x0UL << 14)
#define 	AT91C_USART_CHMODE_AUTO			(0x1UL << 14)
#define 	AT91C_USART_CHMODE_LOCAL		(0x2UL << 14)
#define 	AT91C_USART_CHMODE_REMOTE		(0x3UL << 14)

/* -------- USART_IER : (USART Offset: 0x8) USART Interrupt Enable Register -------- */
#define AT91C_USART_RXRDY	(0x1UL <<  0)
#define AT91C_USART_TXRDY	(0x1UL <<  1)

#endif /* AT91_USART_H_ */
