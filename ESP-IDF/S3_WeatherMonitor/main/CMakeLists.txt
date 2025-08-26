# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    dht11.c
    lcd.c
    gui_mng.c
    gui_mng_cfg.c
    ui/ui.c
    ui/ui_helpers.c
    ui/screens/ui_MainScreen.c
    INCLUDE_DIRS        # optional, add here public include directories
    "." "ui"
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
