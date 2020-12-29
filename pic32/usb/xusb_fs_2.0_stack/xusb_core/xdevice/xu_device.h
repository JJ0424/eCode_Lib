
/*----------------------------------------------------------------------^^-
/ File name:  xu_device.h
/ Author:     JiangJun
/ Data:       [2020-12-14]
/ Version:    v1.33
/-----------------------------------------------------------------------^^-
/ usb2.0 middle layer
/------------------------------------------------------------------------*/

#ifndef _XU_DEVICE_H
#define _XU_DEVICE_H


#include "xu_lib_mx2xx.h"


//------------------------------------------------------------
//              CONFIG
//------------------------------------------------------------

#define _XUD_EP0_BUFF_SIZE              64



//------------------------------------------------------------
//              DESCRIPITION
//------------------------------------------------------------

// Setup Packet
typedef struct __attribute__ ((packed))
{

    // request type
    // 
    // D7:      transfer dir
    //          0 = host to device
    //          1 = device to host
    // ---
    // D6-5:    type
    //          0 = standard
    //          1 = class
    //          2 = vendor
    //          3 = reserved
    // ---
    // D4-0:    recipient
    //          0 = device
    //          1 = interface
    //          2 = endpoint
    //          3 = other
    //          4...31 = reserved
    
    u8 _bm_rtype;       
    
    u8 _b_req;          // specific request 
    u16 _w_val;         // value with the request
    u16 _w_idx;         // index with the request
    u16 _w_len;         // bytes to transfer if has 'data stage'
    
} XUD_DevRequestT;

// Standard Request
typedef enum
{

    _XUD_STDR_GET_STATUS = 0,
    _XUD_STDR_CLEAR_FEATURE,
    _XUD_STDR_RESERVED1,
    _XUD_STDR_SET_FEATURE,
    _XUD_STDR_RESERVED2,
    _XUD_STDR_SET_ADDRESS,
    _XUD_STDR_GET_DESCRIPTOR,
    _XUD_STDR_SET_DESCRIPTOR,
    _XUD_STDR_GET_CONFIGURATION,
    _XUD_STDR_SET_CONFIGURATION,
    _XUD_STDR_GET_INTERFACE,
    _XUD_STDR_SET_INTERFACE,
    _XUD_STDR_SYNCH_FRAME,
    _XUD_STDR_CNT = 13,                 // the numner of request
    _XUD_STDR_NONE,
    
} XUD_StdReqEnumT;


//-------------------------------------------------------
//              Get Descriptor
//-------------------------------------------------------

// Type
typedef enum
{

    _XUD_DESC_NONE = 0,
    _XUD_DESC_TYPE_DEVICE,              // device descriptor
    _XUD_DESC_TYPE_CONFIG,              // config descriptor
    _XUD_DESC_TYPE_STRING,              // string descriptor
    _XUD_DESC_TYPE_INTERFACE,           // interface descriptor
    _XUD_DESC_TYPE_ENDPOINT,            // endpoint descriptor
    _XUD_DESC_TYPE_DEV_QLF,             // device qualifier
    _XUD_DESC_TYPE_OTHER_SPD,           // other speed config
    _XUD_DESC_TYPE_INTER_PWR,           // interface power
    
} XUD_DescTypeT;

// Device Descriptor
typedef struct __attribute__ ((packed))
{

    u8 _b_len;                          // length of this descriptor
    u8 _b_desc_type;                    // descriptor type, 0x01
    u16 _bcd_usb;                       // USB Spec Release Number (BCD)
    u8 _b_dev_class;                    // class code (USB-IF)
    u8 _b_dev_subclass;                 // subclass code (USB-IF)
    u8 _b_dev_prot;                     // protocol code (USB-IF)
    u8 _b_max_pktsize0;                 // max packet size for EP0
    u16 _id_vendor;                     // vendor ID (USB-IF)
    u16 _id_product;                    // product ID
    u16 _bcd_dev;                       // device release number (BCD)
    u8 _i_manufacturer;                 // idx of manufacturer
    u8 _i_product;                      // idx of product
    u8 _i_sn;                           // idx of device's serial number
    u8 _b_num_conf;                     // number of possible configurations.
    
} XUD_DevDescT;

