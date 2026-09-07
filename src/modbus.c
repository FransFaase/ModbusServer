#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "tcpos.h"
#include "dataQueue.h"
#include "modbus.h"
#ifndef UNITY
#include "hal/modbus.h"
#else
#include "mock/modbus.c"
#endif
#include "coroutine.h"

DataQueue modbusReadDataQueue;
DataQueue modbusWriteDataQueue;

void ModbusReadTaskStep(void)
{
    uint32_t count = ModbusRXcount();
    if (count > 0)
    {
        uint32_t room = DataQueueRoom(&modbusReadDataQueue);
        if (count > room)
            count = room;
        if (count > 0)
        {
            for (int i = 0; i < count; i++)
                DataQueueWrite(&modbusReadDataQueue, ModbusRX());
            DataQueueDataAdded(&modbusReadDataQueue);
        }
    }

    QueueAdd(queueid_main_queue, taskid_modbus_read);
}

void ModbusWriteTaskStep(void)
{
    COROUTINE_BEGIN

    if (!DataQueueTryRead(&modbusWriteDataQueue, NULL, 1, taskid_modbus_write))
    {
        COROUTINE_YIELD;
    }

    uint32_t count = DataQueueSize(&modbusWriteDataQueue);
    uint32_t room = ModbusTXRoom();
    if (count > room)
        count = room;

    for (int i = 0; i < count; i++)
        ModbusTX(DataQueueRead(&modbusWriteDataQueue));

    COROUTINE_END

    QueueAdd(queueid_main_queue, taskid_modbus_write);
}

void ModbusInit(void)
{
    DataQueueInit(&modbusReadDataQueue);
    DataQueueInit(&modbusWriteDataQueue);
    TaskInit(taskid_modbus_read, ModbusReadTaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_read);
    TaskInit(taskid_modbus_write, ModbusWriteTaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_write);
}

