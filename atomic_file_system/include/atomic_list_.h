/*
Funzione di controllo per la gestione della lista delle directory
*/

#define POISON_POINTER_DELTA        0
#define LIST_POISON_1               ((void*)(0x00100100 + POISON_POINTER_DELTA))
#define LIST_POISON_2               ((void*)(0x00200200 + POISON_POINTER_DELTA))

/* Non sicuro che offsetof sia dichiarato*/
#if !defined(offsetof)
/* Macro per la funzione offsetof -----> Parametri(type-->tipo,member-->membro)
La macro offsetof attraverso la costante seze_t fa AND bit a bit del membro

*/
#define offsetof(TYPE,MEMBER)       ((size_t)&((TYPE*)0)->MEMBER)
#endif

#define container_of(ptr,type,member) ({                            \
                    const typeof(((type*)0)->member)*__mptr=(ptr);  \
                    (type*)((char*)__mptr-offsetof(type,member));})
                    
                    
/* Dichiarazione di una struttura base per lo spostamento*/
struct list_head
{
    struct list_head *next,*prev;
};

#define ATOMIC_FS_LIST_HEAD_INIT    (name) {&(name),&(name)}

/* Definizione dell'inizio del'header della lista
La macro assegna alla struttura list_head la macro che passa l'indirizzo di memoria 
*/

#define LIST_HEAD                   (name)                          \
                                    struct list_head=ATOMIC_FS_LIST_HEAD_INIT(name)
/* Inizio della lista per le directory*/
static inline void ATOMIC__FS_INIT_LIST_HEAD(struct list_head *list)
{
            list->next=list;
            list->prev=list;
}

static inline void __list_add(struct list_head *new,struct list_head*prev,struct list_head *next)
{
            next->prev=new;         /* Il nuovo nodo diventa il succesivo*/
            new->next=next;         /* Al nuovo nodo assegna nex-->che era stato caricato con new*/
            new->prev=prev;         /* Il puntatore prima diventa quello prima di new*/
            prev->next=new;         /* */
}

static inline list_add(struct list_head *new,struct list_head *head)
{
      __list_add(new,head,head->next); 
}
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	    __list_add(new, head->prev, head);
}
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}
