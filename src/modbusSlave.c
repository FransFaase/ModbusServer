#include <stdint.h>
#include <stdbool.h>
#include "tcpos.h"
#include "dataQueue.h"
#include "modbus.h"
#include "modbusSlave.h"
#include "coroutine.h"
#include "crc16.h"

#define NR_REGISTERS 1024
#define FUNCTION_CODE_READ_REGISTERS  3
#define READ_REGISTERS_REMAINING 6
#define MAX_NR_READ_REGISTERS 127
#define RESPONSE_LEN 512
#define FUNCTION_CODE_WRITE_REGISTER  6
#define WRITE_REGISTER_REMAINING 6
#define FUNCTION_CODE_WRITE_REGISTERS 16
#define WRITE_REGISTERS_INFO 5
#define MAX_NR_WRITE_REGISTERS 127
#define WRITE_REGISTERS_RESP_LEN 4

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

        static uint8_t slave_nr;
        if (!DataQueueTryRead(&modbusReadDataQueue, &slave_nr, 1, taskid_modbus_slave))
        {
            COROUTINE_YIELD;
        }
        CRC16Add(slave_nr);

        static uint8_t function_code;
        if (!DataQueueTryRead(&modbusReadDataQueue, &function_code, 1, taskid_modbus_slave))
        {
            COROUTINE_YIELD;
        }
        CRC16Add(function_code);

        if (function_code == FUNCTION_CODE_READ_REGISTERS)
        {
            static uint8_t remaining[READ_REGISTERS_REMAINING];
            if (!DataQueueTryRead(&modbusReadDataQueue, remaining, READ_REGISTERS_REMAINING, taskid_modbus_slave))
            {
                COROUTINE_YIELD;
            }
            if (CRC16Check(remaining, READ_REGISTERS_REMAINING))
            {
                uint32_t address = ((uint32_t)remaining[0] << 8) | remaining[1];
                uint32_t nr = ((uint32_t)remaining[2] << 8) | remaining[3];
                if (nr < MAX_NR_READ_REGISTERS)
                {
                    response[0] = slave_nr;
                    response[1] = FUNCTION_CODE_READ_REGISTERS;
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
        else if (function_code == FUNCTION_CODE_WRITE_REGISTER)
        {
            static uint8_t remaining[WRITE_REGISTER_REMAINING];
            if (!DataQueueTryRead(&modbusReadDataQueue, remaining, WRITE_REGISTER_REMAINING, taskid_modbus_slave))
            {
                COROUTINE_YIELD;
            }
            if (CRC16Check(remaining, WRITE_REGISTER_REMAINING))
            {
                uint32_t address = ((uint32_t)remaining[0] << 8) | remaining[1];
                uint32_t data = ((uint32_t)remaining[2] << 8) | remaining[3];
                registers[(address) % NR_REGISTERS] = data;

                response[0] = slave_nr;
                response[1] = FUNCTION_CODE_WRITE_REGISTER;
                for (int i = 0; i < WRITE_REGISTER_REMAINING; i++)
                    response[i + 2] = remaining[i];
                if (!DataQueueTryWrite(&modbusWriteDataQueue, response, 2 + WRITE_REGISTER_REMAINING, taskid_modbus_slave))
                {
                    COROUTINE_YIELD;
                }
            }
        }
        else if (function_code == FUNCTION_CODE_WRITE_REGISTERS)
        {
            static uint8_t info[WRITE_REGISTERS_INFO];
            if (!DataQueueTryRead(&modbusReadDataQueue, info, WRITE_REGISTERS_INFO, taskid_modbus_slave))
            {
                COROUTINE_YIELD;
            }
            for (int i = 0; i < WRITE_REGISTERS_INFO; i++)
                CRC16Add(info[i]);
            if (!DataQueueTryRead(&modbusReadDataQueue, NULL, info[4] + 2, taskid_modbus_slave))
            {
                COROUTINE_YIELD;
            }
            uint32_t address = ((uint32_t)info[0] << 8) | info[1];
            uint32_t nr = ((uint32_t)info[2] << 8) | info[3];
            uint32_t nr_bytes = info[4];
            if (nr_bytes == nr * 2 && nr <= MAX_NR_WRITE_REGISTERS)
            {
                static uint32_t reg_values[MAX_NR_WRITE_REGISTERS];
                for (int i = 0; i < nr; i++)
                {
                    uint8_t hi = DataQueueRead(&modbusReadDataQueue);
                    CRC16Add(hi);
                    uint8_t lo = DataQueueRead(&modbusReadDataQueue);
                    CRC16Add(lo);
                    reg_values[i] = ((uint32_t)hi << 8) | lo;
                }
                uint8_t crc16[2];
                crc16[0] = DataQueueRead(&modbusReadDataQueue);
                crc16[1] = DataQueueRead(&modbusReadDataQueue);
                if (CRC16Check(crc16, 2))
                {
                    for (int i = 0; i < nr; i++)
                        registers[(address + i) % NR_REGISTERS] = reg_values[i];
                    response[0] = slave_nr;
                    response[1] = FUNCTION_CODE_WRITE_REGISTERS;
                    for (int i = 0; i < WRITE_REGISTERS_RESP_LEN; i++)
                        response[2 + i] = info[i];
                    CRC16Calculate(response, 2 + WRITE_REGISTERS_RESP_LEN + 2);
                    if (!DataQueueTryWrite(&modbusWriteDataQueue, response, 2 + WRITE_REGISTERS_RESP_LEN + 2, taskid_modbus_slave))
                    {
                        COROUTINE_YIELD;
                    }
                }
            }
        }
    }

    COROUTINE_END
}

