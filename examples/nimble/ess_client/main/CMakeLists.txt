
idf_component_register(
    SRCS 
        "main.c" 
        "src/ess_client_gap.c"
        "src/ess_client_led_matrix.c"
    INCLUDE_DIRS    
        "include"
    PRIV_REQUIRES 
        bt
)
