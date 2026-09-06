#include <unity.h>

extern void TestTinyCoPoOS(void);
extern void TestDataQueue(void);
extern void TestModbus(void);
extern void TestCRC16(void);
extern void TestModbusSlave(void);


void setUp(void)
{
}

void tearDown(void)
{
}

int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestTinyCoPoOS);
    RUN_TEST(TestDataQueue);
    RUN_TEST(TestModbus);
    RUN_TEST(TestCRC16);
    RUN_TEST(TestModbusSlave);
    UNITY_END();
}
