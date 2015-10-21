/*
 * atomic_fs.h
 *
 *  Created on: 21 ott 2015
 *      Author: Atomic Studio Development 
 *	Ringraziamenti: Samsung Electronics 
 *	Altri riferimenti: linux kernel
 *      
 *      
 */

#ifndef INCLUDE_ATOMIC_FS_H_
#define INCLUDE_ATOMIC_FS_H_


#include<inttypes.h>           /* libreria per la gestione della dimensione degli interi*/
#include<linux/types.h>        /* libreria per la gestione delle piccole porzioni di codice riguardo dimensioni predefinite dal sistema*/
#include<sys/types.h>		   /* libreria per la gestione delle piccole porzioni di codice riguardo dimensioni predefinite dal sistema*/

/* Sezione dedicata all'inclusione delle lib personali*/




//compilazione non predefinita della libreria
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

/* Definizione deitipipredefiniti usati nel  sistema operativo*/
typedef u_int64_t  		u64;
typedef u_int32_t		u32;
typedef u_int16_t		u16;
typedef u_int8_t		u8;

/* Definizione della dimensione dei blocchi di codice del bootloader*/
typedef u32				block_t;
typedef u32				nid_t;
typedef u8				bool;
typedef unsigned long 	pgoff_t;


/*Inclusione della libreria byteswap.h che dato un valore lo converte in un 16 bit.....Esempio:4 bit che contengono 1024 facendo uno swap si puo mettere in soli 2*/


static inline uint16_t bswap_16(uint16_t value)
{
   /* Fa il return dello stesso valore solo che con i bit sposati */
	return ((value &(uint16_t)0x00ffU)<<8) /* valore spostato di 8 bit*/
			| ((value &(uint16_t)0x00ffU)>>8);
}

static inline uint32_t bswap_32(uint32_t val)
{
	return ((val & (uint32_t)0x000000ffUL) << 24)
		| ((val & (uint32_t)0x0000ff00UL) <<  8)
		| ((val & (uint32_t)0x00ff0000UL) >>  8)
		| ((val & (uint32_t)0xff000000UL) >> 24);
}
#endif

#if defined HAVE_DECL_BSWAP_64 && !HAVE_DECL_BSWAP_64

static inline uint64_t bswap_64(uint64_t val)
{
	return ((val & (uint64_t)0x00000000000000ffULL) << 56)
		| ((val & (uint64_t)0x000000000000ff00ULL) << 40)
		| ((val & (uint64_t)0x0000000000ff0000ULL) << 24)
		| ((val & (uint64_t)0x00000000ff000000ULL) <<  8)
		| ((val & (uint64_t)0x000000ff00000000ULL) >>  8)
		| ((val & (uint64_t)0x0000ff0000000000ULL) >> 24)
		| ((val & (uint64_t)0x00ff000000000000ULL) >> 40)
		| ((val & (uint64_t)0xff00000000000000ULL) >> 56);
}
#endif

/* Controllo del tipo di sistema che abbiamo in uso*/
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define le16_to_cpu(x)	((__u16)(x))
#define le32_to_cpu(x)	((__u32)(x))
#define le64_to_cpu(x)	((__u64)(x))
#define cpu_to_le16(x)	((__u16)(x))
#define cpu_to_le32(x)	((__u32)(x))
#define cpu_to_le64(x)	((__u64)(x))
#elif __BYTE_ORDER == __BIG_ENDIAN
#define le16_to_cpu(x)	bswap_16(x)
#define le32_to_cpu(x)	bswap_32(x)
#define le64_to_cpu(x)	bswap_64(x)
#define cpu_to_le16(x)	bswap_16(x)
#define cpu_to_le32(x)	bswap_32(x)
#define cpu_to_le64(x)	bswap_64(x)
#endif


/* Funzioni di controllo del tipo di paramentro passato*/
#define typecheck(type,x) \
	({	type __dummy; \
		typeof(x) __dummy2; \
		(void)(&__dummy == &__dummy2); \
		1; \
	 })

/* Funzioni per il debug*/
#define FIX_MSG(fmt, ...)												\
	do {																\
		printf("[FIX] (%s:%4d) ", __func__, __LINE__);					\
		printf(" --> "fmt"\n", ##__VA_ARGS__);							\
	} while (0)

#define ASSERT(exp)														\
	do{																	\
		if(!(exp))														\
		{																\
			printf("[ASSERT](%s:%4d) " #exp"\n",__func__,__LINE__);		\
			exit(-1);													\
		}																\
	}while(0)															\

#define ERR_MSG(fmt, ...)												\
		do {															\
			printf("[%s:%d] " fmt, __func__, __LINE__, ##__VA_ARGS__); 	\
		} while (0)

#define MSG(n, fmt, ...)												\
	do {																\
		if (config.dbg_lv >= n) {										\
			printf(fmt, ##__VA_ARGS__);									\
		}																\
	} while (0)

#define DBG(n, fmt, ...)												\
	do {																\
		if (config.dbg_lv >= n) {										\
			printf("[%s:%4d] " fmt,										\
				__func__, __LINE__, ##__VA_ARGS__);						\
		}																\
	} while (0)

/*Messaggi che possono essere scritti sulla console*/
#define DISP(fmt, ptr, member)											\
	do {																\
		printf("%-30s" fmt, #member, ((ptr)->member));					\
	} while (0)

#define DISP_u32(ptr, member)											\
	do {																\
		assert(sizeof((ptr)->member) <= 4);								\
		printf("%-30s" "\t\t[0x%8x : %u]\n",							\
			#member, ((ptr)->member), ((ptr)->member));					\
	} while (0)

#define DISP_u64(ptr, member)											\
	do {																\
		assert(sizeof((ptr)->member) == 8);								\
		printf("%-30s" "\t\t[0x%8llx : %llu]\n",						\
			#member, ((ptr)->member), ((ptr)->member));					\
	} while (0)

#define DISP_utf(ptr, member)											\
	do {																\
		printf("%-30s" "\t\t[%s]\n", #member, ((ptr)->member)); 		\
	} while (0)

/* Queste sono quelle che mando nel buffer direttamente*/
#define BUF_DISP_u32(buf, data, len, ptr, member)						\
	do {																\
		assert(sizeof((ptr)->member) <= 4);								\
		snprintf(buf, len, #member);									\
		snprintf(data, len, "0x%x : %u", ((ptr)->member),				\
						((ptr)->member));								\
	} while (0)

#define BUF_DISP_u64(buf, data, len, ptr, member)						\
	do {																\
		assert(sizeof((ptr)->member) == 8);								\
		snprintf(buf, len, #member);									\
		snprintf(data, len, "0x%llx : %llu", ((ptr)->member),			\
						((ptr)->member));								\
	} while (0)

#define BUF_DISP_utf(buf, data, len, ptr, member)						\
		snprintf(buf, len, #member)



