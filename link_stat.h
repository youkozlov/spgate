#ifndef __LINK_STAT_H__
#define __LINK_STAT_H__

typedef unsigned short 		u16;
typedef unsigned long  		u32;
typedef struct { unsigned long ip; unsigned long addr; }   ipaddr;
typedef unsigned long long    	u64;

typedef struct {
    u32	BTO;
    u32	BFrame;
    u32	BParity;
    u32	BOverrun;
    
    u32	Queries;
    u32	TrmPackets;
    u32	NoResponse;
    u32	Current;
        
    u32	CRC;
    u32	IFunc;
    u32	Size;
    u16	Alarm;
    u16 IDError;
    u16 Status;
} link_stat_t;

struct link_stat_element_t;

struct iface_stat_element_t {
    link_stat_t		link_stat[16];
};

struct link_stat_element_t {
    u64			addr;
    link_stat_t		link_stat;
//    link_stat_element_t	*next;
    int			next;
};

typedef struct {
//    link_stat_element_t 	*cur;
    int				cur;
    int		master_stat[16];
    int		slave_stat[16];
//    link_stat_element_t		*master_stat[16];
//    link_stat_element_t		*slave_stat[16];
} header_link_stat_t;

#endif
