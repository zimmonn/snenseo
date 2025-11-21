#include "snenseo_machine.h"

LOG_MODULE_REGISTER(snenseo_machine, CONFIG_SNENSEO_MACHINE_LOG_LEVEL);

#define SNENSEO_PIN_NODE DT_ALIAS(snenseo_pin)
#if !DT_NODE_HAS_STATUS_OKAY(SNENSEO_PIN_NODE)
  #error "pn532_irq devicetree alias is not defined, provide "
#endif
static const struct gpio_dt_spec snenseo_turn_on = GPIO_DT_SPEC_GET(SNENSEO_PIN_NODE, gpios);

int snenseo_init_machine(void) {
    LOG_INF("Starting Snenseo transform");
	if (!gpio_is_ready_dt(&snenseo_turn_on)) {
        LOG_ERR("LED device %s is not ready", snenseo_turn_on.port->name);
		return 0;
	}

	int ret = gpio_pin_configure_dt(&snenseo_turn_on, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
        LOG_ERR("Failed to configure %s pin %d, returned: %d", 
            snenseo_turn_on.port->name, snenseo_turn_on.pin, ret);
		return 0;
	}

		ret = gpio_pin_set_dt(&snenseo_turn_on, 1);
		if (ret < 0) {
            LOG_ERR("Couldn't set pin low");
			return 0;
		};

    LOG_INF("WOW I AM A SNENSEO NOW!");
    return 0;
}

static void machine_turn_on(struct k_work* _) {
    LOG_INF("MACIG about to appen. Yor machine will be ready soon!");
    if (gpio_pin_set_dt(&snenseo_turn_on, 0) < 0) {
        LOG_ERR("Today your coffee will be delayd");
    }
    k_msleep(CONFIG_SNENSEO_MACHINE_PULE_PERIOD);
    if (gpio_pin_set_dt(&snenseo_turn_on, 1) < 0) {
        LOG_ERR("Couldn't end pulse?");
    }
    return;
}

K_WORK_DEFINE(turnon, machine_turn_on);

void snenseo_machine_turn_on(void) {
    k_work_submit(&turnon);
}
