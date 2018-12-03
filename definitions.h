#ifndef __DEFINES__
#define __DEFINES__


// The purpose of this file is to defines the macros
// Used in all system's parts


// ----------------- General macros -----------------
#define __ERROR__         -1
#define __OK__            0
#define _DEFAULT_ARP_TTL_ 60
#define LOOPBACK_IP       "127.0.0.1"
#define XARPD_PORT        5050
#define LISTEN_ENQ        5 // listen queue size
#define MAX_IFNAME_LEN    22
#define IFACE_UP          1
#define IFACE_DOWN        0
// -------------- End general macros -----------------


// ----------- Types of arp table entries ------------
#define DYNAMIC_ENTRY 0
#define STATIC_ENTRY  1
// --------- End types of arp table entries ----------


// ------------------ ARP protocol -------------------
#define ARP_ETHERTYPE     0x0806
#define ARP_HW_TYPE       0x0001
#define ARP_PROTOTYPE     0x0800 // IP
#define HW_ADDR_LEN       0x06 // 6 bytes
#define PROTOCOL_ADDR_LEN 0x04 // 4 bytes
#define ARP_REQUEST       0x01
#define ARP_REPLY         0x02
#define ARP_TIMEOUT       0x05 // 5 seconds
// ---------------- End ARP protocol ------------------

// ---------- Inter program communication ------------
// OPCODES
// 0 - 49 xifconfig
#define LIST_IFCES        0
#define CONFIG_IFACE      1
#define SET_IFACE_MTU     2
#define LIST_IFACE        3
#define TURN_IFACE_ON_OFF 4

// 50 - 99 xarp
#define SHOW_ARP_TABLE  50
#define RES_IP          51
#define ADD_ARP_LINE    52
#define DEL_ARP_LINE    53
#define SET_ARP_TTL     54

// 100 - 149 xroute
#define SHOW_ROUTE_TABLE  100
#define ADD_ROUTE_LINE    101
#define DEL_ROUTE_LINE    102
// -------- End inter program communication ----------

#endif
