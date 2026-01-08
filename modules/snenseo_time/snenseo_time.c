#include "snenseo_time.h"

LOG_MODULE_REGISTER(snenseo_time, CONFIG_SNENSEO_TIME_LOG_LEVEL);

static const struct device *rtc = DEVICE_DT_GET(DT_ALIAS(snenseo_rtc));

static timer_task_t *tt_head = NULL;

static void copy_rtc_time_struct(struct snenseo_rtc_time *source,
                                 struct snenseo_rtc_time *dest) {
    if (source == NULL || dest == NULL) {
        return;
    }
    *dest = *source;   // copies the whole struct
}

void snenseo_set_timer(struct snenseo_rtc_time *trig_tim, uint8_t weekdays, struct k_work* work) {
    timer_task_t *curr = tt_head;
    timer_task_t *prev = NULL;
    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    curr = k_malloc(sizeof(timer_task_t));
    if (curr == NULL) {
        LOG_ERR("Couldn't allocate memory for new timer");
        return;
    }
    curr->next = NULL;
    copy_rtc_time_struct(trig_tim, &curr->trig_tim);
    curr->week_days = weekdays;
    curr->tt_work = work;
    if (prev == NULL) {
        tt_head = curr; // List was empty, new node becomes head
    } else {
        prev->next = curr; // Append new node to end of list
    }
}

/**
 * @brief this function just checks if the current date is lower than the date of the timer
 * 
 * @param curr_time time of the RTC
 * @param trig_time time to trigger action 
 * 
 * @return EXPIRED if current date is later than timer date 
 * @return WAITING if current date is earlier than timer date
 */
static time_state_t date_for_coffee(struct rtc_time *curr_time, struct snenseo_rtc_time *trig_tim) {
    // Current date is clearly past trigger
    if (curr_time->tm_year > trig_tim->tm_year ||
        (curr_time->tm_year == trig_tim->tm_year && curr_time->tm_mon > trig_tim->tm_mon) ||
        (curr_time->tm_year == trig_tim->tm_year && curr_time->tm_mon == trig_tim->tm_mon && 
         curr_time->tm_mday > trig_tim->tm_mday)) {
        return EXPIRED;
    }
    
    // Exact date match
    if (curr_time->tm_year == trig_tim->tm_year &&
        curr_time->tm_mon == trig_tim->tm_mon &&
        curr_time->tm_mday == trig_tim->tm_mday) {
        return ON_DATE;
    }
    
    // Current date is before trigger date
    return WAITING;
}

static void time_for_coffee(struct rtc_time *curr_time, timer_task_t *task) {
    // Extract trigger time (fix type cast - assuming snenseo_rtc_time has same layout)
    struct snenseo_rtc_time trig = task->trig_tim;
    /*
    LOG_ERR("CURR: %04d-%02d-%02d %02d:%02d:%02d", 
            curr_time->tm_year + 1900, curr_time->tm_mon + 1, curr_time->tm_mday,
            curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    LOG_ERR("TRIGG: %04d-%02d-%02d %02d:%02d:%02d", 
            trig.tm_year + 1900, trig.tm_mon + 1, trig.tm_mday,
            trig.tm_hour, trig.tm_min, trig.tm_sec);
    */
    // Check if hour and minute match exactly
    if (curr_time->tm_hour != trig.tm_hour || 
        curr_time->tm_min != trig.tm_min) {
        return;  // Not the right time yet
    }
    
    // Calculate seconds delta (trigger_sec could be 0-59)
    int delta_t = curr_time->tm_sec - trig.tm_sec;
    delta_t*=1000;
    if (delta_t < 0 || delta_t >= CONFIG_SNENSEO_TIME_SLEEP_PERIOD) {
        return;  // Too early or window expired
    }
    
    // Trigger the work within the time window
    k_work_submit(task->tt_work);
}

int snenseo_init_time(void) {
    if (!device_is_ready(rtc)) {
        LOG_ERR("Device is not ready");
        return -1;
    }
    LOG_INF("Device ready");
    return 0;
}

void snenseo_time_set_date_time(struct snenseo_rtc_time* tm) {
    int ret = 0;
    ret = rtc_set_time(rtc,(struct rtc_time*) tm);
    if (ret < 0) {
        LOG_ERR("Cannot write date time: %d", ret);
        return;
    }
    return;
}

static void get_date_time_update() {
    int ret = 0;
    struct rtc_time tm;

    ret = rtc_get_time(rtc, &tm);

    if (ret < 0) {
            LOG_ERR("Cannot read date time: %d", ret);
            return;
    }
    LOG_INF("RTC date and time: %04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday + 1, tm.tm_hour, tm.tm_min, tm.tm_sec);
        
    timer_task_t *curr = tt_head;
    timer_task_t *prev = NULL;
    while (curr != NULL) {
        LOG_INF("Timer date and time: %04d-%02d-%02d %02d:%02d:%02d", curr->trig_tim.tm_year + 1900,
               curr->trig_tim.tm_mon + 1, curr->trig_tim.tm_mday + 1, curr->trig_tim.tm_hour, 
               curr->trig_tim.tm_min, curr->trig_tim.tm_sec
            );
        timer_task_t *next = curr->next;  // Cache next BEFORE any changes

        if (curr->week_days == 0) {
            switch(date_for_coffee(&tm, &curr->trig_tim)) {
                case ON_DATE:
                    time_for_coffee(&tm, curr);
                    break;

                case EXPIRED:
                    LOG_ERR("Expired?");
                    if (prev != NULL) {
                        prev->next = next;    // Safe: next already cached
                    } else {
                        tt_head = next;       // Fixed: use next, not NULL
                    }
                    k_free(curr);
                    curr = next;              // Skip deleted node
                    continue;                 // Don't update prev/curr below
                default:
                    LOG_ERR("Default");
                    break;
            }
        }
        //else {} TODO
    
        prev = curr;
        curr = next;  // Use cached next
    }

    return;
}

static void demo_work_handler(struct k_work *_) {
    LOG_ERR("WOW IT WORKS?!?!");
}

K_WORK_DEFINE(demo_work, demo_work_handler);

static void snenseo_tread(void) {
    snenseo_init_time();
    LOG_INF("HURRAY");

    int ret = 0;
    struct rtc_time tm;
    ret = rtc_get_time(rtc, &tm);
    if (ret < 0) {
            LOG_ERR("Cannot read date time: %d", ret);
            return;
    }
    tm.tm_sec += 4;
    tm.tm_sec %= 60;
    LOG_INF("RTC TIMER date and time: %04d-%02d-%02d %02d:%02d:%02d", tm.tm_year,
           tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    snenseo_set_timer((struct snenseo_rtc_time *)&tm, 0, &demo_work);

    while (1) {
        k_msleep(CONFIG_SNENSEO_TIME_SLEEP_PERIOD);
        get_date_time_update();
    }
}

K_THREAD_DEFINE(snenseo_time_thread, CONFIG_SNENSEO_TIME_STACK_SIZE,
                snenseo_tread, NULL, NULL, NULL, CONFIG_SNENSEO_TIME_THREAD_PRIO,
                0, 0);
