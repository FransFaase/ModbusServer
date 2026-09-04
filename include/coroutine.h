#define COROUTINE_BEGIN \
    static uint32_t coroutine_state = 0; \
    switch (coroutine_state) { \
        case 0:
#define COROUTINE_YIELD coroutine_state = __LINE__; return; case __LINE__:
#define COROUTINE_END  }
