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

lv_font_t * title_large;
extern lv_font_t title_large_data;
lv_font_t * body_medium;
extern lv_font_t body_medium_data;
lv_font_t * body_normal;
extern lv_font_t body_normal_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * thermometer_img = NULL;
extern const void * thermometer_img_data;
const void * humidity_img = NULL;
extern const void * humidity_img_data;
const void * pressure_img = NULL;
extern const void * pressure_img_data;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

lv_subject_t red_val;
lv_subject_t green_val;
lv_subject_t blue_val;
lv_subject_t temp_str;
lv_subject_t temp_status_str;
lv_subject_t hum_str;
lv_subject_t hum_status_str;
lv_subject_t press_str;
lv_subject_t press_status_str;

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

    /* Targets: any */

    #if UI_CHECK_COMPILE_TARGET(UI_TARGET_ALL)
    if (ui_check_target(UI_TARGET_ALL)) {
        if (!title_large) {
            /* title_large */
            /* get font 'title_large' from a C array */
            title_large = &title_large_data;

        }
        if (!body_medium) {
            /* body_medium */
            /* get font 'body_medium' from a C array */
            body_medium = &body_medium_data;

        }
        if (!body_normal) {
            /* body_normal */
            /* get font 'body_normal' from a C array */
            body_normal = &body_normal_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if UI_CHECK_COMPILE_TARGET(UI_TARGET_ALL)
    if (ui_check_target(UI_TARGET_ALL)) {
        /* thermometer_img */
        if (!thermometer_img) {
            thermometer_img = &thermometer_img_data;
        }
        /* humidity_img */
        if (!humidity_img) {
            humidity_img = &humidity_img_data;
        }
        /* pressure_img */
        if (!pressure_img) {
            pressure_img = &pressure_img_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&red_val, 0);
    lv_subject_init_int(&green_val, 0);
    lv_subject_init_int(&blue_val, 0);
    static char temp_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char temp_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&temp_str,
                           temp_str_buf,
                           temp_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "-- °C"
                          );
    static char temp_status_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char temp_status_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&temp_status_str,
                           temp_status_str_buf,
                           temp_status_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "--"
                          );
    static char hum_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char hum_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hum_str,
                           hum_str_buf,
                           hum_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "-- %"
                          );
    static char hum_status_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char hum_status_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hum_status_str,
                           hum_status_str_buf,
                           hum_status_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "--"
                          );
    static char press_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char press_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&press_str,
                           press_str_buf,
                           press_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "-- hPa"
                          );
    static char press_status_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char press_status_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&press_status_str,
                           press_status_str_buf,
                           press_status_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "--"
                          );

    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&title_large, "title_large");
    check_font(&body_medium, "body_medium");
    check_font(&body_normal, "body_normal");

    /* Register fonts */
    lv_xml_register_font(NULL, "title_large", title_large);
    lv_xml_register_font(NULL, "body_medium", body_medium);
    lv_xml_register_font(NULL, "body_normal", body_normal);

    /* Register subjects */
    lv_xml_register_subject(NULL, "red_val", &red_val);
    lv_xml_register_subject(NULL, "green_val", &green_val);
    lv_xml_register_subject(NULL, "blue_val", &blue_val);
    lv_xml_register_subject(NULL, "temp_str", &temp_str);
    lv_xml_register_subject(NULL, "temp_status_str", &temp_status_str);
    lv_xml_register_subject(NULL, "hum_str", &hum_str);
    lv_xml_register_subject(NULL, "hum_status_str", &hum_status_str);
    lv_xml_register_subject(NULL, "press_str", &press_str);
    lv_xml_register_subject(NULL, "press_status_str", &press_status_str);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "thermometer_img", thermometer_img);
    lv_xml_register_image(NULL, "humidity_img", humidity_img);
    lv_xml_register_image(NULL, "pressure_img", pressure_img);
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