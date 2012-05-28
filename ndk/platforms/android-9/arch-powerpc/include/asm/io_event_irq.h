/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _ASM_POWERPC_IO_EVENT_IRQ_H
#define _ASM_POWERPC_IO_EVENT_IRQ_H

#include <linux/types.h>
#include <linux/notifier.h>

#define PSERIES_IOEI_RPC_MAX_LEN 216

#define PSERIES_IOEI_TYPE_ERR_DETECTED 0x01
#define PSERIES_IOEI_TYPE_ERR_RECOVERED 0x02
#define PSERIES_IOEI_TYPE_EVENT 0x03
#define PSERIES_IOEI_TYPE_RPC_PASS_THRU 0x04

#define PSERIES_IOEI_SUBTYPE_NOT_APP 0x00
#define PSERIES_IOEI_SUBTYPE_REBALANCE_REQ 0x01
#define PSERIES_IOEI_SUBTYPE_NODE_ONLINE 0x03
#define PSERIES_IOEI_SUBTYPE_NODE_OFFLINE 0x04
#define PSERIES_IOEI_SUBTYPE_DUMP_SIZE_CHANGE 0x05
#define PSERIES_IOEI_SUBTYPE_TORRENT_IRV_UPDATE 0x06
#define PSERIES_IOEI_SUBTYPE_TORRENT_HFI_CFGED 0x07

#define PSERIES_IOEI_SCOPE_NOT_APP 0x00
#define PSERIES_IOEI_SCOPE_RIO_HUB 0x36
#define PSERIES_IOEI_SCOPE_RIO_BRIDGE 0x37
#define PSERIES_IOEI_SCOPE_PHB 0x38
#define PSERIES_IOEI_SCOPE_EADS_GLOBAL 0x39
#define PSERIES_IOEI_SCOPE_EADS_SLOT 0x3A
#define PSERIES_IOEI_SCOPE_TORRENT_HUB 0x3B
#define PSERIES_IOEI_SCOPE_SERVICE_PROC 0x51

struct pseries_io_event {
 uint8_t event_type;
 uint8_t rpc_data_len;
 uint8_t scope;
 uint8_t event_subtype;
 uint32_t drc_index;
 uint8_t rpc_data[PSERIES_IOEI_RPC_MAX_LEN];

};

#endif
