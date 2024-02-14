# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    display_mng.c
    gui_mng.c
    ili9341.c
    tft.c
    xpt2046.c
    sntp_time_sync.c
    ui/ui.c
    ui/screens/ui_MainScreen.c
    ui/screens/ui_ClockScreen.c
    ui/images/ui_img_bg_png.c
    ui/images/ui_img_clock_sec_png.c
    ui/images/ui_img_clock_min_png.c
    ui/images/ui_img_clock_hour_png.c
    ui/images/ui_img_sec_dot_png.c
    INCLUDE_DIRS "." "ui"        # optional, add here public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
