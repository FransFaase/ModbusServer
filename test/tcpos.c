#include <unity.h>
#include <stdio.h>
#include "../src/tcpos.c"


void test1TaskStep(void)
{

}

void test2TaskStep(void)
{

}

extern void TestTinyCoPoOS(void)
{
    QueueInit(queueid_main_queue);

    // Empty queue
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_none, QueuePop(queueid_main_queue));
    
    // Push one task
    TaskInit(taskid_modbus_read, test1TaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_read);
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_read, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_none, QueuePop(queueid_main_queue));

    // Push three tasks
    TaskInit(taskid_modbus_write, test2TaskStep);
    TaskInit(taskid_modbus_slave, test2TaskStep);
    QueueAdd(queueid_main_queue, taskid_modbus_read);
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    QueueAdd(queueid_main_queue, taskid_modbus_write);
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_read, QueuePop(queueid_main_queue));
    QueueAdd(queueid_main_queue, taskid_modbus_slave);
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_write, QueuePop(queueid_main_queue));
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_slave, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_none, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_none, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));

    // Test critical sections with one task
    CriticalSectionInit(critical_section_test, queueid_critical_section_test_queue);
    TEST_ASSERT_TRUE(CriticalSectionEnter(critical_section_test, taskid_modbus_read));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_critical_section_test_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    CriticalSectionLeave(critical_section_test);
    TEST_ASSERT_TRUE(QueueEmpty(queueid_critical_section_test_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));

    // Test critical sections with two tasks
    TEST_ASSERT_TRUE(CriticalSectionEnter(critical_section_test, taskid_modbus_read));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_critical_section_test_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_FALSE(CriticalSectionEnter(critical_section_test, taskid_modbus_write));
    TEST_ASSERT_FALSE(QueueEmpty(queueid_critical_section_test_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    CriticalSectionLeave(critical_section_test);
    TEST_ASSERT_TRUE(QueueEmpty(queueid_critical_section_test_queue));
    TEST_ASSERT_FALSE(QueueEmpty(queueid_main_queue));
    TEST_ASSERT_EQUAL_UINT32(taskid_modbus_write, QueuePop(queueid_main_queue));
    TEST_ASSERT_TRUE(QueueEmpty(queueid_main_queue));
    CriticalSectionLeave(critical_section_test);
    TEST_ASSERT_TRUE(QueueEmpty(queueid_critical_section_test_queue));
}

