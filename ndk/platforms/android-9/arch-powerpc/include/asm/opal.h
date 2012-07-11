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
#ifndef __OPAL_H
#define __OPAL_H

#ifndef __ASSEMBLY__

struct opal_takeover_args {
 u64 k_image;
 u64 k_size;
 u64 k_entry;
 u64 k_entry2;
 u64 hal_addr;
 u64 rd_image;
 u64 rd_size;
 u64 rd_loc;
};

struct rtas_args;

#endif

#define OPAL_SUCCESS 0
#define OPAL_PARAMETER -1
#define OPAL_BUSY -2
#define OPAL_PARTIAL -3
#define OPAL_CONSTRAINED -4
#define OPAL_CLOSED -5
#define OPAL_HARDWARE -6
#define OPAL_UNSUPPORTED -7
#define OPAL_PERMISSION -8
#define OPAL_NO_MEM -9
#define OPAL_RESOURCE -10
#define OPAL_INTERNAL_ERROR -11
#define OPAL_BUSY_EVENT -12
#define OPAL_HARDWARE_FROZEN -13

#define OPAL_CONSOLE_WRITE 1
#define OPAL_CONSOLE_READ 2
#define OPAL_RTC_READ 3
#define OPAL_RTC_WRITE 4
#define OPAL_CEC_POWER_DOWN 5
#define OPAL_CEC_REBOOT 6
#define OPAL_READ_NVRAM 7
#define OPAL_WRITE_NVRAM 8
#define OPAL_HANDLE_INTERRUPT 9
#define OPAL_POLL_EVENTS 10
#define OPAL_PCI_SET_HUB_TCE_MEMORY 11
#define OPAL_PCI_SET_PHB_TCE_MEMORY 12
#define OPAL_PCI_CONFIG_READ_BYTE 13
#define OPAL_PCI_CONFIG_READ_HALF_WORD 14
#define OPAL_PCI_CONFIG_READ_WORD 15
#define OPAL_PCI_CONFIG_WRITE_BYTE 16
#define OPAL_PCI_CONFIG_WRITE_HALF_WORD 17
#define OPAL_PCI_CONFIG_WRITE_WORD 18
#define OPAL_SET_XIVE 19
#define OPAL_GET_XIVE 20
#define OPAL_GET_COMPLETION_TOKEN_STATUS 21  
#define OPAL_REGISTER_OPAL_EXCEPTION_HANDLER 22
#define OPAL_PCI_EEH_FREEZE_STATUS 23
#define OPAL_PCI_SHPC 24
#define OPAL_CONSOLE_WRITE_BUFFER_SPACE 25
#define OPAL_PCI_EEH_FREEZE_CLEAR 26
#define OPAL_PCI_PHB_MMIO_ENABLE 27
#define OPAL_PCI_SET_PHB_MEM_WINDOW 28
#define OPAL_PCI_MAP_PE_MMIO_WINDOW 29
#define OPAL_PCI_SET_PHB_TABLE_MEMORY 30
#define OPAL_PCI_SET_PE 31
#define OPAL_PCI_SET_PELTV 32
#define OPAL_PCI_SET_MVE 33
#define OPAL_PCI_SET_MVE_ENABLE 34
#define OPAL_PCI_GET_XIVE_REISSUE 35
#define OPAL_PCI_SET_XIVE_REISSUE 36
#define OPAL_PCI_SET_XIVE_PE 37
#define OPAL_GET_XIVE_SOURCE 38
#define OPAL_GET_MSI_32 39
#define OPAL_GET_MSI_64 40
#define OPAL_START_CPU 41
#define OPAL_QUERY_CPU_STATUS 42
#define OPAL_WRITE_OPPANEL 43
#define OPAL_PCI_MAP_PE_DMA_WINDOW 44
#define OPAL_PCI_MAP_PE_DMA_WINDOW_REAL 45
#define OPAL_PCI_RESET 49
#define OPAL_PCI_GET_HUB_DIAG_DATA 50
#define OPAL_PCI_GET_PHB_DIAG_DATA 51
#define OPAL_PCI_FENCE_PHB 52
#define OPAL_PCI_REINIT 53
#define OPAL_PCI_MASK_PE_ERROR 54
#define OPAL_SET_SLOT_LED_STATUS 55
#define OPAL_GET_EPOW_STATUS 56
#define OPAL_SET_SYSTEM_ATTENTION_LED 57

