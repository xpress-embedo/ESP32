# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    openweathermap.c
    display_mng.c
    ui.c
    ui_helpers.c
    images/ui_img_delhi_png.c
    images/ui_img_jaipur_png.c
    images/ui_img_leh_png.c
    images/ui_img_shimla_png.c
    screens/ui_Delhi.c
    screens/ui_Jaipur.c
    screens/ui_Leh.c
    screens/ui_Shimla.c
    INCLUDE_DIRS        # optional, add here public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
