#include "functions.h"
#include <stdio.h>
int callback(const void* msg)
{
    printf("callback, msg = %d\r\n",*(int*)msg);

  //  *msg = 32;

    return 0;
}

enum Signals
{
    Head_X_TargetPosi = 1,
    Head_X_TargetSpeed,
    Head_X_TargetAccel,
    Head_X_TargetDecel,
    Head_Y_TargetPosi,
    Head_Y_TargetSpeed,
    Head_Y_TargetAccel,
    Head_Y_TargetDecel,
    RightArm_X_TargetPosi,
    RightArm_X_TargetSpeed,
    RightArm_X_TargetAccel,
    RightArm_X_TargetDecel,
    RightArm_Y_TargetPosi,
    RightArm_Y_TargetSpeed,
    RightArm_Y_TargetAccel,
    RightArm_Y_TargetDecel,
    LeftArm_X_TargetPosi,
    LeftArm_X_TargetSpeed,
    LeftArm_X_TargetAccel,
    LeftArm_X_TargetDecel,
    LeftArm_Y_TargetPosi,
    LeftArm_Y_TargetSpeed,
    LeftArm_Y_TargetAccel,
    LeftArm_Y_TargetDecel,
};

#define UART_RX_MAPPING(msg, sig, start, len) {\
                sig = 0; \
                for(int i=0;i<len;i++)  \
                    sig+=msg[(start+i)]<<(i*8); \
}

uint8_t testMsg[10] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09}; 

uint8_t testSig1 = 0;
uint16_t testSig2 = 2;
uint32_t testSig3 = 10;

////////////////////////////////////////////////////
// UART Messages    ////////////////////////////////
////////////////////////////////////////////////////
enum UartRxMessages
{
    HeadMotion      = 0x01,
    RightArmMotion  = 0x02,
    LeftArmMotion   = 0x03,
    WeelsMotion     = 0x04,
    LedMatrixCtrl   = 0x05,
};

enum UartTxMessages
{
    StatusMsg   = 0xFE,
    ErrorMsg    = 0xFF,
};

uint8_t RxBuffer[3][13] = {0};  // 5 messages , B0-ID, B1..B10-Data, B11-CRC, B12-(0=free, !0=occupied)
uint8_t RxBufferSet_Posi = 0;   // position to store the received message
uint8_t RxBufferGet_Posi = 0;   // position to process the next received message

uint8_t TxBuffer[3][13] = {0};  // 3 messages , B0-ID, B1..B10-Data, B11-CRC, B12-(0=free, !0=occupied)
uint8_t TxBufferSet_Posi = 0;   // position to set a new message for sending
uint8_t TxBufferGet_Posi = 0;   // position to send the next message

////////////////////////////////////////////////////
// TWI Messages ////////////////////////////////////
////////////////////////////////////////////////////


////////////////////////////////////////////////////
// SPI Messages ////////////////////////////////////
////////////////////////////////////////////////////



int main()
{
    int (*notification)(const void* msg);

    int RegNotifyCallback(int (*cb)(const void *msg))
    {
        notification = cb;
        return 0;
    }

    RegNotifyCallback(&callback);

    printf("Hello World!\r\n");

    printf("sumValues(%d, %d) = %d\r\n", 12, 11, sumValues(12,11));

    UART_RX_MAPPING(testMsg,testSig1,1,1);
    UART_RX_MAPPING(testMsg,testSig2,2,2);
    UART_RX_MAPPING(testMsg,testSig3,4,3);
    
    printf("Signal 1: %d\r\n", testSig1);
    printf("Signal 2: %d\r\n", testSig2);
    printf("Signal 3: %d\r\n", testSig3);

    int input = 1;
    while (input != 0)
    {
        printf("Zahl (0=Ende): ");
        scanf("%d",&input);
        notification(&input);
    }
    

    return 0;
}

