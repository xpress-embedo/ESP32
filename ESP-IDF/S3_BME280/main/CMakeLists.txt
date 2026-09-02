idf_component_register( SRCS 
                        "main.c"
                        "lcd.c"
                        "gui_mng.c"
                        "gui_mng_cfg.c"
                        PRIV_REQUIRES
                        spi_flash
                        esp_timer
                        esp_lcd
                        esp_lcd_touch
                        esp_lcd_touch_gt911
                        driver
                        lvgl
                        INCLUDE_DIRS "." "ui" )

target_compile_definitions( ${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE )
