/**
 * @file ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ui_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t ui_target = UI_TARGET_ALL;

/*----------------
 * Translations
 *----------------*/

#ifndef LV_EDITOR_PREVIEW
    static const char * translation_languages[] = {"en", "de", NULL};
    static const char * translation_tags[] = {"dog", "cat", "house", NULL};
    static const char * translation_texts[] = {
        "This is a dog", "Das ist ein Hund", /* dog */
        "A curious little cat", "Eine neugierige kleine Katze", /* cat */
        "The house is cozy and warm", "Das Haus ist gemütlich und warm", /* house */
    };
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/



/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * dog_walk1 = NULL;
extern const void * dog_walk1_data;
const void * dog_walk1_r = NULL;
extern const void * dog_walk1_r_data;
const void * dog_walk2 = NULL;
extern const void * dog_walk2_data;
const void * dog_walk2_r = NULL;
extern const void * dog_walk2_r_data;
const void * background = NULL;
extern const void * background_data;
const void * cloud = NULL;
extern const void * cloud_data;
const void * balloon = NULL;
extern const void * balloon_data;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_init_gen(const char * asset_path)
{
    char buf[256];


    /*----------------
     * Fonts
     *----------------*/




    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if UI_CHECK_COMPILE_TARGET(UI_TARGET_ALL)
    if (ui_check_target(UI_TARGET_ALL)) {
        /* dog_walk1 */
        if (!dog_walk1) {
            dog_walk1 = &dog_walk1_data;
        }
        /* dog_walk1_r */
        if (!dog_walk1_r) {
            dog_walk1_r = &dog_walk1_r_data;
        }
        /* dog_walk2 */
        if (!dog_walk2) {
            dog_walk2 = &dog_walk2_data;
        }
        /* dog_walk2_r */
        if (!dog_walk2_r) {
            dog_walk2_r = &dog_walk2_r_data;
        }
        /* background */
        if (!background) {
            background = &background_data;
        }
        /* cloud */
        if (!cloud) {
            cloud = &cloud_data;
        }
        /* balloon */
        if (!balloon) {
            balloon = &balloon_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if LV_USE_XML
    /* Register widgets */


    /* Register fonts */

    /* Register subjects */

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "dog_walk1", dog_walk1);
    lv_xml_register_image(NULL, "dog_walk1_r", dog_walk1_r);
    lv_xml_register_image(NULL, "dog_walk2", dog_walk2);
    lv_xml_register_image(NULL, "dog_walk2_r", dog_walk2_r);
    lv_xml_register_image(NULL, "background", background);
    lv_xml_register_image(NULL, "cloud", cloud);
    lv_xml_register_image(NULL, "balloon", balloon);
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void ui_set_target(uint32_t target)
{
    ui_target = target;
}

uint32_t ui_get_target(void)
{
    return ui_target;
}

bool ui_check_target(uint32_t target)
{
    return (ui_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}