// Configuartion Descriptor
typedef struct __attribute__ ((packed))
{

    u8 _b_len;                          // length of this descriptor
    u8 _b_desc_type;                    // descriptor type, 0x02
    u16 _w_total_len;                   // total length of all descriptors for this configuration
    u8 _b_num_inters;                   // number of interfaces in this configuration
    u8 _b_conf_val;                     // value of this configuration (1 based)
    u8 _i_conf;                         // index of String Descriptor describing the configuration
    u8 _bm_attributes;                  // configuration characteristics
    u8 _b_max_pwr;                      // maximum power of this conf, * 50mA
    
} XUD_ConfiDescT;

// Interface Descriptor
typedef struct __attribute__ ((packed))
{

    u8 _b_len;                          // length of this descriptor
    u8 _b_desc_type;                    // descriptor type, 0x04
    u8 _b_inter_num;                    // number of this interface (0 based)
    u8 _b_alternate_set;                // value of this alternate interface setting
    u8 _b_num_epns;                     // number of endpoints in this interface
    u8 _b_inter_class;                  // class code (USB-IF)
    u8 _b_inter_subClass;               // subclass code (USB-IF)
    u8 _b_inter_protocol;               // protocol code (USB-IF)
    u8 _i_inter;                        // idx of String Descriptor describing the interface
    
} XUD_InterDescT;

// End-Point Descriptor
typedef struct __attribute__ ((packed))
{

    u8 _b_len;                          // length of this descriptor
    u8 _b_desc_type;                    // descriptor type, 0x05
    u8 _b_epn_addr;                     // endpoint address. Bit 7 indicates direction (0 = OUT, 1 = IN)
    u8 _bm_attributes;                  // endpoint transfer type
    u16 _w_max_pktsize;                 // maximum packet size
    u8 _b_interval;                     // polling interval in frames
    
} XUD_EpnDescT;

// String Descriptor
typedef struct __attribute__ ((packed))
{

    u8 _b_len;                          // length of this descriptor
    u8 _b_desc_type;                    // descriptor type, 0x03
    u8 *_p_str;                         // pointer to string
    
} XUD_StrDescT;


//-------------------------------------------------------
//              Enum Fields
//-------------------------------------------------------

// Interface Class Defination
typedef enum {

    _XUD_INTER_CLASS_NONE = 0,
    _XUD_INTER_CLASS_AUDIO = 0x01,      // audio class
    _XUD_INTER_CLASS_HID = 0x03,        // HID device
    _XUD_INTER_CLASS_MASS = 0x08,       // mass storage

} XUD_InterClassEnum;


//-------------------------------------------------------
//              Descriptor Group
//-------------------------------------------------------

typedef struct {

    // device
    XUD_DevDescT _dev_desc;             // point to device descriptor

    // conf
    u8 _conf_size;                      // conf/ inter/ end-point descriptor...
    u8 *_p_conf_desc;                   // point to descriptor array

    // str
    XUD_StrDescT _str_desc[4];          // 0: Lan-ID, normal: 1-manufacturer, 2-product, 3-s/n

} XUD_DescGroupT;



//------------------------------------------------------------
//              RUNNING TYPEDEF
//------------------------------------------------------------

// Device Status
typedef enum {

    _XUD_STA_ATTACHED = 0,              // wait VBUS attached
    _XUD_STA_POWERED,                   // wait for reset
    _XUD_STA_DEFAULT,                   // SETUP before addressed
    _XUD_STA_ADDRESS,                   // address transfer
    _XUD_STA_CONFIGUARED,               // configuartion transfer
    _XUD_STA_SUSPENDED,                 // USB module enter Low-Power mode
    
} XUD_StatusEnumT;

// F-Res
typedef enum {

    _XUD_FRES_NONE = 0,                 // system error
    _XUD_FRES_OK,                       // other EP return
    _XUD_FRES_SETUP,                    // enter SETUP status
    _XUD_FRES_TX,                       // return the next tx packet
    _XUD_FRES_RX,                       // return the next rx packet
    _XUD_FRES_NOT_SUPPORT_TX,           // STALL with Tx BDT
    _XUD_FRES_NOT_SUPPORT_RX,           // STALL with Rx BDT

    // Error
    _XUD_FRES_ER_PARSE_TK_E01,          // dispatch task parse token
    _XUD_FRES_ER_EP0_TX_SIZE_E02,       // new token size != ready now cnt
    _XUD_FRES_ER_EP0_TOKEN_CHK_E03,     // token is not SETUP/ IN/ OUT
    _XUD_FRES_ER_CONFIG_ERROR_E04,      // set-configuartion error
    _XUD_FRES_ER_CLS_DAT_TX_MUCH_E05,   // the tx data size > 64 bytes
    _XUD_FRES_ER_SET_LOGIC_ER_E06,      // error with "set-operation"

    // Other EP Error
    _XUD_FRES_ER_DATA_NOT_READY_E11,    // data not ready with that ep

} XUD_FResT;


