#include "ess_server_gatt.h"

static const ble_uuid16_t ess_uuid = BLE_UUID16_INIT(ESS_SERVER_GATT_UUID_ESS);

static uint16_t ess_chr_temp_celsius_handle;
static const ble_uuid16_t ess_chr_temp_celsius_uuid = BLE_UUID16_INIT(ESS_SERVER_GATT_UUID_TEMP_CELSIUS);
int ess_chr_temp_celsius_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const ble_uuid16_t ess_dsc_es_measuarement_uuid = BLE_UUID16_INIT(ESS_SERVER_GATT_UUID_ES_MESUAREMENT);
int ess_dsc_es_measuarement_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const ble_uuid16_t ess_dsc_es_trigger_setting_uuid = BLE_UUID16_INIT(ESS_SERVER_GATT_UUID_ES_TRIGGER_SETTING);
int ess_dsc_es_trigger_setting_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const ble_uuid16_t ess_dsc_user_description_uuid = BLE_UUID16_INIT(ESS_SERVER_GATT_UUID_USER_DESCRIPTION);
int ess_dsc_user_description_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);


// GATT ervices table
const struct ble_gatt_svc_def gatt_svcs[] = {
    /* Environmental Sensing Service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &ess_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            /* ESS Characteristic Temperature Celsius */
            {
                .uuid = &ess_chr_temp_celsius_uuid.u,
                .access_cb = ess_chr_temp_celsius_access_cb,
                .flags = BLE_GATT_CHR_F_READ,
                .val_handle = &ess_chr_temp_celsius_handle,
                
                .descriptors = (struct ble_gatt_dsc_def[]) {
                    /* ES Measurement */
                    {
                        .uuid = &ess_dsc_es_measuarement_uuid.u,
                        .access_cb = ess_dsc_es_measuarement_access_cb,
                        .att_flags = BLE_GATT_CHR_F_READ,
                    },
                    /* ES Trigger Settings */
                    {
                        .uuid = &ess_dsc_es_trigger_setting_uuid.u,
                        .access_cb = ess_dsc_es_trigger_setting_access_cb,
                        .att_flags = BLE_GATT_CHR_F_READ,
                    },
                    /* User Descrioption */
                    {
                        .uuid = &ess_dsc_user_description_uuid.u,
                        .access_cb = ess_dsc_user_description_access_cb,
                        .att_flags = BLE_GATT_CHR_F_READ,
                    },
                    /* No more descriptors */
                    {0}
                }
            },
            /* No more characteristics */
            {0}
        },
    },
    /* No more services. */
    {0},
};


int ess_chr_temp_celsius_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    float temp_float;
    char temp_str[5];

    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            temp_float = ess_server_temperature_sensor_get_temp();
            ESP_LOGI("", "Temperature sensor value read %f", temp_float);
            snprintf(temp_str, sizeof(temp_str), "%4.1f", temp_float);
            os_mbuf_append(ctxt->om, temp_str, sizeof(temp_str));
            return 0;
    }
    return 0;
}

int ess_dsc_es_measuarement_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    uint8_t measurement[] = {
        0x00, 0x00,         /* Flags, Reserved */
        0x01,               /* Sampling function, Instantaneous */
        0x00, 0x00, 0x00,   /* Measurement Period, Not in use */
        0x00, 0x00, 0x00,   /* Internal Update Interval, Not in use */
        0x1B,               /* Application, Inside */
        0xFF,               /* Measurement Uncertainty, Information not available */

    };
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, measurement, sizeof(measurement));
            return 0;
    }
    return 0;
    return 0;
}

int ess_dsc_es_trigger_setting_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    uint8_t trigger_settings = 0x00; // Trigger inactive
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, &trigger_settings, sizeof(trigger_settings));
            return 0;
    }
    return 0;
}

int ess_dsc_user_description_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    uint8_t description[] = "esp32 temperature sensor data in Celsius";
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, description, sizeof(description));
            return 0;
    }
    return 0;
}

// Initialize GATT service
void ess_server_gatt_init(void) {
    // GATT service initialization
    ble_svc_gatt_init();

    // add ESS to GATT
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);
}
