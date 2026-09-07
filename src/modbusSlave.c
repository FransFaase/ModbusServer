#include <stdint.h>
#include <stdbool.h>
#include "tcpos.h"
#include "dataQueue.h"
#include "modbus.h"
#include "modbusSlave.h"
#include "coroutine.h"
#include "crc16.h"

#define NR_REGISTERS 1024
#define FUNCTION_CODE_READ_REGISTER  3
#define READ_REMAIING 6
#define MAX_NR_READ_REGISTERS 128
#define RESPONSE_LEN 512

uint32_t registers[NR_REGISTERS];

extern void ModbusSlaveInit(void)
{
    for (int i = 0; i < NR_REGISTERS; i++)
        registers[i] = i;

    TaskInit(taskid_modbus_slave, ModbusSlaveTaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_slave);
}

extern void ModbusSlaveTaskStep(void)
{
    static uint8_t response[RESPONSE_LEN];

    COROUTINE_BEGIN
    
    for (;;)
    {
        CRC16Start();

        uint8_t slave_nr;
        if (!DataQueueTryRead(&modbusReadDataQueue, &slave_nr, 1, taskid_modbus_slave))
        {
            COROUTINE_YIELD;
        }
        CRC16Add(slave_nr);

        uint8_t function_code;
        if (!DataQueueTryRead(&modbusReadDataQueue, &function_code, 1, taskid_modbus_slave))
        {
            COROUTINE_YIELD;
        }
        CRC16Add(function_code);

        if (function_code == FUNCTION_CODE_READ_REGISTER)
        {
            uint8_t remaining[READ_REMAIING];
            if (!DataQueueTryRead(&modbusReadDataQueue, remaining, READ_REMAIING, taskid_modbus_slave))
            {
                COROUTINE_YIELD;
            }
            if (CRC16Check(remaining, READ_REMAIING))
            {
                uint32_t address = ((uint32_t)remaining[0] << 8) | remaining[1];
                uint32_t nr = ((uint32_t)remaining[2] << 8) | remaining[3];
                if (nr < MAX_NR_READ_REGISTERS)
                {
                    response[0] = slave_nr;
                    response[1] = FUNCTION_CODE_READ_REGISTER;
                    response[2] = 2 * nr;
                    for (int i = 0; i < nr; i++)
                    {
                        response[3 + 2 * i] = (registers[(address + i) % NR_REGISTERS] >> 8) & 0xFF;
                        response[4 + 2 * i] = registers[(address + i) % NR_REGISTERS] & 0xFF;
                    }
                    CRC16Calculate(response, 5 + 2 * nr);
                    if (!DataQueueTryWrite(&modbusWriteDataQueue, response, 5 + 2 * nr, taskid_modbus_slave))
                    {
                        COROUTINE_YIELD;
                    }
                }
            }
        }
    }

    COROUTINE_END
}

