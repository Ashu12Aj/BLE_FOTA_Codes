#include "sht40_sensor.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#define SLEEP_TIME_MS 1000

static const struct device *sht;

void sht40_sensor_init(void)
{
    sht = DEVICE_DT_GET_ANY(sensirion_sht4x);

    if (!device_is_ready(sht)) {
        printf("Device %s is not ready\n", sht ? sht->name : "NULL");
        // You may choose to handle error state here (e.g. return error code)
    }
}

void sht40_sensor_read_and_print(void)
{
    struct sensor_value temp, hum;

    if (sensor_sample_fetch(sht) == 0) {
        sensor_channel_get(sht, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        sensor_channel_get(sht, SENSOR_CHAN_HUMIDITY, &hum);

        printf("SHT40: Temperature: %.2f°C Humidity: %.2f%%RH\n",
            sensor_value_to_double(&temp), sensor_value_to_double(&hum));
    } else {
        printf("Error: Failed to fetch sensor data\n");
    }
    
    k_msleep(SLEEP_TIME_MS);
}
