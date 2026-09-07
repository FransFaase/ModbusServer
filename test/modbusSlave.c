#include <unity.h>
#include "../src/modbusSlave.c"
#include "mock/modbus.h"
#include "modbus.h"


extern void TestModbusSlave(void)
{
    TcposInit();
    ModbusInit();
    ModbusSlaveInit();

    uint8_t readThreeRegistersRequest[] = { 
        0x01, // slave address 
        0x03, // function code
        0x00, 0x10, // Start register (16)
        0x00, 0x03, // Number registers (3)
        0x04, 0x0E  // CRC16
    };
    ModbusMockFeedRX(readThreeRegistersRequest, sizeof(readThreeRegistersRequest));

    TcposLoopN(20);

    uint8_t readThreeRegistersExpectedResponse[] = { 
        0x01, // slave address 
        0x03, // function code
        0x06, // nr bytes
        0x00, 0x10, // contents register 16
        0x00, 0x11, // contents register 17
        0x00, 0x12, // contenss register 18
        0x30, 0xBE
    };
    TEST_ASSERT_EQUAL_UINT32(sizeof(readThreeRegistersExpectedResponse), ModbusMockTXcount());
    for (int i = 0; i < sizeof(readThreeRegistersExpectedResponse); i++)
        TEST_ASSERT_EQUAL_UINT8(readThreeRegistersExpectedResponse[i], ModbusMockTX());

}

