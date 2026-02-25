// fota_ble_ashu.c
#include "fota_ble_ashu.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/settings/settings.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/reboot.h>

#include <zephyr/dfu/mcuboot.h>

void check_and_wipe(void) {
    if (!boot_is_img_confirmed()) {
        printk("New DFU Update Detected (Image Unconfirmed)! Performing Factory Reset (Erase All Data)...\n");
        
        const struct flash_area *fa;
        int rc = flash_area_open(FIXED_PARTITION_ID(storage_partition), &fa);
        if (rc == 0) {
            rc = flash_area_erase(fa, 0, fa->fa_size);
            flash_area_close(fa);
            if (rc == 0) {
                printk("Storage Partition Erased. Confirming Image and Rebooting...\n");
                // Confirm the image so we don't wipe again on next boot
                boot_write_img_confirmed();
                k_sleep(K_SECONDS(1));
                sys_reboot(SYS_REBOOT_COLD);
            } else {
                printk("Flash erase failed (err %d)\n", rc);
                 // If erase fails, we still confirm to avoid boot loop
                 boot_write_img_confirmed();
            }
        } else {
             printk("Failed to open storage partition (err %d). Confirming image anyway.\n", rc);
             boot_write_img_confirmed();
        }
    } else {
        printk("Normal Boot (Image Confirmed). No wipe needed.\n");
    }
}

#define CON_STATUS_LED DK_LED2
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed (err %u)\n", err);
        return;
    }
    printk("Connected\n");
    dk_set_led_on(CON_STATUS_LED);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected (reason %u)\n", reason);
    dk_set_led_off(CON_STATUS_LED);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

void fota_ashu_start(void)
{
    int err;

    printk("Starting Bluetooth FOTA example with LED indication\n");

    err = dk_leds_init();
    if (err) {
        printk("LED init failed (err %d)\n", err);
        return;
    }

    err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
        check_and_wipe();
    }

    /* Connectable advertising, using explicit parameters */
    const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
        BT_LE_ADV_OPT_CONN,
        BT_GAP_ADV_FAST_INT_MIN_2,
        BT_GAP_ADV_FAST_INT_MAX_2,
        NULL);

    err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    printk("Advertising successfully started\n");
    printk("BLE FOTA Ready by Ashutosh.......................\n");
}
