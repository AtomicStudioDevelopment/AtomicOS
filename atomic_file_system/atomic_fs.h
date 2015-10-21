/*
 * atomic_fs.h
 *
 *  Created on: 21 ott 2015
 *      Author: Atomic Studio Development
 *      Crediti: Samsung Electronics per il filesystem F2FS che stiamo modificando
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
#define MAX_ACTIVE_DATA_LOG				8


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

struct atomic_checkpoint
{
	__le64 			check_vers_;			/* versione del checpoint*/
	__le64			user_block_count;		/* contatore dei blocchi dell'utente*/
	__le64			valid_block_count;		/* contatore blocchi validi nel main aria*/
	__le64			resv_segment_count;		/* contatore del contatore delle aree riservate*/
	__le64			overpos_segment_count;	/* contatore delle sovvraposizioni*/
	__le64			free_segment_count;		/* contatore dei segmenti free nell'aria*/


	/* Informazini sul nodo corrente */
	__le32 			cur_node_segno[MAX_ACTIVE_NODE_LOG];
	__le16 			cur_node_block_off[MAX_ACTIVE_NODE_LOG];

	/* Informazioni sui dati del segmento*/
	__le32 			cur_data_segno[MAX_ACTIVE_DATA_LOG];
	__le16 			cur_data_blkoff[MAX_ACTIVE_DATA_LOG];

	/* Flag di conferma delle varie opzioni*/
	__le32 			check_flg;						/* Flag:si riferisci al fatto se montato e se il journal e presente*/
	__le32			check_pack_total_block_count;	/*  totale di check_point */
	__le32			check_pack_start_sum;			/*  inizio del numero di checkpoint*/
	__le32			valid_node_count;				/* numero totale di nodi validi*/
	__le32			valid_inode_count;				/* numero totale di inode validi */
	__le32			next_free_nid;					/* numero successivo di nodo*/
	__le32			sit_ver_bitmap_bytesize;		/* valore default=64*/
	__le32			nat_ver_bitmap_bytesize;		/* valore default=256*/
	__le32			check_sum_offeset;				/* checksum dentro il blocco check_point*/
	__le32			elapse_time;					/* tempo trascorso*/

	/* tipo di allocazione del segmento*/
	unsigned char alloc_type[MAX_ACTIVE_LOG_];

	/* versione del SIT e NAT bitmap*/
	unsigned char sit_nat_version_bitmap[1];
}__attribute__((packed));



/*
 *
 *
 * Questa parte si occupa di gestire la parte degli inode che non hanno riferimenti
 *
 */

#define ATOMIC_FS_ORPHANS_PER_BLOCK				1020


struct atomic_fs_orphan_block
{
	__le32 				ino[ATOMIC_FS_ORPHANS_PER_BLOCK];
	__le16 				reserved;
	__le16				block_address;
	__le16				block_count;
	__le32				entry_count;
	__le32				check_sum;
}__attribute__((packed));

/*
 * Struttura del nodo
 */
struct atomic_fs_extent {
	__le32 fofs;			/* il file offset */
	__le32 blk_addr;		/* block init */
	__le32 len;				/* lunghezza */
} __attribute__((packed));



/* Impostazioni definite dei segmenti e delle aree di memoria */

#define ATOMIC_FS_NAME_LEN				255
#define ATOMIC_FS_INLINE_EXTRAS			50						/* 200 byte che uso extra per eventuali */
#define DEF_ADDR_PER_INODE				923						/* puntatore a un inode*/
#define ADDR_PER_INODE(fi)				addr_per_inode(fi)
#define ADDR_PER_BLOCK					1018					/* puntatori diretti a blocchi*/
#define NIDS_PER_BLOCKS					1018 					/* ID dei nodi in blocchi indiretti*/

/*
 * Questi define sotto definiscono come io posso accedere agli indici in maniera facile
 * */

#define NODE_DIR1_BLOCK					(DEF_ADDR_PER_INODE+1)
#define NODE_DIR2_BLOCK					(DEF_ADDR_PER_INODE+2)
#define NODE_DIR1_BLOCK					(DEF_ADDR_PER_INODE+3)
#define NODE_DIR2_BLOCK					(DEF_ADDR_PER_INODE+4)
#define NODE_DIND_BLOCK					(DEF_ADDR_PER_INODE+5)

