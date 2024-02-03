SET(SOURCES screens/ui_MainScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_mangal24.c
    fonts/ui_font_mangal18.c)

add_library(ui ${SOURCES})
