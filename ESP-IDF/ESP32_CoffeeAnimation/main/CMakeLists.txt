# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c         # list the source files of this component
    display_mng.c
    gui_mng.c
    ili9341.c
    tft.c
    xpt2046.c
    ui/ui.c
    ui/ui_helpers.c
    ui/screens/ui_MainScreen.c
    ui/components/ui_comp_hook.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f01_png.c
    ui/images/ui_img_coffee_f02_png.c
    ui/images/ui_img_coffee_f03_png.c
    ui/images/ui_img_coffee_f04_png.c
    ui/images/ui_img_coffee_f05_png.c
    ui/images/ui_img_coffee_f06_png.c
    ui/images/ui_img_coffee_f07_png.c
    ui/images/ui_img_coffee_f08_png.c
    ui/images/ui_img_coffee_f09_png.c
    ui/images/ui_img_coffee_f10_png.c
    ui/images/ui_img_coffee_f11_png.c
    ui/images/ui_img_coffee_f12_png.c
    ui/images/ui_img_coffee_f13_png.c
    ui/images/ui_img_coffee_f14_png.c
    ui/images/ui_img_coffee_f15_png.c
    ui/images/ui_img_coffee_f16_png.c
    ui/images/ui_img_coffee_f17_png.c
    ui/images/ui_img_coffee_f18_png.c
    ui/images/ui_img_coffee_f19_png.c
    ui/images/ui_img_coffee_f20_png.c
    ui/images/ui_img_coffee_f21_png.c
    ui/images/ui_img_coffee_f22_png.c
    ui/images/ui_img_coffee_f23_png.c
    ui/images/ui_img_coffee_f24_png.c
    ui/images/ui_img_coffee_f25_png.c
    ui/images/ui_img_coffee_f26_png.c
    ui/images/ui_img_coffee_f27_png.c
    ui/images/ui_img_coffee_f28_png.c
    ui/images/ui_img_coffee_f29_png.c
    ui/images/ui_img_coffee_f30_png.c
    ui/images/ui_img_coffee_f31_png.c
    INCLUDE_DIRS "." "ui"    # optional, add here public include directories
    PRIV_INCLUDE_DIRS        # optional, add here private include directories
    REQUIRES                 # optional, list the public requirements (component names)
    PRIV_REQUIRES            # optional, list the private requirements
)
