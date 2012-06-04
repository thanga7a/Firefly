//Header File for the Firefly XCVR Capability
#ifndef FF_XCVR
#define FF_XCVR

#include "Arduino.h"

//Arduino Pin Defs
#define TX_nRX 8						//Pin for the TX/!RX Line
#define XCVR_DATA 9



//Public Functional Prototypes
void XCVR_send_char(unsigned char);
void XCVR_init(void);
unsigned char XCVR_available(void);
unsigned char XCVR_read(void);
void XCVR_flush(void);

#endif
