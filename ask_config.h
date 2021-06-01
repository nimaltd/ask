#ifndef _ASK_CONFIG_H_
#define _ASK_CONFIG_H_

#define _ASK_MIN_BYTE_LEN_              3       //  byte
#define _ASK_MAX_BYTE_LEN_              3       //  byte
#define _ASK_MIN_NEW_FRAM_DETECT_TIME_  5000    //  us  
#define _ASK_TOLERANCE_                 40      //  %

#if (_ASK_MAX_BYTE_LEN_ < _ASK_MIN_BYTE_LEN_)
#error SELECT CORRECT ASK MIN/MAX BYTE LENGHT
#endif

#endif
