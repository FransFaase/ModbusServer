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

## TinyCoPoOS running on Core 1

The TinyCoPoOS runs on Core 1 (the second core) while ESP-IDF (based on
FreeRTOS) runs on Core 0. The Idle Watch Dog Timer on Core 1 has been
disabled because the function `TcposLoop` runs in an infinite loop and
does not call any ESP-IDF function and for that reason the Idle Task is
never executed. 

## Tasks and data queues

The implementation makes use of six tasks and two data queues to implement
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

There are three tasks (identified with `taskid_monitor`, `taskid_received_off`,
and `taskid_transmitted_off`) for a monitor that sets two Booleans depending
on whether data has been received or transmitted, stay on for a short period
and are used for enabling the RGB led from Core 0.