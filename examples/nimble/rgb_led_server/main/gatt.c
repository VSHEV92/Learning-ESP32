#include "host/ble_hs.h"
#include "led_strip.h"

extern led_strip_handle_t led_strip;

static uint8_t rgb_led_brightness = 0xff;
static uint8_t rgb_led_color[] = {1, 0, 0};

static uint16_t led_color_chr_val_handle;
static uint16_t led_brightness_chr_val_handle;

// Automation IO and ADC services
static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);

static const ble_uuid128_t led_color_chr_uuid = BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 
                                                                 0xde, 0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

static const ble_uuid128_t led_brightness_chr_uuid = BLE_UUID128_INIT(0x24, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 
                                                                 0xde, 0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

// led color characteristic access handler
int led_color_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            uint8_t temp = rgb_led_color[0];
            rgb_led_color[0] = rgb_led_color[2];
            rgb_led_color[2] = rgb_led_color[1];
            rgb_led_color[1] = temp;

            led_strip_set_pixel (
                led_strip, 
                0, 
                rgb_led_color[0]*rgb_led_brightness, 
                rgb_led_color[1]*rgb_led_brightness, 
                rgb_led_color[2]*rgb_led_brightness
            );
            led_strip_refresh(led_strip);
            
            return 0;
    }
    return 0;
}

// led brightness characteristic access handler
int led_brightness_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (ctxt->om->om_len == 1) {
                rgb_led_brightness = ctxt->om->om_data[0];

                led_strip_set_pixel (
                    led_strip, 
                    0, 
                    rgb_led_color[0]*rgb_led_brightness, 
                    rgb_led_color[1]*rgb_led_brightness, 
                    rgb_led_color[2]*rgb_led_brightness
                );
                led_strip_refresh(led_strip);
            
            }
            return 0;
    }
    return 0;
}


// GATT services table
const struct ble_gatt_svc_def gatt_svcs[] = {
    /* Automation IO service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &auto_io_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            /* Led Color characteristic */
            {
                .uuid = &led_color_chr_uuid.u,
                .access_cb = led_color_chr_access,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &led_color_chr_val_handle
            },
            /* Led Brightness characteristic */
            {
                .uuid = &led_brightness_chr_uuid.u,
                .access_cb = led_brightness_chr_access,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &led_brightness_chr_val_handle
            },
            /* No more characteristics */
            {0}
        },
    },
    /* No more services. */
    {0},
};
