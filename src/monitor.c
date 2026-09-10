#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "tcpos.h"
#include "coroutine.h"
#include "modbus_stats.h"

bool receivedData = false;
bool transmittedData = false;

extern bool MonitorReceivedData(void)
{
    return receivedData;
}

extern bool MonitorTransmittedData(void)
{
    return transmittedData;
}

static void MonitorTaskStep(void)
{
    static uint32_t received_count_follow = 0;
    static uint32_t transmitted_count_follow = 0;

    COROUTINE_BEGIN

    for (;;)
    {
        uint32_t received_count = ModbusBytesReceived();
        if (received_count != received_count_follow)
        {
            receivedData = true;
            TimerStart(timerid_received_off, taskid_received_off, 200);
            received_count_follow = received_count;
        }        

        uint32_t transmitted_count = ModbusBytesTransmitted();
        if (transmitted_count != transmitted_count_follow)
        {
            transmittedData = true;
            TimerStart(timerid_transmitted_off, taskid_transmitted_off, 200);
            transmitted_count_follow = transmitted_count;
        }        

        TimerStart(timerid_monitor, taskid_monitor, 5);
        COROUTINE_YIELD
    }

    COROUTINE_END
}

static void DataReceivedOffTaskStep(void)
{
    receivedData = false;
}

static void DataTransmittedOffTaskStep(void)
{
    transmittedData = false;
}

extern void MonitorInit(void)
{
    TaskInit(taskid_monitor, MonitorTaskStep);
    QueueAdd(queueid_main_queue, taskid_monitor);

    TaskInit(taskid_received_off, DataReceivedOffTaskStep);
    TaskInit(taskid_transmitted_off, DataTransmittedOffTaskStep);
}

