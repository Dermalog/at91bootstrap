/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2006, Atmel Corporation

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
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
#include "common.h"
#include "hardware.h"
#include "board.h"
#include "usart.h"
#include "debug.h"
#include "slowclk.h"
#include "dataflash.h"
#include "nandflash.h"
#include "sdcard.h"
#include "flash.h"
#include "string.h"
#include "onewire_info.h"
#include "env.h" 
#include "nand.h"
#include "ddramtest.h"

extern int load_kernel(struct nand_info *nand, struct image_info *img_info);

typedef int (*load_function)(struct nand_info *nand, struct image_info *img_info);

void (*sdcard_set_of_name)(char *) = NULL;

static void display_banner (void)
{
	char *version = "AT91Bootstrap";
	char *ver_num = " "AT91BOOTSTRAP_VERSION" ("COMPILE_TIME")";

	dbgu_puts(version);
	dbgu_puts(ver_num);
	dbgu_puts("\n");
}
#if 0
int main(void)
{
	struct image_info image;
	char *media_str = NULL;
	int ret;

	char filename[FILENAME_BUF_LEN];
	char of_filename[FILENAME_BUF_LEN];

	memset(&image, 0, sizeof(image));
	memset(filename, 0, FILENAME_BUF_LEN);
	memset(of_filename, 0, FILENAME_BUF_LEN);

	image.dest = (unsigned char *)JUMP_ADDR;
#ifdef CONFIG_OF_LIBFDT
	image.of = 1;
	image.of_dest = (unsigned char *)OF_ADDRESS;
#endif

#ifdef CONFIG_NANDFLASH
	media_str = "NAND: ";
	image.offset = IMG_ADDRESS;
	image.length = IMG_SIZE;
#ifdef CONFIG_OF_LIBFDT
	image.of_offset = OF_OFFSET;
	image.of_length = OF_LENGTH;
#endif
#endif

#ifdef CONFIG_DATAFLASH
	media_str = "SF: ";
	image.offset = IMG_ADDRESS;
	image.length = IMG_SIZE;
#ifdef CONFIG_OF_LIBFDT
	image.of_offset = OF_OFFSET;
	image.of_length = OF_LENGTH;
#endif
#endif

#ifdef CONFIG_SDCARD
	media_str = "SD/MMC: ";
	image.filename = filename;
	strcpy(image.filename, OS_IMAGE_NAME);
#ifdef CONFIG_OF_LIBFDT
	image.of_filename = of_filename;
	strcpy(image.of_filename, OF_FILENAME);
#endif
#endif

#ifdef CONFIG_HW_INIT
	hw_init();
#endif

	display_banner();

#ifdef CONFIG_LOAD_ONE_WIRE
	/* Load one wire informaion */
	load_1wire_info();
#endif
	init_loadfunction();

	ret = (*load_image)(&image);

	if (media_str)
		usart_puts(media_str);

	if (ret == 0){
		usart_puts("Done to load image\n");
	}
	if (ret == -1) {
		usart_puts("Failed to load image\n");
		while(1);
	}
	if (ret == -2) {
		usart_puts("Success to recovery\n");
		while (1);
	}

#ifdef CONFIG_SCLK
	slowclk_switch_osc32();
#endif

	return JUMP_ADDR;
}
#endif
int main(void){
	struct image_info image;
	struct nand_info nand;
	memset(&image, 0, sizeof(image));

	hw_init();

	nand_init(&nand);

	display_banner();

	env_main(&nand, &image);

	image.dest = (unsigned char *)JUMP_ADDR;
#ifdef CONFIG_OF_LIBFDT
	image.of = 1;
	image.of_dest = (unsigned char *)OF_ADDRESS;
#endif

	load_kernel(&nand,&image);

	return JUMP_ADDR;
}
