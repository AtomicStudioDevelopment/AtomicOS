* atomic_fs_f2fs.h
 *
 *  Created on: 23 ott 2015
 *      Author: Atomic Studio Development
 */

#ifndef _F2FS_H_
#define _F2FS_H_


/* Inclusione delle librerie standard*/

#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<mntent.h>
#include<linux/types.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<sys/mount.h>
#include<assert.h>

#include <atomic_fs.h>

/* Codice errore*/

#define EXIT_ERR_CODE		(-1)

#define ver_after(a,b)		(typecheck(unsigned long long ,a)&&				\
							typecheck(unsigned long long ,b) &&				\
							((long long)((a)-(b))>0))

struct list_head
{
	struct list_head *next,*prev;
};

enum
{
	NAT_BITMAP,
	SIT_BITMAP
};


struct node_info
{
	nid_t nid;
	nid_t ino;
	u32 blk_address;
	unsigned char version;
};

struct atomic_f2fs_nm_info
{
	block_t		nat_blkaddr;
	nid_t		max_ind;
	nid_t		init_scan_nid;
	nid_t		next_scan_nid;

	unsigned int nat_cnt;
	unsigned int fcnt;

	char *nat_bitmap;
	int bitmap_size;
};


struct seg_entry
{
	unsigned short  valid_blocks;/* #contatore dei blocchi validi*/
	unsigned short 	*cur_valid_map;/* validity bitmap of blocks*/
	/*
	 * Salvo il contatore dei blocchi e la validita del bitmap nell'ultimo check_point
	 * Questo informazioni le userò in futuro
	 *
	 * */

	unsigned short checkpoints_valid_blocks;
	unsigned short *checkpoints_valid_map;
	unsigned char type;							/* tipo segmento--->esempio ->CURSEG_XXX_TYPE*/
	unsigned char orig_type;					/* tipo segmento--->esempio ->CURSEG_XXX_TYPE*/
	unsigned long long time_modif;				/* ultima data della modifica del segmento*/
};

struct sec_entry
{
	unsigned int valid_blocks;					/* # contatore dei blocchi validi in una sezione*/
};

/* Questa è la struct che contiene le informazioni sui contatori di validita inserite nella bitmap
 * di tutta l'aria del MAIN.
 * SIT------>SEGMENT INFORMATION TABLE
 * */
struct sit_info
{
	block_t 		sit_base_addr;				/* il blocco di partenza dell'area di SIT*/
	block_t			sit_block;					/* contatore blocchi usati dall'aria di SIT*/
	block_t			written_valid_blocks;		/* contatore dei blocchi nell'aria di main*/
	char 			*sit_bitmap;				/* SIT BITMAP puntatore */
	unsigned int 	bitmap_size;				/* SIT BITMAP size*/


	/* Qui prevedo un entrata non pulita in una sezione del blocco*/
	unsigned long 		*dirty_sentries_bitmap;		/* bitmap per le entrate non corette o con valori sbagliati*/
	unsigned int 		dirty_sentries;				/* contatore delle entrate sbagliate*/
	unsigned int 		sent_per_block;				/* contatore dei SIT per ogni blocco*/
	struct seg_entry	*sentries;					/* le cache per le SIT dei segmenti*/
	struct sec_entry	*sec_entries;				/* le cache per le SIT delle sezioni*/
};

struct curseg_info
{
	struct atomic_fs_summary *sum_blk;						/* */
	unsigned char 		alloc_type;							/* tipo attuale di allocazione*/
	unsigned int 		segno;								/* il numero del segmento corrente*/
	unsigned short 		next_blkoff;						/* il prossimo blocco spostato di  --->offset dove posso scrivere*/
	unsigned int 		zone;								/* il numero della zona corrente*/
	unsigned int 		next_segno;							/* il segmento preallocato*/
};

struct atomic_f2_fs_sm_info
{
	struct sit_info 	*sit_info;
	struct curseg_info	*curseg_array;

	block_t  			seg0_blk_addr;
	block_t				main_blk_addr;
	block_t				ssa_blk_addr;

	/* SSA ----------> Segment summary area
	 *
	 * Questa contiene le informazioni di tutti i dati i nodi  dei blocchi che stanno nella MAIN aria
	 * */