#ifndef __ASSEMBLY__

enum OpalVendorApiTokens {
 OPAL_START_VENDOR_API_RANGE = 1000, OPAL_END_VENDOR_API_RANGE = 1999
};
enum OpalFreezeState {
 OPAL_EEH_STOPPED_NOT_FROZEN = 0,
 OPAL_EEH_STOPPED_MMIO_FREEZE = 1,
 OPAL_EEH_STOPPED_DMA_FREEZE = 2,
 OPAL_EEH_STOPPED_MMIO_DMA_FREEZE = 3,
 OPAL_EEH_STOPPED_RESET = 4,
 OPAL_EEH_STOPPED_TEMP_UNAVAIL = 5,
 OPAL_EEH_STOPPED_PERM_UNAVAIL = 6
};
enum OpalEehFreezeActionToken {
 OPAL_EEH_ACTION_CLEAR_FREEZE_MMIO = 1,
 OPAL_EEH_ACTION_CLEAR_FREEZE_DMA = 2,
 OPAL_EEH_ACTION_CLEAR_FREEZE_ALL = 3
};
enum OpalPciStatusToken {
 OPAL_EEH_PHB_NO_ERROR = 0,
 OPAL_EEH_PHB_FATAL = 1,
 OPAL_EEH_PHB_RECOVERABLE = 2,
 OPAL_EEH_PHB_BUS_ERROR = 3,
 OPAL_EEH_PCI_NO_DEVSEL = 4,
 OPAL_EEH_PCI_TA = 5,
 OPAL_EEH_PCIEX_UR = 6,
 OPAL_EEH_PCIEX_CA = 7,
 OPAL_EEH_PCI_MMIO_ERROR = 8,
 OPAL_EEH_PCI_DMA_ERROR = 9
};
enum OpalShpcAction {
 OPAL_SHPC_GET_LINK_STATE = 0,
 OPAL_SHPC_GET_SLOT_STATE = 1
};
enum OpalShpcLinkState {
 OPAL_SHPC_LINK_DOWN = 0,
 OPAL_SHPC_LINK_UP = 1
};
enum OpalMmioWindowType {
 OPAL_M32_WINDOW_TYPE = 1,
 OPAL_M64_WINDOW_TYPE = 2,
 OPAL_IO_WINDOW_TYPE = 3
};
enum OpalShpcSlotState {
 OPAL_SHPC_DEV_NOT_PRESENT = 0,
 OPAL_SHPC_DEV_PRESENT = 1
};
enum OpalExceptionHandler {
 OPAL_MACHINE_CHECK_HANDLER = 1,
 OPAL_HYPERVISOR_MAINTENANCE_HANDLER = 2,
 OPAL_SOFTPATCH_HANDLER = 3
};
enum OpalPendingState {
 OPAL_EVENT_OPAL_INTERNAL = 0x1,
 OPAL_EVENT_NVRAM = 0x2,
 OPAL_EVENT_RTC = 0x4,
 OPAL_EVENT_CONSOLE_OUTPUT = 0x8,
 OPAL_EVENT_CONSOLE_INPUT = 0x10,
 OPAL_EVENT_ERROR_LOG_AVAIL = 0x20,
 OPAL_EVENT_ERROR_LOG = 0x40,
 OPAL_EVENT_EPOW = 0x80,
 OPAL_EVENT_LED_STATUS = 0x100
};

enum OpalMCE_Version {
 OpalMCE_V1 = 1,
};

enum OpalMCE_Severity {
 OpalMCE_SEV_NO_ERROR = 0,
 OpalMCE_SEV_WARNING = 1,
 OpalMCE_SEV_ERROR_SYNC = 2,
 OpalMCE_SEV_FATAL = 3,
};

