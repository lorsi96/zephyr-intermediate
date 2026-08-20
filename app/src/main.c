/*
 * Lecture 3 - Homework Starter Code
 *
 * ================================================================
 * TASKS
 * ================================================================
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdbool.h>

LOG_MODULE_REGISTER(homework, LOG_LEVEL_DBG);

#define STACK_SIZE    1024
#define SENSOR_MS     100    /* sensor fires every 100ms */
#define BURST_MS      20
#define POLL_MS       10     /* polling consumer checks every 10ms */
#define EVENT_COUNT   10     /* total sensor events to produce */
#define FAST_EV_CNT   5
#define RESCH_MS      30

/* Statistics */
static int total_events;
static int total_processed;


static void sensor_handler(struct k_work *work) {
      ARG_UNUSED(work);
      total_processed++;
      LOG_INF("[HANDLER] processed event %d  tick=%u",
              total_processed, k_uptime_get_32());
}

K_WORK_DELAYABLE_DEFINE(sensor_work, sensor_handler);

/* I also migrated sensor_sim_fn to a self reschedule handler for more pratice. */
static void sensor_sim_handler(struct k_work *work) {
	struct k_work_delayable *dwork = k_work_delayable_from_work(work);
        total_events++;
        LOG_INF("[SENSOR] event %d  tick=%u", total_events, k_uptime_get_32());


        int ret = k_work_reschedule(&sensor_work, K_MSEC(RESCH_MS));
        if (ret < 0) { 
		LOG_ERR("submit failed: %d", ret); 
	}
	
	if (total_events >= EVENT_COUNT) {
		return;
	}

        /* Keep EVENT_COUNT events, but fire the first FAST_EV_CNT "faster". */
	ret = k_work_reschedule(dwork, K_MSEC(total_events < FAST_EV_CNT ? BURST_MS : SENSOR_MS));
        if (ret < 0) { 
		LOG_ERR("delayed work submit failed: %d", ret); 
	}

}

K_WORK_DELAYABLE_DEFINE(sensor_sim, sensor_sim_handler);

int main(void)
{
    k_work_schedule(&sensor_sim, K_MSEC(50));

    /* Wait long enough for all events to complete */
    k_msleep((EVENT_COUNT + 2) * SENSOR_MS + 500);

    return 0;
}

