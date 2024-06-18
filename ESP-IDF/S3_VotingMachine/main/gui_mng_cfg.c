/*
 * gui_mng_cfg.c
 *
 *  Created on: Jun 15, 2024
 *      Author: xpress_embedo
 */

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "main.h"
#include "ui.h"
#include "lvgl.h"
#include "gui_mng.h"
#include "gui_mng_cfg.h"

// Private Macros
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

typedef struct _party_logo_t
{
  char *name;
  const lv_img_dsc_t *logo;
} party_logo_t;

// function template for callback function
typedef void (*gui_mng_callback)(uint8_t * data);

typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

// Private Function Prototypes

// Private Variables
static const char *TAG = "GUI_CFG";

//static const gui_mng_event_cb_t gui_mng_event_cb[] =
//{
//};

// this is party logo database table (this consist of all the party names and logos mapped)
static party_logo_t party_logo_db_table[] =
{
  { "AAP",        &ui_img_aap_png        },
  { "BJP",        &ui_img_bjp_png        },
  { "BSP",        &ui_img_bsp_png        },
  { "Congress",   &ui_img_congress_png   },
  { "CPI",        &ui_img_cpi_png        },
  { "NCP",        &ui_img_ncp_png        },
  { "SP",         &ui_img_sp_png         },
  { "TMC",        &ui_img_tmc_png        },
  { "JDU",        &ui_img_jdu_png        },
  // { NULL,         &ui_img_na_png         },
};

static lv_obj_t * party_name_table[MAX_NUM_OF_PARTY];
static lv_obj_t * party_logo_table[MAX_NUM_OF_PARTY];

// Public Function Definitions

/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  uint8_t num_of_parties = 0;
  char *name;
  uint8_t idx = 0;
  uint8_t jdx = 0;

  ui_init();

  // prepare widget table
  party_name_table[0] = ui_lblPartyName1;
  party_name_table[1] = ui_lblPartyName2;
  party_name_table[2] = ui_lblPartyName3;
  party_name_table[3] = ui_lblPartyName4;
  party_name_table[4] = ui_lblPartyName5;
  party_name_table[5] = ui_lblPartyName6;
  party_name_table[6] = ui_lblPartyName7;

  party_logo_table[0] = ui_imgPartyLogo1;
  party_logo_table[1] = ui_imgPartyLogo2;
  party_logo_table[2] = ui_imgPartyLogo3;
  party_logo_table[3] = ui_imgPartyLogo4;
  party_logo_table[4] = ui_imgPartyLogo5;
  party_logo_table[5] = ui_imgPartyLogo6;
  party_logo_table[6] = ui_imgPartyLogo7;

  // get the maximum number of political parties
  num_of_parties = get_number_of_parties();
  ESP_LOGI( TAG, "Fetched Party Number: %d", num_of_parties );

  for( idx=0; idx < num_of_parties; idx++ )
  {
    name = get_name_of_party(idx);
    ESP_LOGI( TAG, "Fetched Party Name: %s", name );
    lv_label_set_text(party_name_table[idx], name);
    // search the list
    for( jdx=0; jdx<NUM_ELEMENTS(party_logo_db_table); jdx++ )
    {
      ESP_LOGI( TAG, "JDX: %d", jdx );
      if( strcmp( party_logo_db_table[jdx].name, name) == 0 )
      {
        ESP_LOGI( TAG, "Found!!");
        lv_img_set_src( party_logo_table[idx], party_logo_db_table[jdx].logo );
        break;
      }
    }
  }
}

/**
 * @brief Process the events posted to GUI manager module
 *        This function calls the dedicated function based on the event posted
 *        to GUI manager queue, I will think of moving this function to GUI manager
 * @param event event name
 * @param data event data pointer
 */
void gui_cfg_mng_process( gui_mng_event_t event, uint8_t *data )
{
//  uint8_t idx = 0;
//  for( idx=0; idx < NUM_ELEMENTS(gui_mng_event_cb); idx++ )
//  {
//    // check if event matches the table
//    if( event == gui_mng_event_cb[idx].event )
//    {
//      // call the callback function with arguments, if not NULL
//      if( gui_mng_event_cb[idx].callback != NULL )
//      {
//        gui_mng_event_cb[idx].callback(data);
//      }
//    }
//  }
}

// Private Function Definitions

