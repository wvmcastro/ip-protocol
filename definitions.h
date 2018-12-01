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
#define SHOW_TABLE    0
#define RES_IP        1
#define ADD_ARP_LINE  2
#define DEL_ARP_LINE  3
#define SET_ARP_TTL   4
#define LIST_IFCES    5
#define CONFIG_IFACE  6
#define SET_IFACE_MTU 7

// new features implemented in ipd
#define LIST_IFACE    8
// -------- End inter program communication ----------

#endif
