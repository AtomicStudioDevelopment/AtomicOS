/*
 * atomic_fs.h
 *
 *  Created on: 21 ott 2015
 *      Author: Atomic Studio Development
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


/* Qui inizia la dichiarazione delle proprietà del sistema
 * Alcune di queste stanno gia definite nel kernel di linux
 * - page size
 * - page cache size
 * - bits per byte
 * - checksum offset
 */

#define PAGE_SIZE					4096    		//4 GB
#define PAGE_CACHE_SIZE				4096  	      	//dimensione non casuale si vedra dopo perchè
#define BITS_PER_BYTE				8
#define ATOMIC_FS_SUPER_MAGIC		0xF2F52010		//numero magico nel boot master
#define CHECK_SUM_OFFSET			4092


/* Impostazioni per quando si crea una partizione */
#define ATOMIC_FS_NUMBER_OF_CHECKPOINT_PACK			2
#define DEFAULT_SECTOR_SIZE							512
#define DEFAULT_SECTOR_PER_BLOCK					8
#define DEFAULT_BLOCKS_PER_SEGMENT					512
#define DEFAULT_SEGMENTS_PER_SECTION				1

#define VERS_LEN									256

/* Possibili opzioni di configurazione */
enum atomic_fs_config_func
{
	FSCK,DUMP
};

/* Definizione della struttura di una partizione o file nel file system*/
struct atomic_fs_config
{
	u_int32_t 	sectors_size; 		/* dimensione di un settore*/
	u_int32_t 	sectors_reserved;		/* settori riservati per il sistema*/
	double 		over_position;			/* definisce se un nodo è sovraposto a un altro*/
	u_int32_t 	cur_segments[6];		/* definisce i segnemti attualmente utilizzati*/
	u_int32_t	segments_per_sec;
	u_int32_t	sectors_per_zone;
	u_int32_t	segments_per_zone;
	u_int32_t	start_sector;
	u_int64_t	total_sectors;
	u_int32_t	sectors_per_blocks;
	u_int32_t	blocks_per_segment;

	/* Altri bit per le proprietà della partizione*/
	__u8 		init_version[VERS_LEN+1];
	__u8		sb_version[VERS_LEN+1];
	__u8		version[VERS_LEN+1];

	/*Nome del volume*/
	char * volume_label;
	int heap;
	int32_t fd,kd;
	int32_t dump,fd;
	char *device_name;
	char *extension_list;
	int dbg_lv;//debug level
	int trim;
	int func;
	void *private;//puntatore di scorta
	int fix_on;
	int bug_on;
	int auto_fix;
	__le32 feature;

}__attribute__((packed));

#ifdef CONFIG_64BIT
#define BITS_PER_LONG	64
#else
#define BITS_PER_LONG	32


/* Costanti per il BITMASK*/
#define BIT_MASK_(nr)   (1<<(nr%BITS_PER_LONG))
#define BIT_WORD_(nr)	(nr/BITS_PER_LONG)

/*Qui ci sono le stesse cose che troverete in  kernel/fs/f2fs/f2fs.h*/
#define NR_CURSEG_DATA_TYPE		(3)
#define NR_CURSEG_NODE_TYPE		(3)
#define NR_CURSEG_TYPE			(NR_CURSEG_DATA_TYPE+NR_CURSEG_NODE_TYPE)

/* Definizione dei punti di entrata e  di uscita nel mount*/
enum
{
	CURSEG_HOT_DATA=0, 	/*punto di entrata nelle directory*/
	CURSEG_WARM_DATA,	/* data blocks*/
	CURSEG_COLD_DATA,	/*esterni--->multimedia su linux */
	CURSEG_HOT_NODE,	/* nodo diretto ai blocchi  delle directory*/
	CURSEG_WARM_NODE,	/* nodo diretto ai file normali*/
	CURSEG_COLD_NODE,	/* nodo indiretto che quindi mi porta da tutte le parti*/
	NO_CHECK_TYPE
};

/* Sezione copiata da linux/fs/f2fs/segment.h*/
#define GET_SUM_TYPE(footer)  			((footer)->entry_type)
#define SET_SUM_TYPE(footer,type)  		((footer)->entry_type=type)


