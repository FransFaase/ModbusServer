
#define DATA_QUEUE_SIZE 1024

typedef struct dataQueue
{
    uint32_t size;
    uint32_t read_pos;
    uint32_t write_pos;
    uint8_t data[DATA_QUEUE_SIZE];
    TaskId wait_read_task_id;
    uint32_t *read_data;
} DataQueue;

extern void DataQueueInit(DataQueue *dataQueue);
inline bool DataQueueEmpty(DataQueue *dataQueue) { return dataQueue->size == 0; }
inline bool DataQueueFull(DataQueue *dataQueue) { return dataQueue->size == DATA_QUEUE_SIZE; }
inline uint32_t DataQueueRoom(DataQueue *dataQueue) { return DATA_QUEUE_SIZE - dataQueue->size; }
extern uint32_t DataQueueRead(DataQueue *dataQueue);
extern bool DataQueueTryRead(DataQueue *dataQueue, uint32_t *data, TaskId taskId);
extern void DataQueueWrite(DataQueue *dataQueue, uint32_t data);
extern void DataQueueHasData(DataQueue *dataQueue);
