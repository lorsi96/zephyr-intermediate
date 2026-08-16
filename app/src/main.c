#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE 1024
#define PRIO 5
#define TEST_MAX_CNT  1000000UL
// #define ATOMIC_TEST

/* Shared counter. */
#ifdef ATOMIC_TEST
atomic_t shared_cnt = ATOMIC_INIT(0);
#else
static volatile uint32_t shared_cnt = 0;
#endif


/* Counter update strategies. */

// Atomic.
#ifdef ATOMIC_TEST
static inline void atomic_cnt_update() {
    atomic_inc(&shared_cnt);
}
#endif

// Mutexed.
K_MUTEX_DEFINE(homework_mutex);
static inline void safe_cnt_update() {
    k_mutex_lock(&homework_mutex, K_FOREVER);
    shared_cnt++;
    k_mutex_unlock(&homework_mutex);
}

// Unsafe.
static inline void unsafe_cnt_update() {
    shared_cnt++;
}

/* Program. */

K_SEM_DEFINE(thread_sem, 0, 2);
void thread_fn(void *p1, void *p2, void *p3) {
  for(uint32_t i=0; i<TEST_MAX_CNT; i++) {
	#ifdef ATOMIC_TEST
	atomic_cnt_update();
	#else
	safe_cnt_update();
	#endif
  }
  k_sem_give(&thread_sem);
}


K_THREAD_DEFINE(thread_a, STACK_SIZE, thread_fn,
                NULL, NULL, NULL, PRIO, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, thread_fn,
                NULL, NULL, NULL, PRIO, 0, 0);

int main(void)
{
  k_sem_take(&thread_sem, K_FOREVER);
  k_sem_take(&thread_sem, K_FOREVER);

  uint32_t cnt;
  #ifdef ATOMIC_TEST
  cnt = (uint32_t)atomic_get(&shared_cnt);
  #else
  cnt = shared_cnt; 
  #endif

  if (shared_cnt != (2*TEST_MAX_CNT)) {
    LOG_ERR("shared_cnt = %d", cnt);
  } else {
    LOG_INF("shared_cnt = %d", cnt);
  }

  return 0;
}

