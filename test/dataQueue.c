#include <unity.h>
#include "../src/dataQueue.c"

extern void TestDataQueue(void)
{
    DataQueue dataQueue;
    DataQueueInit(&dataQueue);

    TEST_ASSERT_EQUAL_UINT32(0, DataQueueSize(&dataQueue));
    TEST_ASSERT_EQUAL_UINT32(DATA_QUEUE_SIZE, DataQueueRoom(&dataQueue));

    uint8_t data1[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    TEST_ASSERT_TRUE(DataQueueTryWrite(&dataQueue, data1, 10, taskid_modbus_read));
    TEST_ASSERT_EQUAL_UINT32(10, DataQueueSize(&dataQueue));
    TEST_ASSERT_EQUAL_UINT32(DATA_QUEUE_SIZE-10, DataQueueRoom(&dataQueue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));

    uint8_t data2[8];
    TEST_ASSERT_TRUE(DataQueueTryRead(&dataQueue, data2, 8, taskid_modbus_write));
    TEST_ASSERT_EQUAL_UINT32(2, DataQueueSize(&dataQueue));
    for (int i = 0; i < 8; i++)
        TEST_ASSERT_EQUAL_UINT8(i, data2[i]);
    TEST_ASSERT_FALSE(DataQueueTryRead(&dataQueue, data2, 8, taskid_modbus_write));
    TEST_ASSERT_EQUAL_UINT32(2, DataQueueSize(&dataQueue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));

    uint8_t data3[8] = { 10, 11, 12, 13, 14, 15, 16, 17 };
    TEST_ASSERT_TRUE(DataQueueTryWrite(&dataQueue, data3, 8, taskid_modbus_read));
    TEST_ASSERT_EQUAL_UINT32(2, DataQueueSize(&dataQueue));
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    for (int i = 0; i < 8; i++)
        TEST_ASSERT_EQUAL_UINT8(i + 8, data2[i]);
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_write, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));

    TEST_ASSERT_TRUE(DataQueueTryRead(&dataQueue, data2, 2, taskid_modbus_write));
    TEST_ASSERT_EQUAL_UINT32(0, DataQueueSize(&dataQueue));
    for (int i = 0; i < 2; i++)
        TEST_ASSERT_EQUAL_UINT8(i + 16, data2[i]);

    TEST_ASSERT_TRUE(DataQueueTryWrite(&dataQueue, data1, 10, taskid_modbus_read));
    TEST_ASSERT_EQUAL_UINT32(10, DataQueueSize(&dataQueue));
    TEST_ASSERT_FALSE(DataQueueTryWrite(&dataQueue, data3, 8, taskid_modbus_read));
    TEST_ASSERT_EQUAL_UINT32(10, DataQueueSize(&dataQueue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_TRUE(DataQueueTryRead(&dataQueue, data2, 8, taskid_modbus_write));
    TEST_ASSERT_EQUAL_UINT32(10, DataQueueSize(&dataQueue));
    for (int i = 0; i < 8; i++)
        TEST_ASSERT_EQUAL_UINT8(i, data2[i]);
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_read, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));

    TEST_ASSERT_TRUE(DataQueueTryRead(&dataQueue, data2, 8, taskid_modbus_write));
    TEST_ASSERT_EQUAL_UINT32(2, DataQueueSize(&dataQueue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    for (int i = 0; i < 8; i++)
        TEST_ASSERT_EQUAL_UINT8(i + 8, data2[i]);

    TEST_ASSERT_TRUE(DataQueueTryRead(&dataQueue, data2, 2, taskid_modbus_write));
    TEST_ASSERT_EQUAL_UINT32(0, DataQueueSize(&dataQueue));
    for (int i = 0; i < 2; i++)
        TEST_ASSERT_EQUAL_UINT8(i + 16, data2[i]);

}

