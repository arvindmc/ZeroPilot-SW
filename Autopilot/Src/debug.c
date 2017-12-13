
#include "debug.h"

int _write(int file, char *ptr, int len) {
#if DBG_TYPE == DBG_UART
    HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, 0xFF);
#elif DBG_TYPE == DBG_ITM
    for (uint8_t i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
#endif
    return len;
}

void ReceiveDbgCmd(void) {
  uint8_t rxData;

  // reset queue pointer
  ReceiveQueuePointer = 0;

  while (1) {
    // try to receive data
    HAL_UART_Receive_IT(&huart3, &rxData, 1);

    // binary semaphore is given after receive is complete
    if (xSemaphoreTake(debugSemaphoreHandle, (TickType_t)10) == pdTRUE) {
      if (rxData == '\r') {
        // if line break, end
        ReceiveQueue[ReceiveQueuePointer] = '\0';
        debug();
        break;
      } else {
        // push character on queue
        ReceiveQueue[ReceiveQueuePointer++] = rxData;
      }
    }
  }
}

void ProcessDbgCmd(char *cmd) {
  debugCmd(cmd);
}
