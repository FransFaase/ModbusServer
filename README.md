# ModbusClient

This is an experiment to implement a dumb Modbus client for ESP32-S3 based
on [TinyCoPoOS](https://github.com/FransFaase/TinyCoPoOS). It will be
dumb in the sense that it just acts as small memory storage where you
can read and write values to registers.

The current implementation supports the following function codes:
- 3: Read registers
- 6: Write register
- 16: Write registers

Unit-test have been implemented and the code compiles for the esp32-s3-devkitc-1
target, but it has not been tested on this.

