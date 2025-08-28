SET(SOURCES screens/ui_SensorScreen.c
    screens/ui_MainScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_wifi_disconnected_png.c
    images/ui_img_school_logo_png.c
    images/ui_img_2479933.c
    images/ui_img_wifi_png.c
    images/ui_img_wifi_connected_png.c
    fonts/ui_font_Number.c)

add_library(ui ${SOURCES})
