#include <stdbool.h>
#include <stdint.h>
#include "tcpos.h"
#include "dataQueue.h"
#include "modbus.h"
#include "hal/modbus.h"

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
    uint32_t count = DataQueueSize(&modbusWriteDataQueue);
    if (count > 0 && count < ModbusTXRoom())
    {
        for (int i = 0; i < count; i++)
            ModbusTX(DataQueueRead(&modbusWriteDataQueue));
    }
}

void ModbusInit(void)
{
    TaskInit(taskid_modbus_read, ModbusReadTaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_read);
    TaskInit(taskid_modbus_write, ModbusWriteTaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_write);
}

