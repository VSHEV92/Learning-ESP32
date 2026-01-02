#include "host/ble_hs.h"

extern int adc_raw_data;
extern uint16_t adc_chr_val_handle;

// Automation IO and ADC services
static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
static const ble_uuid128_t adc_chr_uuid = BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 
                                                           0xde, 0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

// adc characteristic access handler
int adc_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        // Read characteristic event
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, &adc_raw_data, sizeof(adc_raw_data));
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
            /* ADC characteristic */
            {
                .uuid = &adc_chr_uuid.u,
                .access_cb = adc_chr_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,
                .val_handle = &adc_chr_val_handle
            },
            /* No more characteristics */
            {0}
        },
    },
    /* No more services. */
    {0},
};
