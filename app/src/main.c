#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE 1024

#define PRIO_A 7
#define PRIO_B 5
#define PRIO_C 3

#define PRIO_COOP    -1
#define ITERS_COOP    5
#define SLEEP_COOP_US 100000


void thread_fn(const char *msg, uint32_t sleep_ms)
{
    for(;;) {
        LOG_INF("%s", msg);
        k_msleep(sleep_ms);
    }
}

void _thread_coop_fn(void *p1, void *p2, void *p3) {
    for(uint8_t i=0;i<ITERS_COOP;i++) {
        LOG_INF("T_COOP Running (%d/%d)", i+1, ITERS_COOP);
	k_busy_wait(SLEEP_COOP_US);
    }
}


K_THREAD_DEFINE(thread_high_fn, STACK_SIZE, thread_fn,
                /*msg=*/"T_HIGH_running",
		/*sleep_ms=*/100, NULL, PRIO_C, 0, 0);
K_THREAD_DEFINE(thread_mid_fn, STACK_SIZE, thread_fn,
                /*msg=*/"T_MID_running",
		/*sleep_ms=*/200, NULL, PRIO_B, 0, 0);
K_THREAD_DEFINE(thread_low_fn, STACK_SIZE, thread_fn,
                /*msg=*/"T_LOW_running",
		/*sleep_ms=*/300, NULL, PRIO_A, 0, 0);
K_THREAD_DEFINE(thread_coop_fn, STACK_SIZE, _thread_coop_fn,
                NULL, NULL, NULL, PRIO_COOP, 0, 0);  

int main(void)
{
    return 0;
}

