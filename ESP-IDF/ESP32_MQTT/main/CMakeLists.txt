# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    dht11.c
    display_mng.c
    ili9341.c
    xpt2046.c
    tft.c
    gui_mng.c
    gui_mng_cfg.c
    ui/ui.c
    ui/screens/ui_MainScreen.c
    INCLUDE_DIRS        # optional, add here public include directories
    "." "ui"
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
