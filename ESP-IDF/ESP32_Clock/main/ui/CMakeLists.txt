SET(SOURCES screens/ui_MainScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_bg_png.c
    images/ui_img_clock_sec_png.c
    images/ui_img_sec_dot_png.c)

add_library(ui ${SOURCES})
