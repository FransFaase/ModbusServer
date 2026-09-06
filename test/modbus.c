#include <unity.h>
#include <stdio.h>
#include "../src/modbus.c"


extern void TestModbus(void)
{
    ModbusInit();

    TEST_ASSERT_EQUAL_UINT32(0, DataQueueSize(&modbusReadDataQueue));

    TEST_ASSERT_EQUAL_UINT32(10, TcposLoopN(10));

    uint8_t data1[6] = "hallo"; 
    ModbusMockFeedRX(data1, 5);

    TEST_ASSERT_EQUAL_UINT32(10, TcposLoopN(10));

    TEST_ASSERT_EQUAL_UINT32(5, DataQueueSize(&modbusReadDataQueue));

    for (int i = 0; i < 5; i++)
    {
        uint8_t v = DataQueueRead(&modbusReadDataQueue);
        TEST_ASSERT_EQUAL_CHAR(data1[i], v);
    }

    uint8_t data2[6] = "World";
    TEST_ASSERT_TRUE(DataQueueTryWrite(&modbusWriteDataQueue, data2, 5, taskid_none));

    TEST_ASSERT_EQUAL_UINT32(5, TcposLoopN(5));
    TEST_ASSERT_EQUAL_UINT32(0, dataTXcount);

    TXhasRoom = 10;

    TEST_ASSERT_EQUAL_UINT32(4, TcposLoopN(4));
    TEST_ASSERT_EQUAL_UINT32(5, ModbusMockTXcount());

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQUAL_UINT8(data2[i], ModbusMockTX());

}
