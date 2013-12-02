#include "common.h"
#include "hardware.h"
#include "board.h"
#include "nandflash.h"
#include <crc.h>
#include "string.h"
#include "nand.h"
#include "flash.h"
#include "usart.h"
#include "debug.h"
#include "div.h"

#include <stdint.h>
#include "memory.h"
#include "stdio.h"

unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

#ifdef _WIN32
struct image_info
{
	unsigned int offset;
	unsigned int length;
	char *filename;
	unsigned char *dest;

	unsigned char of;
	unsigned int of_offset;
	unsigned int of_length;
	char *of_filename;
	unsigned char *of_dest;
};

#define dbg_log(lebele, fmt, ...)  printf(fmt, ## __VA_ARGS__ )
#define dbgu_print(str) printf(str)
#endif

#define BOOTVAR_ADDR 0x40000
#define ENVVAR_ADDR  0x80000
#define VAR_LEN      0x20000
#define VAR_CNT      2

#ifdef _WIN32
/*
int load_nandflash (struct image_info *image){
 int retval;
 size_t read_size;
 FILE *f;

 f = fopen("mtd1","r");
 if (f==NULL){
 return -1;
 }

 retval = fseek(f,image->offset - BOOTVAR_ADDR,SEEK_SET);
 if (retval)
 return retval;

 read_size = fread(image->dest,image->length,1,f);
 if (read_size != 1 )
 return -1 ;

 fclose(f);
 return read_size;
}
int save_nandflash (struct image_info *image){
 int retval;
 size_t write_size;
 FILE *f;

 f = fopen("mtd2","r+");
 if (f==NULL){
 return -1;
 }

 retval = fseek(f,image->offset - BOOTVAR_ADDR,SEEK_SET);
 if (retval)
 return retval;

 write_size = fwrite(image->dest,image->length,1,f);
 if (write_size != 1 )
 return -1 ;

 fclose(f);
 return write_size;
}
*/
#endif

typedef struct _Vars{
	uint32_t crc32;
	uint8_t seq;
	char buf[VAR_LEN-sizeof(uint32_t)-sizeof(uint8_t)];
}Vars;

Vars *vars = (Vars *)(OF_ADDRESS + OF_LENGTH);

int check_crc(const Vars *v){
	return crc32(0x0000L, (const unsigned char*)v->buf, sizeof(v->buf)) == v->crc32 ? 1 : 0;
}

int get_seq_case(const int seq){
	switch (seq ){
	case 0x00: return 0;
		break;
	case 0xff : return 1;
		break;
	default: return 2;
		break;
	}
}

int get_latest_var_id(Vars *v){
	int id;
	const int compare = 2;
	const int wrong = -1;
	const int select_id_table[3][3]={
        { wrong,  0,      1      },
        { 1,      wrong,  0      },  
        { 0,      1,      compare}
    };

	id = select_id_table[get_seq_case(v[0].seq)][get_seq_case(v[1].seq)];
    if (id < 0 ){
		dbg_log(1,"wrong variables combinations\n\r");
		return -1;
    } else if (id == compare ){
		if (vars[0].seq > vars[1].seq)
			id = 0;
		else
			id = 1;
	}

	// here id have a latest one
    if (check_crc(&vars[id]) == 0 ){
        id = (id + 1)%2; // if this wrong crc then try other id
        if (check_crc(&vars[id]) == 0){
			dbg_log(1,"wrong crc32 on variables\n\r");
		}
	}

	return id;
}

