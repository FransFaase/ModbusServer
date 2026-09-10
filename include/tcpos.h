// Presumes that stdint.h and stdbool.h are already included

typedef enum
{
    taskid_tick_timer,
    // Start user task ids
    taskid_modbus_read,
    taskid_modbus_write,
    taskid_modbus_server,
    taskid_monitor,
    taskid_received_off,
    taskid_transmitted_off,
    // End user task ids
    taskid_none
}  TaskId;

typedef enum
{
    // Start user timer ids
    timerid_monitor,
    timerid_received_off,
    timerid_transmitted_off,
    // End user timer ids
    NR_TIMERS    
} TimerId;

typedef enum
{
    queueid_main_queue,
    // Start user queue ids
    // End user queue ids
#ifdef UNITY
    queueid_critical_section_test_queue,
#endif
    NR_QUEUES
} QueueId;

typedef enum
{
    // Start user critical section ids
    // End user critical section ids
//#ifdef UNITY
    critical_section_test,
//#endif
    NR_CRITICAL_SECTIONS,
} CriticalSectionId;

extern uint32_t tcpos_timer_tick; // To be incremented on timer interrupt

extern void TaskInit(TaskId taskId, void (*func)(void));

extern void QueueInit(QueueId queue_id);
extern void QueueAdd(QueueId queue_id, TaskId task_id);
extern bool QueueEmpty(QueueId queue_id);
extern TaskId QueuePop(QueueId queue_id);

extern void TimerStart(TimerId timerId, TaskId taskId, uint32_t time);
extern void TimerStop(TimerId timerId);

extern void CriticalSectionInit(CriticalSectionId critical_section_id, QueueId queue_id);
extern bool CriticalSectionEnter(CriticalSectionId critical_section_id, TaskId task_id);
extern void CriticalSectionLeave(CriticalSectionId critical_section_id);

extern void TcposInit(void);
extern void TcposLoop(void* ptr);
#ifdef UNITY
extern uint32_t TcposLoopN(uint32_t n);
#endif

extern uint32_t TcposTasksExecuted(void);

