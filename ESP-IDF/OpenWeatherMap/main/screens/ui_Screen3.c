// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: OpenWeatherMap

#include "../ui.h"

void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_jaipurImage = lv_img_create(ui_Screen3);
    lv_img_set_src(ui_jaipurImage, &ui_img_jaipur_png);
    lv_obj_set_width(ui_jaipurImage, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_jaipurImage, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_jaipurImage, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_jaipurImage, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_jaipurImage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Temperature2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_Temperature2, 20);
    lv_obj_set_width(ui_Temperature2, lv_pct(45));
    lv_obj_set_x(ui_Temperature2, -70);
    lv_obj_set_y(ui_Temperature2, -100);
    lv_obj_set_align(ui_Temperature2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Temperature2, "Temperature: ");
    lv_obj_set_style_text_color(ui_Temperature2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Temperature2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Temperature2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Temperature2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Pressure2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_Pressure2, 20);
    lv_obj_set_width(ui_Pressure2, lv_pct(45));
    lv_obj_set_x(ui_Pressure2, -70);
    lv_obj_set_y(ui_Pressure2, -80);
    lv_obj_set_align(ui_Pressure2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Pressure2, "Pressure:");
    lv_obj_set_style_text_color(ui_Pressure2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Pressure2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Pressure2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Pressure2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempValue2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_tempValue2, 20);
    lv_obj_set_width(ui_tempValue2, lv_pct(45));
    lv_obj_set_x(ui_tempValue2, 80);
    lv_obj_set_y(ui_tempValue2, -100);
    lv_obj_set_align(ui_tempValue2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tempValue2, "20 C");
    lv_obj_set_style_text_color(ui_tempValue2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempValue2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_tempValue2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tempValue2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pressureValue2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_pressureValue2, 20);
    lv_obj_set_width(ui_pressureValue2, lv_pct(45));
    lv_obj_set_x(ui_pressureValue2, 80);
    lv_obj_set_y(ui_pressureValue2, -80);
    lv_obj_set_align(ui_pressureValue2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_pressureValue2, "1000 bar");
    lv_obj_set_style_text_color(ui_pressureValue2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_pressureValue2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_pressureValue2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_pressureValue2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Humidity2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_Humidity2, 20);
    lv_obj_set_width(ui_Humidity2, lv_pct(45));
    lv_obj_set_x(ui_Humidity2, -70);
    lv_obj_set_y(ui_Humidity2, -60);
    lv_obj_set_align(ui_Humidity2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Humidity2, "Humidity:");
    lv_obj_set_style_text_color(ui_Humidity2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Humidity2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Humidity2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Humidity2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_humidityValue2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_humidityValue2, 20);
    lv_obj_set_width(ui_humidityValue2, lv_pct(45));
    lv_obj_set_x(ui_humidityValue2, 80);
    lv_obj_set_y(ui_humidityValue2, -60);
    lv_obj_set_align(ui_humidityValue2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_humidityValue2, "80 %");
    lv_obj_set_style_text_color(ui_humidityValue2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_humidityValue2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_humidityValue2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_humidityValue2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_CityName2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_CityName2, 20);
    lv_obj_set_width(ui_CityName2, lv_pct(45));
    lv_obj_set_x(ui_CityName2, -70);
    lv_obj_set_y(ui_CityName2, -40);
    lv_obj_set_align(ui_CityName2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_CityName2, "City Name:");
    lv_obj_set_style_text_color(ui_CityName2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_CityName2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_CityName2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_CityName2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_cityNameValue2 = lv_label_create(ui_Screen3);
    lv_obj_set_height(ui_cityNameValue2, 20);
    lv_obj_set_width(ui_cityNameValue2, lv_pct(45));
    lv_obj_set_x(ui_cityNameValue2, 80);
    lv_obj_set_y(ui_cityNameValue2, -40);
    lv_obj_set_align(ui_cityNameValue2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_cityNameValue2, "Jaipur");
    lv_obj_set_style_text_color(ui_cityNameValue2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_cityNameValue2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_cityNameValue2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_cityNameValue2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

}