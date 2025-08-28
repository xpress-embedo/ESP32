# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    dht11.c
    influxDB.c
    lcd.c
    gui_mng.c
    gui_mng_cfg.c
    ui/ui.c
    ui/ui_helpers.c
    ui/screens/ui_MainScreen.c
    ui/screens/ui_SensorScreen.c
    ui/components/ui_comp_hook.c
    ui/fonts/ui_font_Number.c
    ui/images/ui_img_2479933.c
    ui/images/ui_img_school_logo_png.c
    ui/images/ui_img_wifi_connected_png.c
    ui/images/ui_img_wifi_disconnected_png.c
    ui/images/ui_img_wifi_png.c
    INCLUDE_DIRS        # optional, add here public include directories
    "." "ui" "ui/screens"
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