char * varenv_read(struct nand_info *nand,int32_t flash_addr ){

	int ret;
	int i;
	int id;

	for (i = 0; i < VAR_CNT; ++i) {
		ret = nand_loadimage(nand, flash_addr + i * VAR_LEN,VAR_LEN,(unsigned char *) &vars[i]);
		if (ret < 0) {
			return NULL ;
		}
	}

	id = get_latest_var_id(&vars[0]);

#if TESTING
	// testing only
	// 0x00 0x00
    vars[0].seq = 0x00; vars[1].seq = 0x00 ; 
    if (get_latest_var_id(&vars[0])>=0){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}
	// 0x00 0xff
    vars[0].seq = 0x00; vars[1].seq = 0xff ; 
    if (get_latest_var_id(&vars[0])!=0){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0x00 0x01
    vars[0].seq = 0x00; vars[1].seq = 0x01 ; 
    if (get_latest_var_id(&vars[0])!=1){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0xff 0x00
    vars[0].seq = 0xff; vars[1].seq = 0x00 ; 
    if (get_latest_var_id(&vars[0])!=1){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0xff 0xff
    vars[0].seq = 0xff; vars[1].seq = 0xff ; 
    if (get_latest_var_id(&vars[0])>=0){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0xff 0xfe
    vars[0].seq = 0xff; vars[1].seq = 0xfe ; 
    if (get_latest_var_id(&vars[0])!=0){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0x01 0x00
    vars[0].seq = 0x01; vars[1].seq = 0x00 ; 
    if (get_latest_var_id(&vars[0])!=0){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0xfe 0xff
    vars[0].seq = 0xfe; vars[1].seq = 0xff ; 
    if (get_latest_var_id(&vars[0])!=1){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0x01 0x02
    vars[0].seq = 0x01; vars[1].seq = 0x02 ; 
    if (get_latest_var_id(&vars[0])!=1){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

	// 0x02 0x01
    vars[0].seq = 0x02; vars[1].seq = 0x01 ; 
    if (get_latest_var_id(&vars[0])!=0){
		dbg_log("%s(%d) wrong result",__FILE__,__LINE__);
		return NULL;
	}

#endif

	return vars[id].buf;
}

typedef struct _bootvar {
	unsigned char new_firmware;
	unsigned char cmd_nr;
} BootVars;

BootVars bootvars;

int32_t fill_char(char *buf, char val) {
	*buf = val;

	return 1;
}

uint32_t fill_hex_int(char *buf, unsigned int data) {
	short num = 0;

	if ((data >> 4) > 0) {
		num += fill_hex_int(buf, data >> 4);
		buf += num;
	}

	if ((data & 0xF) < 10)
		fill_char(buf, (data & 0xF) + '0');
	else
		fill_char(buf, (data & 0xF) - 10 + 'a');
	num++;

	return num;
}

char * num2hex(uint32_t num) {
	static char p[10];
	fill_hex_int(p, num);
	return p;
}

char *strafter(char *str, char after) {
	char *p = NULL;
	while (*str && *str != after) {
		str++;
	}
	p = str + 1;
	return p;
}

uint64_t hex2num(char *hex) {
	uint64_t num = 0x0000;
	while (*hex) {
		if (*hex >= '0' && *hex <= '9') {
			num *= 16;
			num += *hex - (int) '0';
		} else if (*hex >= 'A' && *hex <= 'F') {
			num *= 16;
			num += *hex - (int) 'A' + 10;
		} else if (*hex >= 'a' && *hex <= 'f') {
			num *= 16;
			num += *hex - (int) 'a' + 10;
		} else
			break;
		hex++;
	}
	return num;
}

int bootvars_write(struct nand_info * nand, uint32_t flash_addr) {
	uint8_t id, seq;
	uint32_t row_address;
	int32_t res;
	// struct image_info image;
	char *p;
	id = get_latest_var_id(&vars[0]);
	seq = vars[id].seq + 1;
	id = (id + 1) % 2;
	vars[id].seq = seq;
	p = vars[id].buf;
	memset(p, 0, sizeof(vars[id].buf));
	strcpy(p, "new_firmware=");
	p += strlen(p);
	fill_hex_int(p, bootvars.new_firmware);
	p += strlen(p);
	p++; // skip '\0' to separate variables.
	strcpy(p, "cmd_nr=");
	p += strlen(p);
	fill_hex_int(p, bootvars.cmd_nr);
	p += strlen(p);
	vars[id].crc32 = crc32(0, (const unsigned char *) vars[id].buf,
			sizeof(vars[id].buf));
	//image.dest = (unsigned char *)&vars[id];
	//image.offset = flash_addr+id*VAR_LEN;
	//image.length = VAR_LEN;
	row_address = div(flash_addr + id * VAR_LEN, nand->pagesize);

	res = nand_erase_block(nand, row_address);
	if (res < 0)
		dbg_log(1,"nand_erase_block failed: error code : %d\n",res);

	res = nand_write_block(nand, row_address, (uint8_t *) &vars[id]);
	if (res < 0)
		dbg_log(1,"nand_write_block failed: error code : %d\n",res);

	return 0; //save_nandflash(&image);
}

void bootvars_decode(char *p) {
	int len;

	memset(&bootvars, 0, sizeof(bootvars));

	while ((len = strlen(p)) != 0) {
		if (strncmp(p, "new_firmware", 12) == 0) {
			bootvars.new_firmware = hex2num(strafter(p, '='));
		} else if (strncmp(p, "cmd_nr", 6) == 0) {
			bootvars.cmd_nr = hex2num(strafter(p, '='));
		}

		p += len + 1;
	}
}

typedef struct _envvar {
	char cmd[300];
	uint32_t kernelAddr;
	uint32_t dtbAddr;
	uint32_t macAddr[2];
} EnvVars;

EnvVars envvars[2];

void envvar_decode(char * p) {
	int len;

	memset(&envvars[0], 0, sizeof(envvars));

	while ((len = strlen(p)) != 0) {
		if (strncmp(p, "cmd0", 4) == 0)
			strcpy(envvars[0].cmd, strafter(p, '='));
		else if (strncmp(p, "cmd1", 4) == 0)
			strcpy(envvars[1].cmd, strafter(p, '='));
		else if (strncmp(p, "kernelAddr0", 11) == 0)
			envvars[0].kernelAddr = hex2num(strafter(strafter(p, '='), 'x'));
		else if (strncmp(p, "kernelAddr1", 11) == 0)
			envvars[1].kernelAddr = hex2num(strafter(strafter(p, '='), 'x'));
		else if (strncmp(p, "dtbAddr0", 8) == 0)
			envvars[0].dtbAddr = hex2num(strafter(strafter(p, '='), 'x'));
		else if (strncmp(p, "dtbAddr1", 8) == 0)
			envvars[1].dtbAddr = hex2num(strafter(strafter(p, '='), 'x'));
		else if (strncmp(p, "macAddr", 7) == 0){
			uint64_t macAddr;
			uint64_t h = hex2num(strafter(strafter(p, '='), 'x'));
			macAddr = ((h & 0xFF)<<40) |
					(((h >> 8) & 0xFF)<< 32) |
					(((h >> 16) & 0xFF)<< 24) |
					(((h >> 24) & 0xFF)<< 16) |
					(((h >> 32) & 0xFF)<< 8) |
					(((h >> 40) & 0xFF))
			;
			envvars[0].macAddr[0] = macAddr >>32 &0xFFFFFFFFUL;
			envvars[0].macAddr[1] = macAddr & 0xFFFFFFFFUL;
		}
		p += len + 1;
	}

}

void dump(unsigned char *buf, int len)
{
	int i = 0;
	static const char hex[]="0123456789ABCDEF";

	for (i = 0; i < len; i++) {
		if (i % 16 == 0)
			dbg_log(1, "\r\n");
		if (i % 16 == 0)
			dbg_log(1, "%x : ",buf+i);
		dbg_log(1,"%c%c ", hex[buf[i]>>4],  hex[buf[i]&0x0F] );
	}
	dbg_log(1, "\r\n");
}

int env_main(struct nand_info *nand, struct image_info *image) {
	char * p;
	uint8_t cmd_nr;
	char *cmd=NULL;
	uint32_t kernelAddr=0;
	uint32_t dtbAddr=0;

	p = varenv_read(nand, BOOTVAR_ADDR);
	dump((unsigned char *)vars,0x20);
	if (p == NULL ) {
		dbg_log(1,"ENV:Failed to load bootvar region from FLASH\n\r");
		return -1;
	}

	bootvars_decode(p);
	dbg_log(1,"new_firmware = %x\n\r",bootvars.new_firmware);
	dbg_log(1,"cmd_nr       = %x\n\r",bootvars.cmd_nr);

	if (bootvars.new_firmware) {
		cmd_nr = (bootvars.cmd_nr + 1) % 2;
		bootvars.new_firmware = 0;
		bootvars_write(nand, BOOTVAR_ADDR);
	} else
		cmd_nr = bootvars.cmd_nr;

	p = varenv_read(nand, ENVVAR_ADDR);
	if (p == NULL ) {
		dbg_log(1,"ENV:Failed to load envvar region from FLASH\n\r");
		return -1;
	}

	envvar_decode(p);
	cmd = envvars[cmd_nr].cmd;
	kernelAddr = envvars[cmd_nr].kernelAddr;
	dtbAddr = envvars[cmd_nr].dtbAddr;
	usart_puts(cmd);


#ifdef CONFIG_NANDFLASH
	if (kernelAddr)
		image->offset = kernelAddr;
	else
		image->offset = IMG_ADDRESS;

	image->length = IMG_SIZE;
#ifdef CONFIG_OF_LIBFDT
	if (dtbAddr)
		image->of_offset = dtbAddr;
	else
		image->of_offset = OF_OFFSET;

	image->of_length = OF_LENGTH;
#endif
#endif

	if (strlen(cmd))
		image->linux_kernel_arg_string = cmd;

	if (envvars[0].macAddr[1]!=0){
		writel( 0x504D4300, AT91C_PMC_WPMR);

		writel(1 << (AT91C_ID_GMAC-32), AT91C_PMC_PCER1);
		writel(1 << (AT91C_ID_EMAC-32), AT91C_PMC_PCER1);

		writel( envvars[0].macAddr[1], MACB_SA1B );
		writel( envvars[0].macAddr[0], MACB_SA1T );
		writel( envvars[0].macAddr[1], GEM_SA1B  );
		writel( envvars[0].macAddr[0], GEM_SA1T  );
	}


	dbg_log(1,"\n\r"); dbg_log(1,"kernelAddr=%x\n\r",kernelAddr); dbg_log(1,"dtbAddr=%x\n\r",dtbAddr);
	return 0;
}