enum OpalMCE_Disposition {
 OpalMCE_DISPOSITION_RECOVERED = 0,
 OpalMCE_DISPOSITION_NOT_RECOVERED = 1,
};

enum OpalMCE_Initiator {
 OpalMCE_INITIATOR_UNKNOWN = 0,
 OpalMCE_INITIATOR_CPU = 1,
};

enum OpalMCE_ErrorType {
 OpalMCE_ERROR_TYPE_UNKNOWN = 0,
 OpalMCE_ERROR_TYPE_UE = 1,
 OpalMCE_ERROR_TYPE_SLB = 2,
 OpalMCE_ERROR_TYPE_ERAT = 3,
 OpalMCE_ERROR_TYPE_TLB = 4,
};

enum OpalMCE_UeErrorType {
 OpalMCE_UE_ERROR_INDETERMINATE = 0,
 OpalMCE_UE_ERROR_IFETCH = 1,
 OpalMCE_UE_ERROR_PAGE_TABLE_WALK_IFETCH = 2,
 OpalMCE_UE_ERROR_LOAD_STORE = 3,
 OpalMCE_UE_ERROR_PAGE_TABLE_WALK_LOAD_STORE = 4,
};

enum OpalMCE_SlbErrorType {
 OpalMCE_SLB_ERROR_INDETERMINATE = 0,
 OpalMCE_SLB_ERROR_PARITY = 1,
 OpalMCE_SLB_ERROR_MULTIHIT = 2,
};

enum OpalMCE_EratErrorType {
 OpalMCE_ERAT_ERROR_INDETERMINATE = 0,
 OpalMCE_ERAT_ERROR_PARITY = 1,
 OpalMCE_ERAT_ERROR_MULTIHIT = 2,
};

enum OpalMCE_TlbErrorType {
 OpalMCE_TLB_ERROR_INDETERMINATE = 0,
 OpalMCE_TLB_ERROR_PARITY = 1,
 OpalMCE_TLB_ERROR_MULTIHIT = 2,
};

enum OpalThreadStatus {
 OPAL_THREAD_INACTIVE = 0x0,
 OPAL_THREAD_STARTED = 0x1
};

enum OpalPciBusCompare {
 OpalPciBusAny = 0,
 OpalPciBus3Bits = 2,
 OpalPciBus4Bits = 3,
 OpalPciBus5Bits = 4,
 OpalPciBus6Bits = 5,
 OpalPciBus7Bits = 6,
 OpalPciBusAll = 7,
};

enum OpalDeviceCompare {
 OPAL_IGNORE_RID_DEVICE_NUMBER = 0,
 OPAL_COMPARE_RID_DEVICE_NUMBER = 1
};

enum OpalFuncCompare {
 OPAL_IGNORE_RID_FUNCTION_NUMBER = 0,
 OPAL_COMPARE_RID_FUNCTION_NUMBER = 1
};

enum OpalPeAction {
 OPAL_UNMAP_PE = 0,
 OPAL_MAP_PE = 1
};

enum OpalPeltvAction {
 OPAL_REMOVE_PE_FROM_DOMAIN = 0,
 OPAL_ADD_PE_TO_DOMAIN = 1
};

enum OpalMveEnableAction {
 OPAL_DISABLE_MVE = 0,
 OPAL_ENABLE_MVE = 1
};

enum OpalPciResetAndReinitScope {
 OPAL_PHB_COMPLETE = 1, OPAL_PCI_LINK = 2, OPAL_PHB_ERROR = 3,
 OPAL_PCI_HOT_RESET = 4, OPAL_PCI_FUNDAMENTAL_RESET = 5,
 OPAL_PCI_IODA_TABLE_RESET = 6,
};

enum OpalPciResetState {
 OPAL_DEASSERT_RESET = 0,
 OPAL_ASSERT_RESET = 1
};

enum OpalPciMaskAction {
 OPAL_UNMASK_ERROR_TYPE = 0,
 OPAL_MASK_ERROR_TYPE = 1
};

enum OpalSlotLedType {
 OPAL_SLOT_LED_ID_TYPE = 0,
 OPAL_SLOT_LED_FAULT_TYPE = 1
};