#define ATOMIC_FS_INLINE_EXTRA_ADD		0x01		/* file inline per xtra flag*/
#define ATOMIC_FS_INLINE_DATA			0x02		/* file inline per data flag*/
#define ATOMIC_FS_INLINE_DENTRY			0x04		/* file inline per dentry flag */
#define ATOMIC_FS_DATA_EXIST			0x08		/* file inline per data exist*/
#define ATOMIC_FS_INLINE_DOTS			0x10		/* per i punti di entrata*/

#define MAX_INLINE_DATA		(sizeof(__le32) * (DEF_ADDR_PER_INODE - \
						ATOMIC_FS_INLINE_EXTRAS - 1))

#define INLINE_DATA_OFFSET	(PAGE_CACHE_SIZE - sizeof(struct node_footer) \
				- sizeof(__le32)*(DEF_ADDR_PER_INODE + 5 - 1))

#define DEF_DIR_LEVEL					0

#define FADVISE_COLD_BIT       			0x01
#define FADVISE_LOST_PINO_BIT  			0x02
#define FADVISE_ENCRYPT_BIT    			0x04

#define file_is_encrypt(i_advise)      	((i_advise) & FADVISE_ENCRYPT_BIT)


struct atomic_fs_inode {
	__le16 i_mode;			/*tipo file*/
	__u8 i_advise;			/* file hints */
	__u8 i_inline;			/* inline flag */
	__le32 i_uid;			/* nome_utente */
	__le32 i_gid;			/* ID gruppo(se presente) */
	__le32 i_links;			/* contatore cpllegamenti */
	__le64 i_size;			/* dimensione file in byte */
	__le64 i_blocks;		/* dimensione del file in blocchi */
	__le64 i_atime;			/* creato */
	__le64 i_ctime;			/* cambia data */
	__le64 i_mtime;			/* ultima modifica*/
	__le32 i_atime_nsec;		/* accesso  */
	__le32 i_ctime_nsec;		/* cambia data*/
	__le32 i_mtime_nsec;		/* ultima modifica*/
	__le32 i_generation;		/* vesione del file (for NFS) */
	__le32 i_current_depth;		/* directory*/
	__le32 i_xattr_nid;		/* ID per salvare eventuali altri flag */
	__le32 i_flags;			/* attributi del file */
	__le32 i_pino;			/* numero dell'inode a cui appartiene */
	__le32 i_namelen;		/* lunchezza nome file */
	__u8 i_name[ATOMIC_FS_NAME_LEN];	/* file name for SPOR */
	__u8 i_dir_level;		/* valore più grande per le directory più grosse*/

	struct atomic_fs_extent i_ext;	/* serve a memorizzare qualcosa di più grosso */

	__le32 i_addr[DEF_ADDR_PER_INODE];	/* Pointers to data blocks */

	__le32 i_nid[5];		/* direct(2), indirect(2),
						double_indirect(1) node id */
} __attribute__((packed));

struct direct_node {
	__le32 addr[ADDR_PER_BLOCK];	/* array di blocchi di dati */
} __attribute__((packed));

struct indirect_node {
	__le32 nid[NIDS_PER_BLOCKS];	/* array di blocchi di dati */
} __attribute__((packed));


enum {
	COLD_BIT_SHIFT = 0,
	FSYNC_BIT_SHIFT,
	DENT_BIT_SHIFT,
	OFFSET_BIT_SHIFT
};

#define XATTR_NODE_OFFSET	((((unsigned int)-1) << OFFSET_BIT_SHIFT) \
				>> OFFSET_BIT_SHIFT)

struct node_footer {
	__le32 nid;		/* id del nodo */
	__le32 ino;		/* numero inodo */
	__le32 flag;		/* include cold/fsync/dentry marks and offset */
	__le64 cp_ver;		/* versione del checkpoint */
	__le32 next_blkaddr;	/* il prossimo nodo alla struttura di blocchi */
} __attribute__((packed));

struct atomic_fs_node {
	/* Puo essere di 3 tipi:diretto indiretto inode--->collegamento*/
	union {
		struct atomic_fs_inode i;
		struct direct_node dn;
		struct indirect_node in;
	};
	struct node_footer footer;
} __attribute__((packed));