/* Sezione copiata da include/linux/f2fs_sb.h*/
#define atomic_fs_super_offset			1024
#define atomic_fs_min_log_sector_size	9
#define atomic_fs_max_log_sector_size	12
#define atomic_fs_block_size			4096
#define atomic_fs_max_extension			64
#define atomic_fs_block_align(x)		(((x) + atomic_fs_block_size - 1) / atomic_fs_block_size)
#define NULL_ADDR						0x0U
#define NEW_ADDR						-1U
#define atomic_fs_root_ino(sbi)			(sbi->root_ino_num)
#define atomic_fs_node_ino(sbi)			(sbi->node_ino_num)
#define atomic_fs_meta_ino(sbi)			(sbi->meta_ino_num)

/* Dichiarazione di un nodo direcupero per la situazione ---> puo essere usato anche da node,meta */
#define ALL_ATOMIC_FS_ZERO				(ATOMIC_FS_NO_FS | __ATOMIC_FS_ZERO)


/* Ora ,rimane una questione di fondo molto importante
 *
 * Per un fatto di ottimizzazione dobbiamo creare una serie di log per uno per i nodi e uno per le mete
 * Si pensa che 16 file di log bastano per avere una situazione stabile
 * 8 per nodi
 * 8 per le mete
 * */

#define MAX_ACTIVE_LOG_					16
#define MAX_ACTIVE_NODE_LOG				8
#define MAX_ACTIVE_META_LOG				8


#define ATOMIC_FS_FEATURE_ENCRYPT		0x0001

/* Proprietà per i superblocki presenti nel hd*/
struct atomic_fs_super_blocks
{
	__le32		magic_word;						/* numero magico*/
	__le16 		major_vers;						/* major version*/
	__le16		minor_vers;						/* minor version*/
	__le32		log_sector_size;				/* log2 sector size in bytes*/
	__le32		log_sectors_per_block;			/* log2 # of sectors per block*/
	__le32		log_block_size;					/* log2 block size in bytes*/
	__le32		log_block_per_seg;				/* log2 # of blocks per segment*/
	__le32		segs_per_sect;					/* # of segments per section*/
	__le32		sects_per_zone;					/* # of sectors per zone*/
	__le32		check_sum_offset;				/* checksum offset inside super_block*/
	__le64		block_count;					/* numero totale di blocchi */
	__le32		section_count;					/* numero di sezioni presenti*/
	__le32		segment_count;					/* numero di segmenti presenti*/
	__le32		segment_count_check_pnt;		/* numero totale di check_point presenti nel segmento*/
	__le32		segment_count_SIT;				/* numero di segmenti per il SIT*/
	__le32		segment_count_nat;				/* numero di segmenti per NAT*/
	__le32		segment_count_ssa;				/* numero disegmenti per SSA*/
	__le32		segment_count_main;				/* numero di segmenti del main area*/
	__le32		segment0_block_addr;			/* start block parte da segmento 0*/
	__le32		check_pnt_block_addr;			/* start block per il check_point */
	__le32 		sit_blkaddr;					/* start block address of SIT */
	__le32 		nat_blkaddr;					/* start block address of NAT */
	__le32 		ssa_blkaddr;					/* start block address of SSA */
	__le32 		main_blkaddr;					/* start block address of main area */
	__le32 		root_ino;						/* root inode number */
	__le32 		node_ino;						/* node inode number */
	__le32 		meta_ino;						/* meta inode number */
	__u8 		uuid[16];						/* 128 bit per l'uuid del volume*/
	__le16 		volumen_name[512];				/* nome del volume*/
	__le32		count_extension;				/* contatore della estensioni*/
	__u8		extension_list[atomic_fs_max_extension][8];/* extension array*/
	__le32 		check_pnt_payload;
	__u8 		version[VERS_LEN];				/*versione del kernel*/
	__u8 		init_version[VERS_LEN];			/*versione iniziale el kernel*/
	__le32		feature;						/* feature definite prima*/
	__u8 		encrypt_level;					/* versione del livello di criptazione*/
	__u8		encrypt_pw_salt[16];			/* password usata per l'algortimo di stringtokey*/
	__u8 		reserved[871];					/* regioni di memoria usate dal sistema operativo*/

}__attribute__((packed));


/*
 *
 * Impostazioni dei check_point
 *
 * */

#define CHECK_FASTBOOT_FLAG					0x00000020
#define CHECK_ATOMICK_FS_FLAG				0x00000010
#define CHECK_ERROR_FLAG					0x00000008
#define CHECK_COMPACT_SUM_FLAG				0x00000004
#define CHECK_ORPHAN_FLAG					0x00000002
#define CHECK_UNMOUNT_FLAG					0x00000001



