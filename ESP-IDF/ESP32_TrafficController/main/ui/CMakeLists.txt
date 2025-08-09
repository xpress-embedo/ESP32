SET(SOURCES screens/ui_MainScreen.c
    screens/ui_Panel1.c
    screens/ui_Panel2.c
    screens/ui_Panel3.c
    screens/ui_Panel4.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_school_logo_png.c
    images/ui_img_1402433841.c
    fonts/ui_font_DsDigital72.c)

add_library(ui ${SOURCES})