/*
 * Per le entrate NAT
 */
#define NAT_ENTRY_PER_BLOCK (PAGE_CACHE_SIZE / sizeof(struct atomic_fs_nat_entry))

struct atomic_fs_nat_entry {
	__u8 version;		/* l'ultimo indirizzo che è entrato  */
	__le32 ino;		/* numero dell'inode */
	__le32 block_addr;	/* indirizzo del blocco */
} __attribute__((packed));

struct atomic_fs_nat_block {
	struct atomic_fs_nat_entry entries[NAT_ENTRY_PER_BLOCK];
} __attribute__((packed));


/*
 * Per le entrate SIT
 *
 * Ogni elemento ha una dimensione di 2mb quindi sono accettati solo nodi che occupano 64 bytes,quindi 512 bit
 * Se cambio diventa instabile quindi per ora lasciare COSI
 */
#define SIT_VBLOCK_MAP_SIZE 64
#define SIT_ENTRY_PER_BLOCK (PAGE_CACHE_SIZE / sizeof(struct f2fs_sit_entry))

/*
 * F2FS uses 4 bytes to represent block address. As a result, supported size of
 * disk is 16 TB and it equals to 16 * 1024 * 1024 / 2 segments.
 */
#define ATOMIC_FS_MAX_SEGMENT     			((16 * 1024 * 1024) / 2)
#define ATOMIC_FS_MAX_SIT_BITMAP_SIZE    	((ATOMIC_FS_MAX_SEGMENT  / SIT_ENTRY_PER_BLOCK) / 8)

#define SIT_VBLOCKS_SHIFT	10
#define SIT_VBLOCKS_MASK	((1 << SIT_VBLOCKS_SHIFT) - 1)
#define GET_SIT_VBLOCKS(raw_sit)				\
	(le16_to_cpu((raw_sit)->vblocks) & SIT_VBLOCKS_MASK)
#define GET_SIT_TYPE(raw_sit)					\
	((le16_to_cpu((raw_sit)->vblocks) & ~SIT_VBLOCKS_MASK)	\
	 >> SIT_VBLOCKS_SHIFT)

struct atomic_fs_sit_entry {
	__le16 vblocks;				/* reference above */
	__u8 valid_map[SIT_VBLOCK_MAP_SIZE];	/* bitmap for valid blocks */
	__le64 mtime;				/* segment age for cleaning */
} __attribute__((packed));

struct atomic_fs_sit_block {
	struct atomic_fs_sit_entry entries[SIT_ENTRY_PER_BLOCK];
} __attribute__((packed));

/*
 * For segment summary
 *
 * One summary block contains exactly 512 summary entries, which represents
 * exactly 2MB segment by default. Not allow to change the basic units.
 *
 * NOTE: For initializing fields, you must use set_summary
 *
 * - If data page, nid represents dnode's nid
 * - If node page, nid represents the node page's nid.
 *
 * The ofs_in_node is used by only data page. It represents offset
 * from node's page's beginning to get a data block address.
 * ex) data_blkaddr = (block_t)(nodepage_start_address + ofs_in_node)
 */
#define ENTRIES_IN_SUM		512
#define	SUMMARY_SIZE		(7)	/* sizeof(struct summary) */
#define	SUM_FOOTER_SIZE		(5)	/* sizeof(struct summary_footer) */
#define SUM_ENTRIES_SIZE	(SUMMARY_SIZE * ENTRIES_IN_SUM)

/* a summary entry for a 4KB-sized block in a segment */
struct atomic_fs_summary {
	__le32 nid;		/* parent node id */
	union {
		__u8 reserved[3];
		struct {
			__u8 version;		/* node version number */
			__le16 ofs_in_node;	/* block index in parent node */
		} __attribute__((packed));
	};
} __attribute__((packed));

/* summary block type, node or data, is stored to the summary_footer */
#define SUM_TYPE_NODE		(1)
#define SUM_TYPE_DATA		(0)

struct summary_footer {
	unsigned char entry_type;	/* SUM_TYPE_XXX */
	__u32 check_sum;		/* summary checksum */
} __attribute__((packed));

