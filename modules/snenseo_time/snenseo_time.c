#include "snenseo_time.h"

LOG_MODULE_REGISTER(snenseo_time, CONFIG_SNENSEO_TIME_LOG_LEVEL);

static const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(snenseo_rtc));


int snenseo_init_time(void) {
    if (!device_is_ready(rtc)) {
        LOG_ERR("Device is not ready");
        return;
    }
    LOG_INF("Device ready");
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

static void get_date_time()
{
        int ret = 0;
        struct rtc_time tm;

        ret = rtc_get_time(rtc, &tm);
        if (ret < 0) {
                LOG_ERR("Cannot read date time: %d", ret);
                return;
        }

        LOG_INF("RTC date and time: %04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900,
               tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        return;
}


static void snenseo_tread(void) {
    snenseo_init_time();
    LOG_INF("HURRAY");
    while (1) {
        k_msleep(CONFIG_SNENSEO_TIME_SLEEP_PERIOD);
        get_date_time();
    }
}

K_THREAD_DEFINE(snenseo_time_thread, CONFIG_SNENSEO_TIME_STACK_SIZE,
                snenseo_tread, NULL, NULL, NULL, CONFIG_SNENSEO_TIME_THREAD_PRIO,
                0, 0);
