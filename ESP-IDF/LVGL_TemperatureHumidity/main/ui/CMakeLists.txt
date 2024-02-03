SET(SOURCES screens/ui_MainScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_mangal.c)

add_library(ui ${SOURCES})
