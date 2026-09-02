# Pulls in LV_EDITOR_PROJECT_SOURCES, kept up to date by the LVGL UI Editor
include("${CMAKE_CURRENT_LIST_DIR}/ui/file_list_gen.cmake")

idf_component_register( SRCS 
                        "main.c"
                        "lcd.c"
                        "gui_mng.c"
                        "gui_mng_cfg.c"
                        ${LV_EDITOR_PROJECT_SOURCES}
                        PRIV_REQUIRES
                        spi_flash
                        esp_timer
                        esp_lcd
                        esp_lcd_touch
                        esp_lcd_touch_gt911
                        driver
                        lvgl
                        INCLUDE_DIRS "." "ui" "ui/screens" "ui/components" )

target_compile_definitions( ${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE )
