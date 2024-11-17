#include "servo57.h"
#include <stdlib.h>
#include <generic/canserial.h>

static CanTxMessage CAN_TxMessage;
static CanRxMessage CAN_RxMessage;

void can_init(void) {
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    CAN_InitType CAN_InitStructure;
    CAN_FilterInitType CAN_FilterInitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin       = CAN_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_CAN;
    GPIO_InitPeripheral(CAN_PORT, &GPIO_InitStructure);
    /* Configure CAN TX PB9 */
    GPIO_InitStructure.Pin        = CAN_TX_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(CAN_PORT, &GPIO_InitStructure);

    /* Configure CAN */
    CAN_DeInit(CAN);
    /* Struct init*/
    CAN_InitStruct(&CAN_InitStructure);
    /* CAN cell init */
    CAN_InitStructure.TTCM              = DISABLE;
    CAN_InitStructure.ABOM              = DISABLE;
    CAN_InitStructure.AWKUM             = DISABLE;
    CAN_InitStructure.NART              = DISABLE;
    CAN_InitStructure.RFLM              = DISABLE;
    CAN_InitStructure.TXFP              = ENABLE;
    CAN_InitStructure.OperatingMode     = CAN_Normal_Mode;
    CAN_InitStructure.RSJW              = CAN_RSJW_1tq;
    CAN_InitStructure.TBS1              = CAN_TBS1_10tq;
    CAN_InitStructure.TBS2              = CAN_TBS2_5tq;
    CAN_InitStructure.BaudRatePrescaler = 2;
    /*Initializes the CAN */
    CAN_Init(CAN, &CAN_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = CAN_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_FilterInitStructure.Filter_Num            = 0;
    CAN_FilterInitStructure.Filter_Mode           = CAN_Filter_IdMaskMode;
    CAN_FilterInitStructure.Filter_Scale          = CAN_Filter_32bitScale;
    CAN_FilterInitStructure.Filter_HighId         = CANBUS_ID_ADMIN >> 16;
    CAN_FilterInitStructure.Filter_LowId          = CANBUS_ID_ADMIN & 0xFFFF;
    CAN_FilterInitStructure.FilterMask_HighId     = 0;
    CAN_FilterInitStructure.FilterMask_LowId      = 0;
    CAN_FilterInitStructure.Filter_FIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.Filter_Act            = ENABLE;
    CAN_InitFilter(&CAN_FilterInitStructure);
    CAN_INTConfig(CAN, CAN_INT_FMP0, ENABLE);

    canserial_set_uuid((void*)UID_BASE, UID_LENGTH);
}

void can_tx(uint8_t *data, uint8_t len) {
    CAN_TxMessage.StdId   = 0x0400; 
    CAN_TxMessage.IDE     = CAN_ID_STD;  
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;   
    CAN_TxMessage.DLC     = len;  
    for(int i = 0; i < len; i++) {
       CAN_TxMessage.Data[i] = data[i];
    }
    int mb = CAN_TransmitMessage(CAN, &CAN_TxMessage);
    print_log("TX to %d\n", mb);
}

void CAN_RX0_IRQHandler(void) {
    /* receive interrupt */
    if (CAN_GetIntStatus(CAN, CAN_INT_FMP0)) {
        CAN_ReceiveMessage(CAN, CAN_FIFO0, &CAN_RxMessage);
        CAN_ClearINTPendingBit(CAN, CAN_INT_FMP0);
    }
    /* send interrupt */
    else if (CAN_GetFlagSTS(CAN, CAN_FLAG_RQCPM0)) {
        CAN_ClearFlag(CAN, CAN_FLAG_RQCPM0);
    }
    /* data overflow interrupt */
    else if (CAN_GetIntStatus(CAN, CAN_INT_FOV0)) {

    }
    print_log("CAN IRQ\n");
}