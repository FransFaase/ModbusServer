#include <stdbool.h>
#include <stdint.h>
#include "tcpos.h"
#include "dataQueue.h"
#include "modbus.h"
#include "hal/modbus.h"

void ModbusInit(void)
{
    TaskInit(taskid_modbus_read, ModbusReadTaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_read);
}

void ModbusReadTaskStep(void)
{
    int count = ModbusRXcount();
    if (count > 0)
    {
        uint32_t room = DataQueueRoom(&modebusReadDataQueue);
        if (count > room)
            count = room;
        if (count > 0)
        {
            for (int i = 0; i < count; i++)
                DataQueueWrite(&modebusReadDataQueue, ModbusRX());
            DataQueueHasData(&modebusReadDataQueue);
        }
    }

    QueueAdd(queueid_main_queue, taskid_modbus_read);
}