#define SUM_JOURNAL_SIZE	(atomic_fs_block_size - SUM_FOOTER_SIZE -\
				SUM_ENTRIES_SIZE)
#define NAT_JOURNAL_ENTRIES	((SUM_JOURNAL_SIZE - 2) /\
				sizeof(struct nat_journal_entry))
#define NAT_JOURNAL_RESERVED	((SUM_JOURNAL_SIZE - 2) %\
				sizeof(struct nat_journal_entry))
#define SIT_JOURNAL_ENTRIES	((SUM_JOURNAL_SIZE - 2) /\
				sizeof(struct sit_journal_entry))
#define SIT_JOURNAL_RESERVED	((SUM_JOURNAL_SIZE - 2) %\
				sizeof(struct sit_journal_entry))
/*
 * frequently updated NAT/SIT entries can be stored in the spare area in
 * summary blocks
 */
enum {
	NAT_JOURNAL = 0,
	SIT_JOURNAL
};

struct nat_journal_entry {
	__le32 nid;
	struct atomic_fs_nat_entry ne;
} __attribute__((packed));

struct nat_journal {
	struct nat_journal_entry entries[NAT_JOURNAL_ENTRIES];
	__u8 reserved[NAT_JOURNAL_RESERVED];
} __attribute__((packed));

struct sit_journal_entry {
	__le32 segno;
	struct atomic_fs_sit_entry se;
} __attribute__((packed));

struct sit_journal {
	struct sit_journal_entry entries[SIT_JOURNAL_ENTRIES];
	__u8 reserved[SIT_JOURNAL_RESERVED];
} __attribute__((packed));

/* 4KB-sized summary block structure */
struct atomic_fs_summary_block {
	struct atomic_fs_summary entries[ENTRIES_IN_SUM];
	union {
		__le16 n_nats;
		__le16 n_sits;
	};
	/* spare area is used by NAT or SIT journals */
	union {
		struct nat_journal nat_j;
		struct sit_journal sit_j;
	};
	struct summary_footer footer;
} __attribute__((packed));

/*
 * Operazioni sulle directory
 */
#define ATOMIC_FS_DOT_HASH				0
#define ATOMIC_FS_DDOT_HASH				ATOMIC_FS_DOT_HASH
#define ATOMIC_FS_MAX_HASH				(~((0x3ULL) << 62))
#define ATOMIC_FS_HASH_COL_BIT			((0x1ULL) << 63)

typedef __le32	f2fs_hash_t;

/* Una direcotry occupa 8 bit */
#define ATOMIC_FS_SLOT_LEN				8
#define ATOMIC_FS_SLOT_LEN_BITS			3

#define GET_DENTRY_SLOTS(x)	((x + ATOMIC_FS_SLOT_LEN - 1) >> ATOMIC_FS_SLOT_LEN_BITS)

/* numero delle entry in un bloco */
#define NR_DENTRY_IN_BLOCK	214

/* Numero massimo di direcotry che una directory puù vedere dall'interno */
#define MAX_DIR_HASH_DEPTH	63

#define SIZE_OF_DIR_ENTRY	11	/* by byte */
#define SIZE_OF_DENTRY_BITMAP	((NR_DENTRY_IN_BLOCK + BITS_PER_BYTE - 1) / \
					BITS_PER_BYTE)
#define SIZE_OF_RESERVED	(PAGE_SIZE - ((SIZE_OF_DIR_ENTRY + \
				ATOMIC_FS_SLOT_LEN) * \
				NR_DENTRY_IN_BLOCK + SIZE_OF_DENTRY_BITMAP))

/*  */
struct atomic_fs_dir_entry {
	__le32 hash_code;	/* hash code */
	__le32 ino;		/* numero inode */
	__le16 name_len;	/* lunghezza del file name */
	__u8 file_type;		/* tipo di file */
} __attribute__((packed));

/*  */
struct atomic_fs_dentry_block {
	/*  */
	__u8 dentry_bitmap[SIZE_OF_DENTRY_BITMAP];
	__u8 reserved[SIZE_OF_RESERVED];
	struct atomic_fs_dir_entry dentry[NR_DENTRY_IN_BLOCK];
	__u8 filename[NR_DENTRY_IN_BLOCK][ATOMIC_FS_SLOT_LEN];
} __attribute__((packed));

