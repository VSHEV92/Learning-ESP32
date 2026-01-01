- # Bluetooth Stack: NimBLE


------

## nimble_port_init()

1. Initialize **esp_bt_controller_config_t** structure using **BT_CONTROLLER_INIT_CONFIG_DEFAULT()** macro.
2. Call **esp_bt_controller_init()** with **esp_bt_controller_config_t** structure as a parameter. Initial configuration of  ESP bluetooth controller:
   - Check configuration parameters
   - **esp_mac_bb_pd_mem_init()** - Initialize backup memory for MAC and Baseband power up/down
   -  **esp_phy_modem_init()** - Initialize backup memory for Phy power up/down
   - **esp_bt_power_domain_on()** - Power on Bluetooth Wi-Fi power domain
   - **btdm_osi_funcs_register()** - set api and callback function of bluetooth controller to **osi_funcs_ro** structure
   - print BT controller compile version (**BLE_INIT** tag)
   - Call **periph_module_enable()** and **periph_module_reset()** to enable clock and reset bluetooth controller
   - Call **btdm_controller_init()** - ROM code, no info
   - Set **btdm_controller_status** to **ESP_BT_CONTROLLER_STATUS_INITED**
3. Call **esp_bt_controller_enable()** to enable bluetooth controller
   - Check **btdm_controller_status** and bluetooth controller mode
   - Call **esp_phy_enable()** to enable controller PHY (enable clock, calibration and initialization)
   - Call **btdm_controller_enable()** - ROM code, no info
   - Set **btdm_controller_status** to **ESP_BT_CONTROLLER_STATUS_ENABLED**
4.  Call **esp_nimble_init()** to Initialize the NimBLE host stack
   1. Call **npl_freertos_funcs_init()** to initialize NimBLE api (NPL - NimBLE porting layer) to FreeRTOS api by using **npl_funcs_ro** structure
   2. Call **npl_freertos_mempool_init()** to allocate memory for mutexes, semaphores and queues used by NimBLE. Use NPL to for using FreeRTOS mutexes, semaphores and queues.
   3. Call **esp_nimble_hci_init()** to initialize host-controller interface
      - Call **ble_buf_alloc()** to allocate memory for NimBLE HCI events and commands (nimble/transport)
      - Set VHCI callbacks to **vhci_host_cb** structure by using **esp_vhci_host_register_callback()**
      - Call **ble_transport_init()** to initialize NimBLE HCI events and commands (nimble/transport)
      - Create and give binary semaphore **vhci_send_sem**
   4. Call **ble_npl_eventq_init()** to initialize NimBLE event queue
   5.  Call **os_mempool_module_init()** and **os_msys_init()** to initialize the global memory pool
   6.  Call **ble_transport_ll_init()** to initilize HCI of bluetooth controller. This is NimBLE function used for different BT controllers. For ESP just set HCI receive callback to **ble_transport_host_recv_cb**
   7. Call **ble_transport_hs_init()** to initialize Host. Call **ble_hs_init()** to init all BLE Host data structures. At the end **ble_hs_init()** put to event queue **ble_hs_start()**. In **ble_hs_start()** function **ble_hs_sync()** is called, which at the end call **ble_hs_cfg.sync_cb** call back.