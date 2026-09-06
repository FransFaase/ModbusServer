#include "modbus.h"

uint8_t dataRX[16];
uint32_t dataRXcount = 0;
uint32_t TXhasRoom = 0;
uint8_t dataTX[100];
uint32_t dataTXcount = 0;

extern uint32_t ModbusRXcount(void)
{
    return dataRXcount;
}

extern uint8_t ModbusRX(void)
{
    uint8_t r = dataRX[0];
    dataRXcount--;
    for (int i = 0; i < dataRXcount; i++)
        dataRX[i] = dataRX[i + 1];
    return r;
}

extern uint32_t ModbusTXRoom(void)
{
    return TXhasRoom;
}

extern void ModbusTX(uint8_t byte)
{
    dataTX[dataTXcount++] = byte;
}


extern void ModbusMockFeedRX(uint8_t *data, uint32_t nr)
{
    for (int i = 0; i < nr; i++)
        dataRX[dataRXcount++] = data[i];
}

extern void ModbusMockTXSetRoom(uint32_t room) { TXhasRoom = room; }
extern uint32_t ModbusMockTXcount() { return dataTXcount; }

extern uint8_t ModbusMockTX()
{
    uint8_t r = dataTX[0];
    dataTXcount--;
    for (int i = 0; i < dataTXcount; i++)
        dataTX[i] = dataTX[i + 1];
    return r;
}

