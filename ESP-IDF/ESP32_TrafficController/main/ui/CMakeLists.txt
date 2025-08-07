SET(SOURCES screens/ui_MainScreen.c
    screens/ui_Panel1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_school_logo_png.c)

add_library(ui ${SOURCES})
