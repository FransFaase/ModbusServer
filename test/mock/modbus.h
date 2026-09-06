
extern uint32_t ModbusRXcount(void);
extern uint8_t ModbusRX(void);
extern uint32_t ModbusTXRoom(void);
extern void ModbusTX(uint8_t byte);

extern void ModbusMockFeedRX(uint8_t *data, uint32_t nr);
extern void ModbusMockTXSetRoom(uint32_t room);
extern uint32_t ModbusMockTXcount();
extern uint8_t ModbusMockTX(void);

