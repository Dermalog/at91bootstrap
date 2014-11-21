/*
 * ddramtest.c
 *
 *  Created on: 06.02.2014
 *      Author: ak
 */

#include "ddramtest.h"
#include "watchdog.h"

extern int dbg_printf(const char *fmt_str, ...);
extern int usart1_printf(const char *fmt_str, ...);

#define uint32_t unsigned int

int data_test(uint32_t pattern){
	uint32_t *ddramaddr;

	dbg_printf("write  (%x) :",pattern);
	usart1_printf("write  (%x) :",pattern);

	for (ddramaddr = (uint32_t*)OS_MEM_BANK; ddramaddr < (uint32_t *)(OS_MEM_BANK +OS_MEM_SIZE); ddramaddr++ ){
		*ddramaddr = pattern;
		if ( ( ((uint32_t)ddramaddr)&0x1FFFFF ) == 0){
			dbg_printf("#");
			usart1_printf("#");
			at91_update_wdt();
		}
	}

	dbg_printf("\nverify (%x) :",pattern);
	usart1_printf("\nverify (%x) :",pattern);

	for (ddramaddr = (uint32_t *)OS_MEM_BANK; ddramaddr < (uint32_t *)(OS_MEM_BANK +OS_MEM_SIZE); ddramaddr++ ){

		if ( ( ((uint32_t)ddramaddr)&0x1FFFFF ) == 0){
			dbg_printf("#");
			usart1_printf("#");
			at91_update_wdt();
		}

		if ( *ddramaddr != pattern){
			dbg_printf("\nDDRAM TEST Error: addr:%x expect:%x get:%x\n",ddramaddr,pattern,*ddramaddr);
			usart1_printf("\nDDRAM TEST Error: addr:%x expect:%x get:%x\n",ddramaddr,pattern,*ddramaddr);
			return 0;
		}
	}

	dbg_printf("\n");
	usart1_printf("\n");

	return 1;
}

int addr_test(){
	uint32_t *ddramaddr;

	dbg_printf("write  (addr) :");
	usart1_printf("write  (addr) :");

	for (ddramaddr = (uint32_t *)OS_MEM_BANK; ddramaddr < (uint32_t *)(OS_MEM_BANK +OS_MEM_SIZE); ddramaddr++ ){
		*ddramaddr = (uint32_t)ddramaddr;
		if ( ( ((uint32_t)ddramaddr)&0x1FFFFF ) == 0){
			dbg_printf("#");
			usart1_printf("#");
			at91_update_wdt();
		}
	}

	dbg_printf("\nverify (addr) :");
	usart1_printf("\nverify (addr) :");

	for (ddramaddr = (uint32_t *)OS_MEM_BANK; ddramaddr < (uint32_t *)(OS_MEM_BANK +OS_MEM_SIZE); ddramaddr++ ){
		if ( ( ((uint32_t)ddramaddr)&0x1FFFFF ) == 0){
			dbg_printf("#");
			usart1_printf("#");
			at91_update_wdt();
		}

		if (*ddramaddr != (uint32_t)ddramaddr){
			dbg_printf("\nDDRAM TEST: addr:%x expect:%x get:%x\n",ddramaddr,ddramaddr,*ddramaddr);
			usart1_printf("\nDDRAM TEST: addr:%x expect:%x get:%x\n",ddramaddr,ddramaddr,*ddramaddr);
			return 0;
		}
	}

	dbg_printf("\n");
	usart1_printf("\n");

	return 1;
}

void ddramtest(int cnt){
	int max_cnt = cnt;
	while(cnt--){
		dbg_printf("Test # %x\n",max_cnt-cnt);
		usart1_printf("Test # %x\n",max_cnt-cnt);
		while (data_test(0x00000000)==0);
		while (data_test(0xFFFFFFFF)==0);
		while (data_test(0xAAAAAAAA)==0);
		while (data_test(0x55555555)==0);
		while (addr_test() == 0 );
		dbg_printf("\n");
		usart1_printf("\n");
	}

}
