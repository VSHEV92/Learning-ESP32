
idf_component_register(
    SRCS 
        "main.c" 
        "src/ess_server_gap.c"
        "src/ess_server_gatt.c"
        "src/ess_server_temperature_sensor.c"
    INCLUDE_DIRS    
        "include"
    PRIV_REQUIRES 
        bt
        esp_driver_tsens
)
