#include "soc/soc.h"
#include "soc/uart_struct.h"

FORCE_INLINE_ATTR uint32_t ModbusRXcount(void)
{
    return UART2.status.rxfifo_cnt;
}

FORCE_INLINE_ATTR uint8_t ModbusRX(void)
{
    return UART2.fifo.val;
}

FORCE_INLINE_ATTR void ModbusTX(uint8_t byte)
{
    UART2.fifo.val = (int)byte;
}