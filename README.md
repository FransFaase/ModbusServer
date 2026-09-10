# ModbusServer

This is an experiment to implement a dumb Modbus server for ESP32-S3 based
on [TinyCoPoOS](https://github.com/FransFaase/TinyCoPoOS). It will be
dumb in the sense that it just acts as small memory storage where you
can read and write values to registers.

The current implementation supports the following function codes:
- 3: Read registers
- 6: Write register
- 16: Write registers

Unit-test have been implemented and the code runs on a esp32-s3-devkitc-1
target, but the Modbus functionality has not been tested. The RGB led is
blinking and it prints some information, including the number of tasks that
have been executed. It does print that it received one byte and as no error
handling has been implemented yet, I fear that it will not work.

## Tasks and data queues

The implementation makes use of three tasks and two data queues to implement
the application specific functionality. This is maybe a bit of an overkill.

There is a task (identified with `taskid_modbus_read`) reading that polls the
receiving side of the serial device to see if data has arrived. As soon as this
happens, it queues the data into the `modbusReadDataQueue` data queue.

There is a task (identified with `taskid_modbus_server`) being activated when
data is written to the `modbusReadDataQueue` data queue and write responses
to the `modbusWriteDataQueue` data queue.

There is a task (identified with `taskid_modbus_write`) being activated when
data is written to the `modbusWriteDataQueue` data queue and, if in case
the transmit buffer is full, starts polling until is is available, and writes
the data to the transmit buffer.
