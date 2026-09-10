#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "tcpos.h"

uint32_t tcpos_timer_tick = 0;

typedef uint32_t TimeTick;
TimeTick central_tick = 0;
#define MAX_TIME_TICK_MASK 0x7FFFFFE
#define INCREMENT_TIME_TICK central_tick = (central_tick & MAX_TIME_TICK_MASK) + 1;
#define TIMER_DONE(X) ((X) == central_tick)
#define TIMER_ON(T) (((central_tick + (T) - 1) & MAX_TIME_TICK_MASK) + 1)
#define TIMER_OFF 0


typedef struct
{
	void (*function)();
	TaskId next_task_id;
} Task;

Task tasks[taskid_none];

typedef struct
{
	TaskId first_id;
	TaskId last_id;
} Queue;

Queue queues[NR_QUEUES];

extern void TaskInit(TaskId taskId, void (*func)(void))
{
	tasks[taskId].next_task_id = taskid_none;
	tasks[taskId].function = func;
}

extern void QueueInit(QueueId queue_id)
{
	queues[queue_id].first_id = taskid_none;
	queues[queue_id].last_id = taskid_none;
}

extern void QueueAdd(QueueId queue_id, TaskId task_id)
{
	if (queues[queue_id].first_id == taskid_none)
		queues[queue_id].first_id = task_id;
	else
		tasks[queues[queue_id].last_id].next_task_id = task_id;
	queues[queue_id].last_id = task_id;
}

extern bool QueueEmpty(QueueId queue_id)
{
	return queues[queue_id].first_id == taskid_none;
}

extern TaskId QueuePop(QueueId queue_id)
{
	TaskId task_id = queues[queue_id].first_id;
	if (task_id != taskid_none)
	{
		queues[queue_id].first_id = tasks[task_id].next_task_id;
		tasks[task_id].next_task_id = taskid_none;
	}
	
	return task_id;
}

typedef struct
{
	TimeTick time;
	TaskId task_id;
} Timer;

Timer timers[NR_TIMERS];

extern void TimerStart(TimerId timerId, TaskId taskId, uint32_t time)
{
	timers[timerId].time = TIMER_ON(time);
	timers[timerId].task_id = taskId;
}

extern void TimerStop(TimerId timerId)
{
	timers[timerId].time = TIMER_OFF;
}


typedef struct
{
	QueueId queue_id;
	TaskId claimed_by_task_id;
} CriticalSection;

CriticalSection criticalSections[NR_CRITICAL_SECTIONS];

extern void CriticalSectionInit(CriticalSectionId critical_section_id, QueueId queue_id)
{
	QueueInit(queue_id);
	criticalSections[critical_section_id].queue_id = queue_id;
	criticalSections[critical_section_id].claimed_by_task_id = taskid_none;
}

extern bool CriticalSectionEnter(CriticalSectionId critical_section_id, TaskId task_id)
{
	if (criticalSections[critical_section_id].claimed_by_task_id != taskid_none)
	{
		QueueAdd(criticalSections[critical_section_id].queue_id, task_id);
		return false;
	}
	criticalSections[critical_section_id].claimed_by_task_id = task_id;
	return true;
}
// Caller needs to exit the task when this function returns false

extern void CriticalSectionLeave(CriticalSectionId critical_section_id)
{
	TaskId next_task_id = QueuePop(criticalSections[critical_section_id].queue_id);
	criticalSections[critical_section_id].claimed_by_task_id = next_task_id;
	if (next_task_id != taskid_none)
		QueueAdd(queueid_main_queue, next_task_id);
}

static void TickTimerTaskStep(void)
{
	static uint32_t tcpos_timer_tick_follower = 0;
	if (tcpos_timer_tick_follower != tcpos_timer_tick)
	{
		tcpos_timer_tick_follower++;

		INCREMENT_TIME_TICK;
		for (int i = 0; i < NR_TIMERS; i++)
			if (TIMER_DONE(timers[i].time))
			{
				timers[i].time = TIMER_OFF;
				QueueAdd(queueid_main_queue, timers[i].task_id);
			}
	}
	QueueAdd(queueid_main_queue, taskid_tick_timer);
}

extern void TcposInit(void)
{
	QueueInit(queueid_main_queue);
	TaskInit(taskid_tick_timer, TickTimerTaskStep);
	QueueAdd(queueid_main_queue, taskid_tick_timer);
}

static uint32_t tasksExecuted = 0;

extern void TcposLoop(void* ptr)
{
	for (;;)
	{
		TaskId task_id = QueuePop(queueid_main_queue);
		if (task_id == taskid_none)
			break;
		
		tasks[task_id].function();
		tasksExecuted++;
	}
}

#ifdef UNITY
extern uint32_t TcposLoopN(uint32_t n)
{
	for (int i = 0; i < n; i++)
	{
		TaskId task_id = QueuePop(queueid_main_queue);
		//printf("step %d: task %d ", i, task_id);
		if (task_id == taskid_none)
		{
			//printf(" exit\n");
			return i;
		}
		
		if (tasks[task_id].function != 0)
			tasks[task_id].function();
		//else
		//	printf(" no function");
		//printf("\n");
		tasksExecuted++;
	}
	return n;
}
#endif

extern uint32_t TcposTasksExecuted(void)
{
	return tasksExecuted;
}

