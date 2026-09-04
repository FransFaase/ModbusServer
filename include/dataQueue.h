#define DATA_QUEUE_SIZE 1024

typedef struct dataQueue
{
    uint32_t size;
    uint32_t read_pos;
    uint32_t write_pos;
    uint8_t data[DATA_QUEUE_SIZE];
    TaskId wait_read_task_id;
    uint8_t *read_data;
    uint32_t read_length;
    TaskId wait_write_task_id;
    uint8_t *write_data;
    uint32_t write_length;
} DataQueue;

extern void DataQueueInit(DataQueue *dataQueue);

inline uint32_t DataQueueSize(DataQueue *dataQueue) { return dataQueue->size; }
extern uint8_t DataQueueRead(DataQueue *dataQueue);
extern void DataQueueDataRemoved(DataQueue *dataQueue);
extern bool DataQueueTryRead(DataQueue *dataQueue, uint8_t *data, uint32_t length, TaskId taskId);

inline uint32_t DataQueueRoom(DataQueue *dataQueue) { return DATA_QUEUE_SIZE - dataQueue->size; }
extern void DataQueueWrite(DataQueue *dataQueue, uint8_t data);
extern void DataQueueDataAdded(DataQueue *dataQueue);
extern bool DataQueueTryWrite(DataQueue *dataQueue, uint8_t *data, uint32_t length, TaskId taskId);