//-------------------------------------------------------
//              Device Run-Status
//-------------------------------------------------------

typedef struct {


    //-------------------------------------------------------
    //              system run-value
    //-------------------------------------------------------
    
    // status
    XUD_StatusEnumT _dstatus;

    // usb device address
    u8 _usb_addr;                       // 1 - 127

    // SOF frame number
    u16 _sof_frame;                     // 11bit SOF frame number

    // point to descriptor group
    XUD_DescGroupT *_p_desc_set;        // device/ conf/ interface...

    // usb token now
    XUL_TxRxEndT _epn_new_tk;           // the new token [ SETUP/ IN/ OUT ]    


    //-------------------------------------------------------
    //              EP0 run-value
    //-------------------------------------------------------

    // ep0 status
    XUL_TxRxEndT _ep0_new_tx;           // the new tx token [ IN ]
    XUL_TxRxEndT _ep0_new_rx;           // the new rx token [ SETUP/ OUT ]

    // ep0 buffer
    u8 _ep0_tx_buff[_XUD_EP0_BUFF_SIZE];
    u8 _ep0_rx_buff[_XUD_EP0_BUFF_SIZE];
        
    // ep0 standard request
    u8 _ep0_host_need;                  // the host wLength Field
    u8 _ep0_total_cnt;                  // total data with this sreq
    u8 _ep0_rdy_now_cnt;                // the count of this tx/ rx
    u8 _ep0_dpassed_cnt;                // ep0 passed data
    u8 *_p_ep0_desc;                    // ep0 data point

    // ep0 standard request    
    XUD_StdReqEnumT _ep0_sreq_now;      // requset indictor
    XUD_DevRequestT _ep0_dreq_now;      // the device request now
    
} XUD_RunStatusT;


//-------------------------------------------------------
//              Report Task Status
//-------------------------------------------------------

// point to report task status
typedef void (*fpXUD_TaskReport)(u8, XUD_RunStatusT *, XUL_TxRxEndT *, XUD_FResT);


//------------------------------------------------------------
//              FUNCTION
//------------------------------------------------------------

// STATIC
extern XUD_FResT XUD_PoweredStatus(XUD_RunStatusT *, XUL_TxRxEndT *, XUL_TxReadyT *, XUL_RxReadyT *);
extern XUD_FResT XUD_DefaultStatus(XUD_RunStatusT *, XUL_TxRxEndT *, XUL_TxReadyT *, XUL_RxReadyT *);
extern XUD_FResT XUD_AddressStatus(XUD_RunStatusT *, XUL_TxRxEndT *, XUL_TxReadyT *, XUL_RxReadyT *);
extern XUD_FResT XUD_ConfuredStatus(XUD_RunStatusT *, XUL_TxRxEndT *, XUL_TxReadyT *, XUL_RxReadyT *);
extern XUD_FResT XUD_SuspendedStatus(XUD_RunStatusT *, XUL_TxRxEndT *, XUL_TxReadyT *, XUL_RxReadyT *);

extern XUD_FResT XUD_EP0_Request(XUD_RunStatusT *, XUL_TxRxEndT *, XUL_TxReadyT *, XUL_RxReadyT *);

extern XUD_FResT XUD_GetStatus(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_ClearFeature(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_SetFeature(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_SetAddress(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_GetDescriptor(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);

extern XUD_FResT XUD_SetDescriptor(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_GetConfiguration(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_SetConfiguration(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_GetInterface(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_SetInterface(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);
extern XUD_FResT XUD_SyncFrame(XUD_RunStatusT *, XUD_DevRequestT *, u8 *, u8 *);

// EXTERN
extern void XUD_Init(fpXUD_TaskReport);
extern void XUD_DispatchTasks(void);
extern void XUD_GetDeviceStatus(XUD_StatusEnumT *stat, u8 *addr, u16 *fnum);

// EXTERN PUBLIC
extern void _xud_memset(void *, u8, u8);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//
