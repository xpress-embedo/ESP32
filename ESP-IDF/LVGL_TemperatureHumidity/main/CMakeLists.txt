# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    display_mng.c
    gui_mng.c
    thingspeak.c
    ili9341.c
    tft.c
    xpt2046.c
    dht11.c
    ui/ui.c
    ui/ui_helpers.c
    ui/screens/ui_MainScreen.c
    ui/components/ui_comp_hook.c
    INCLUDE_DIRS "." "ui"       # optional, add here public include directories
    PRIV_INCLUDE_DIRS   		# optional, add here private include directories
    REQUIRES            		# optional, list the public requirements (component names)
    PRIV_REQUIRES               # optional, list the private requirements
)
