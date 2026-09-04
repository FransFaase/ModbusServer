#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "tcpos.h"
#include "dataQueue.h"

extern void DataQueueInit(DataQueue *dataQueue)
{
    dataQueue->size = 0;
    dataQueue->read_pos = 0;
    dataQueue->write_pos = 0;
    dataQueue->wait_read_task_id = taskid_none;
    dataQueue->read_data = NULL;
    dataQueue->wait_write_task_id = taskid_none;
    dataQueue->write_data = NULL;
}

extern uint8_t DataQueueRead(DataQueue *dataQueue)
{
    uint8_t result = dataQueue->data[dataQueue->read_pos];
    dataQueue->read_pos = (dataQueue->read_pos + 1) % DATA_QUEUE_SIZE;
    dataQueue->size--;
    return result;
}

extern void DataQueueDataRemoved(DataQueue *dataQueue)
{
    if (dataQueue->wait_write_task_id != taskid_none && dataQueue->write_length <= DataQueueRoom(dataQueue))
    {
        QueueAdd(queueid_main_queue, dataQueue->wait_write_task_id);
        dataQueue->wait_read_task_id = taskid_none;
        for (int i = 0; i < dataQueue->write_length; i++)
            DataQueueWrite(dataQueue, dataQueue->write_data[i]);
        dataQueue->write_data = NULL;
    }
}

extern bool DataQueueTryRead(DataQueue *dataQueue, uint8_t *data, uint32_t length, TaskId taskId)
{
    if (length <= DataQueueSize(dataQueue))
    {
        for (int i = 0; i < length; i++)
            data[i] = DataQueueRead(dataQueue);
        DataQueueDataRemoved(dataQueue);
        return true;
    }

    dataQueue->read_data = data;
    dataQueue->read_length = length;
    dataQueue->wait_read_task_id = taskId;
    return false;
}

extern void DataQueueWrite(DataQueue *dataQueue, uint8_t data)
{
    dataQueue->data[dataQueue->write_pos] = data;
    dataQueue->write_pos = (dataQueue->write_pos + 1) % DATA_QUEUE_SIZE;
    dataQueue->size++;
}

extern void DataQueueDataAdded(DataQueue *dataQueue)
{
    if (dataQueue->wait_read_task_id != taskid_none && dataQueue->size >= dataQueue->read_length)
    {
        QueueAdd(queueid_main_queue, dataQueue->wait_read_task_id);
        dataQueue->wait_read_task_id = taskid_none;
        for (int i = 0; i < dataQueue->read_length; i++)
            dataQueue->read_data[i] = DataQueueRead(dataQueue);
        dataQueue->read_data = NULL;
    }
}

extern bool DataQueueTryWrite(DataQueue *dataQueue, uint8_t *data, uint32_t length, TaskId taskId)
{
    if (length <= DataQueueRoom(dataQueue))
    {
        for (int i = 0; i < length; i++)
            DataQueueWrite(dataQueue, data[i]);
        DataQueueDataAdded(dataQueue);
        return true;
    }

    dataQueue->write_data = data;
    dataQueue->write_length = length;
    dataQueue->wait_write_task_id = taskId;

    return false;
}