/* per le directory inline */
#define NR_INLINE_DENTRY	(MAX_INLINE_DATA * BITS_PER_BYTE / \
				((SIZE_OF_DIR_ENTRY + ATOMIC_FS_SLOT_LEN) * \
				BITS_PER_BYTE + 1))
#define INLINE_DENTRY_BITMAP_SIZE	((NR_INLINE_DENTRY + \
					BITS_PER_BYTE - 1) / BITS_PER_BYTE)
#define INLINE_RESERVED_SIZE	(MAX_INLINE_DATA - \
				((SIZE_OF_DIR_ENTRY + ATOMIC_FS_SLOT_LEN) * \
				NR_INLINE_DENTRY + INLINE_DENTRY_BITMAP_SIZE))

/*  */
struct atomic_fs_inline_dentry {
	__u8 dentry_bitmap[INLINE_DENTRY_BITMAP_SIZE];
	__u8 reserved[INLINE_RESERVED_SIZE];
	struct atomic_fs_dir_entry dentry[NR_INLINE_DENTRY];
	__u8 filename[NR_INLINE_DENTRY][ATOMIC_FS_SLOT_LEN];
} __packed;

/* FLAGS*/
enum FILE_TYPE {
	ATOMIC_FS_FT_UNKNOWN,
	ATOMIC_FS_FT_REG_FILE,
	ATOMIC_FS_FT_DIR,
	ATOMIC_FS_FT_CHRDEV,
	ATOMIC_FS_FT_BLKDEV,
	ATOMIC_FS_FT_FIFO,
	ATOMIC_FS_FT_SOCK,
	ATOMIC_FS_FT_SYMLINK,
	ATOMIC_FS_FT_MAX,
	/* aggunti per fsck */
	ATOMIC_FS_FT_ORPHAN,
	ATOMIC_FS_FT_XATTR,
	ATOMIC_FS_FT_LAST_FILE_TYPE = ATOMIC_FS_FT_XATTR,
};

/* copiato daf2fs/segment.h */
enum {
	LFS = 0,
	SSR
};

extern void ASCIIToUNICODE(u_int16_t *, u_int8_t *);
extern int log_base_2(u_int32_t);
extern unsigned int addrs_per_inode(struct f2fs_inode *);

extern int get_bits_in_byte(unsigned char n);
extern int set_bit(unsigned int nr,void * addr);
extern int clear_bit(unsigned int nr, void * addr);
extern int test_bit(unsigned int nr, const void * addr);
extern int atomic_fs_test_bit(unsigned int, const char *);
extern int atomic_fs_set_bit(unsigned int, char *);
extern int atomic_fs_clear_bit(unsigned int, char *);
extern unsigned long find_next_bit(const unsigned long *,
				unsigned long, unsigned long);

extern u_int32_t atomic_fs_cal_crc32(u_int32_t, void *, int);
extern int atomic_fs_crc_valid(u_int32_t blk_crc, void *buf, int len);

extern void atomic_fs_init_configuration(struct atomic_fs_config *);
extern int atomic_fs_dev_is_umounted(struct atomic_fs_config *);
extern int atomic_fs_get_device_info(struct atomic_fs_config *);
extern void atomic_fs_finalize_device(struct atomic_fs_config *);

extern int dev_read(void *, __u64, size_t);
extern int dev_write(void *, __u64, size_t);
extern int dev_write_block(void *, __u64);
extern int dev_write_dump(void *, __u64, size_t);
/* All bytes in the buffer must be 0 use dev_fill(). */
extern int dev_fill(void *, __u64, size_t);

extern int dev_read_block(void *, __u64);
extern int dev_read_blocks(void *, __u64, __u32 );
extern int dev_reada_block(__u64);

extern int dev_read_version(void *, __u64, size_t);
extern void get_kernel_version(__u8 *);
f2fs_hash_t atomic_fs_dentry_hash(const unsigned char *, int);

extern struct atomic_fs_config config;

#define ALIGN(val, size)	((val) + (size) - 1) / (size)
#define SEG_ALIGN(blks)		ALIGN(blks, config.blks_per_seg)
#define ZONE_ALIGN(blks)	ALIGN(blks, config.blks_per_seg * \
					config.segs_per_zone)

#endif
