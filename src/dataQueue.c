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
}

extern uint32_t DataQueueRead(DataQueue *dataQueue)
{
    uint32_t result = dataQueue->data[dataQueue->read_pos];
    dataQueue->read_pos = (dataQueue->read_pos + 1) % DATA_QUEUE_SIZE;
    dataQueue->size--;
    return result;
}

extern bool DataQueueTryRead(DataQueue *dataQueue, uint32_t *data, TaskId taskId)
{
    if (!DataQueueEmpty(dataQueue))
    {
        *data = DataQueueRead(dataQueue);
        return true;
    }

    dataQueue->read_data = data;
    dataQueue->wait_read_task_id = taskId;
    return false;
}

extern void DataQueueWrite(DataQueue *dataQueue, uint32_t data)
{
    dataQueue->data[dataQueue->write_pos] = data;
    dataQueue->write_pos = (dataQueue->write_pos + 1) % DATA_QUEUE_SIZE;
    dataQueue->size++;
}

extern void DataQueueHasData(DataQueue *dataQueue)
{
    if (dataQueue->wait_read_task_id != taskid_none)
    {
        QueueAdd(queueid_main_queue, dataQueue->wait_read_task_id);
        dataQueue->wait_read_task_id = taskid_none;
        *dataQueue->read_data = DataQueueRead(dataQueue);
        dataQueue->read_data = NULL;
    }
}