enum OpalLedAction {
 OPAL_TURN_OFF_LED = 0,
 OPAL_TURN_ON_LED = 1,
 OPAL_QUERY_LED_STATE_AFTER_BUSY = 2
};

enum OpalEpowStatus {
 OPAL_EPOW_NONE = 0,
 OPAL_EPOW_UPS = 1,
 OPAL_EPOW_OVER_AMBIENT_TEMP = 2,
 OPAL_EPOW_OVER_INTERNAL_TEMP = 3
};

struct opal_machine_check_event {
 enum OpalMCE_Version version:8;
 uint8_t in_use;
 enum OpalMCE_Severity severity:8;
 enum OpalMCE_Initiator initiator:8;
 enum OpalMCE_ErrorType error_type:8;
 enum OpalMCE_Disposition disposition:8;
 uint8_t reserved_1[2];
 uint64_t gpr3;
 uint64_t srr0;
 uint64_t srr1;
 union {
 struct {
 enum OpalMCE_UeErrorType ue_error_type:8;
 uint8_t effective_address_provided;
 uint8_t physical_address_provided;
 uint8_t reserved_1[5];
 uint64_t effective_address;
 uint64_t physical_address;
 uint8_t reserved_2[8];
 } ue_error;

 struct {
 enum OpalMCE_SlbErrorType slb_error_type:8;
 uint8_t effective_address_provided;
 uint8_t reserved_1[6];
 uint64_t effective_address;
 uint8_t reserved_2[16];
 } slb_error;

 struct {
 enum OpalMCE_EratErrorType erat_error_type:8;
 uint8_t effective_address_provided;
 uint8_t reserved_1[6];
 uint64_t effective_address;
 uint8_t reserved_2[16];
 } erat_error;

 struct {
 enum OpalMCE_TlbErrorType tlb_error_type:8;
 uint8_t effective_address_provided;
 uint8_t reserved_1[6];
 uint64_t effective_address;
 uint8_t reserved_2[16];
 } tlb_error;
 } u;
};

enum {
 OPAL_P7IOC_NUM_PEST_REGS = 128,
};

struct OpalIoP7IOCPhbErrorData {
 uint32_t brdgCtl;

 uint32_t portStatusReg;
 uint32_t rootCmplxStatus;
 uint32_t busAgentStatus;

 uint32_t deviceStatus;
 uint32_t slotStatus;
 uint32_t linkStatus;
 uint32_t devCmdStatus;
 uint32_t devSecStatus;

 uint32_t rootErrorStatus;
 uint32_t uncorrErrorStatus;
 uint32_t corrErrorStatus;
 uint32_t tlpHdr1;
 uint32_t tlpHdr2;
 uint32_t tlpHdr3;
 uint32_t tlpHdr4;
 uint32_t sourceId;

 uint32_t rsv3;

 uint64_t errorClass;
 uint64_t correlator;

 uint64_t p7iocPlssr;
 uint64_t p7iocCsr;
 uint64_t lemFir;
 uint64_t lemErrorMask;
 uint64_t lemWOF;
 uint64_t phbErrorStatus;
 uint64_t phbFirstErrorStatus;
 uint64_t phbErrorLog0;
 uint64_t phbErrorLog1;
 uint64_t mmioErrorStatus;
 uint64_t mmioFirstErrorStatus;
 uint64_t mmioErrorLog0;
 uint64_t mmioErrorLog1;
 uint64_t dma0ErrorStatus;
 uint64_t dma0FirstErrorStatus;
 uint64_t dma0ErrorLog0;
 uint64_t dma0ErrorLog1;
 uint64_t dma1ErrorStatus;
 uint64_t dma1FirstErrorStatus;
 uint64_t dma1ErrorLog0;
 uint64_t dma1ErrorLog1;
 uint64_t pestA[OPAL_P7IOC_NUM_PEST_REGS];
 uint64_t pestB[OPAL_P7IOC_NUM_PEST_REGS];
};

typedef struct oppanel_line {
 const char * line;
 uint64_t line_len;
} oppanel_line_t;

struct rtc_time;

#endif

#endif