	unsigned int 		segment_count;
	unsigned int 		main_segments;
	unsigned int 		reserved_segments;
	unsigned int 		over_pos_segments;
};

struct super_blocks_info/*sb_info ----->questo era il nome precendente*/
{
	/* Questa sezione è abbastanza complessa in quanto offre tutte le caratteristiche del
	 * algoritmo di scrittura lettura e indexing del file system
	 * */
	struct f2fs_fsck  			*fsck;

	struct f2fs_super_block 	*raw_super;

	struct f2fs_nm_info			*nm_info;

	struct f2fs_sm_info			*sm_info;

	struct f2fs_check_point		*ckpt;


	struct list_head  orphane_inode_list;
	unsigned int number_orphans;


	/* Anche se dichiarati precedentemente ridichiaro qui i tupi base del file system*/

	unsigned int 			log_sectors_per_block;		/* log2 dei settori per blocco*/
	unsigned int 			log_block_size;				/* log2 della dimensione del blocco*/
	unsigned int 			block_size;					/* dimensione del blocco*/

	/* il nodo root*/
	unsigned int 			root_inode_num;				/* numero del nodo root*/
	unsigned int 			node_inode_num;				/* numero del nodo di collegamento*/
	unsigned int 			meta_ino_num;				/* numero del nodo meta*/

	/* Log2 per i blocchi per segmento*/
	unsigned int 			log_blocks_per_seg;			/* log2 dei blocchi per segmento*/
	unsigned int 			blocks_per_seg;				/* blocchi per segmento*/
	unsigned int 			segs_per_section;			/* segmenti per sezione*/
	unsigned int 			secs_per_zone;				/* sezioni per zona*/

	unsigned int 			total_sections;				/* # del totale della sezioni*/
	unsigned int 			total_node_count;			/* # dei nodi --->contatore dei nodi totali*/
	unsigned int 			total_valid_node_count;		/* numero di node validi contabili*/
	unsigned int 			total_valid_inode_count;	/* numero di inode validi contabili*/

	int 					active_logs;				/* numero dei log attivi*/

	block_t					_user_block_count;			/* contatore dei blocchi del utente*/
	block_t					_total_valid_block_count;	/* contatore dei blocchi validi */
	block_t					alloc_valid_block_count;	/* contatore dei blocchi allocati precedentemente*/
	block_t					last_valid_block_count;		/* contatore dell'ultimo blocco coretto-----> da usare per recovery*/
	u32						s_next_generation;			/* per il supporto NFS*/

	unsigned int			curr_victim_sec;			/* la vittima nel casso di pulizia ---->quella a basso livello con thread kernel*/

};

static inline struct f2fs_super_block *F2Fs_RAW_SUPER(struct super_blocks_info *sbi)
{
	return (struct f2fs_super_block*)(sbi->raw_super);/*in caso di recupero*/
}

static inline struct f2fs_checkpoint *F2FS_CKPT(struct super_blocks_info *sbi)
{
	return (struct f2fs_checkpoint*)(sbi->ckpt);
}

static inline struct f2fs_fsck *FSCK(struct super_blocks_info *sbi)
{
	return (struct f2fs_fsck*)(sbi->fsck);
}

static inline struct f2fs_nm_info *NM_INFO(struct super_blocks_info *sbi)
{
	return (struct f2fs_nm_info*)(sbi->nm_info);
}
static inline struct f2fs_sm_info *SM_INFO(struct super_blocks_info *sbi)
{
	return (struct f2fs_sm_info*)(sbi->sm_info);
}
static inline struct f2fs_sit_info *SIT_INFO(struct super_blocks_info *sbi)
{
	return (struct f2fs_sit_info*)(SM_INFO(sbi)->f2fs_sm_info);
}
static inline void *inline_data_addr(struct atomic_fs_node *node_block )
{
	return (void*)&(node_block->i.i_addr[1]);
}
static inline unsigned int ofs_of_node(struct atomic_fs_node *node_block)
{
	unsigned flag=le_32_to_cpu(node_block->footer.flag);
	return flag>>OFFSET_BIT_SHIFT;
}
static inline unsigned long __bitmap_size(struct super_blocks_info *sbi,int flags)
{
	struct f2fs_checkpoint *ckpt=F2FS_CKPT(sbi);
}

