extern void CRC16Start(void);
extern void CRC16Add(uint32_t value);
extern bool CRC16Check(const uint8_t *data, uint32_t length);
extern void CRC16Calculate(uint8_t *data, uint32_t length);
extern uint32_t CRC16Value(void);
