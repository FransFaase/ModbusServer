#include <unity.h>
#include "../src/modbusServer.c"
#include "mock/modbus.h"
#include "modbus.h"


extern void TestModbusServer(void)
{
    TcposInit();
    ModbusInit();
    ModbusServerInit();

    uint8_t writeRegisterRequest[] = { 
        0x01, // server address 
        0x06, // function code
        0x00, 0x11, // Start register (17)
        0x04, 0x03, // Value 
        0x9B, 0x0E  // CRC16
    };
    ModbusMockFeedRX(writeRegisterRequest, sizeof(writeRegisterRequest));

    TcposLoopN(20);

    TEST_ASSERT_EQUAL_UINT32(sizeof(writeRegisterRequest), ModbusMockTXcount());
    for (int i = 0; i < sizeof(writeRegisterRequest); i++)
        TEST_ASSERT_EQUAL_UINT8(writeRegisterRequest[i], ModbusMockTX());

    uint8_t readThreeRegistersRequest[] = { 
        0x01, // server address 
        0x03, // function code
        0x00, 0x10, // Start register (16)
        0x00, 0x03, // Number registers (3)
        0x04, 0x0E  // CRC16
    };
    ModbusMockFeedRX(readThreeRegistersRequest, sizeof(readThreeRegistersRequest));

    TcposLoopN(30);

    uint8_t readThreeRegistersExpectedResponse[] = { 
        0x01, // server address 
        0x03, // function code
        0x06, // nr bytes
        0x00, 0x10, // contents register 16
        0x04, 0x03, // contents register 17
        0x00, 0x12, // contenss register 18
        0x91, 0x8B  // CRC16
    };
    TEST_ASSERT_EQUAL_UINT32(sizeof(readThreeRegistersExpectedResponse), ModbusMockTXcount());
    for (int i = 0; i < sizeof(readThreeRegistersExpectedResponse); i++)
        TEST_ASSERT_EQUAL_UINT8(readThreeRegistersExpectedResponse[i], ModbusMockTX());

    uint8_t writeRegistersRequest[] = { 
        0x01, // server address 
        0x10, // function code
        0x00, 0x0F, // Start register (15)
        0x00, 0x03, // Nr registers 
        0x06,       // Nr byes
        0x01, 0x05, // Value for register 15
        0x02, 0x06, // Value for reguster 16
        0x03, 0x07, // Value for register 17
        0xBB, 0xEA  // CRC16
    };
    ModbusMockFeedRX(writeRegistersRequest, sizeof(writeRegistersRequest));

    TcposLoopN(20);

    uint8_t writeRegistersExpectedResponse[] = { 
        0x01, // server address 
        0x10, // function code
        0x00, 0x0F, // Start register (15)
        0x00, 0x03, // Nr registers 
        0xB0, 0x0B  // CRC16
    };
    TEST_ASSERT_EQUAL_UINT32(sizeof(writeRegistersExpectedResponse), ModbusMockTXcount());
    for (int i = 0; i < sizeof(writeRegistersExpectedResponse); i++)
        TEST_ASSERT_EQUAL_UINT8(writeRegistersExpectedResponse[i], ModbusMockTX());

    ModbusMockFeedRX(readThreeRegistersRequest, sizeof(readThreeRegistersRequest));

    TcposLoopN(30);

    uint8_t readThreeRegistersExpectedResponse2[] = { 
        0x01, // server address 
        0x03, // function code
        0x06, // nr bytes
        0x02, 0x06, // contenss reguster 16
        0x03, 0x07, // contenss register 17
        0x00, 0x12, // contenss register 18
        0x99, 0x1F  // CRC16
    };
    TEST_ASSERT_EQUAL_UINT32(sizeof(readThreeRegistersExpectedResponse2), ModbusMockTXcount());
    for (int i = 0; i < sizeof(readThreeRegistersExpectedResponse2); i++)
        TEST_ASSERT_EQUAL_UINT8(readThreeRegistersExpectedResponse2[i], ModbusMockTX());

}

