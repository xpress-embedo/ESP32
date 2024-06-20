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

typedef struct _widgets_t
{
  lv_obj_t * panel_table[MAX_NUM_OF_PARTY];
  lv_obj_t * name_table[MAX_NUM_OF_PARTY];        // for main screen
  lv_obj_t * name_rslt_table[MAX_NUM_OF_PARTY];   // for results (bar chart) screen
  lv_obj_t * logo_table[MAX_NUM_OF_PARTY];
  lv_obj_t * vote_btn_table[MAX_NUM_OF_PARTY];
  lv_obj_t * vote_rslt_table[MAX_NUM_OF_PARTY];
  lv_obj_t * vote_per_table[MAX_NUM_OF_PARTY];
} widgets_t;

// function template for callback function
typedef void (*gui_mng_callback)(uint8_t * data);

typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

// Private Function Prototypes
static void gui_vote_button_event( lv_event_t * e );
static void gui_results_button_event( lv_event_t * e );
static void gui_reset_button_event( lv_event_t *e );
static void gui_main_screen_event( lv_event_t *e );
static void gui_election_result_screen_event( lv_event_t *e );
static void winning_timer_anim_cb( lv_timer_t *timer );

// Private Variables
/* todo: will be used if new features will be added
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
};
*/
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
//{ NULL,         &ui_img_na_png         },
};

static const char        *  TAG = "GUI_CFG";
static uint8_t              winner_idx = 0;
static lv_coord_t           chart_series_array[MAX_NUM_OF_PARTY] = { 0 };
static lv_chart_series_t *  ui_chartResults_series = { NULL };
static uint16_t             votes[MAX_NUM_OF_PARTY] = { 0 };
static widgets_t            widgets_table = { NULL };

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
  widgets_table.name_table[0] = ui_lblPartyName1;
  widgets_table.name_table[1] = ui_lblPartyName2;
  widgets_table.name_table[2] = ui_lblPartyName3;
  widgets_table.name_table[3] = ui_lblPartyName4;
  widgets_table.name_table[4] = ui_lblPartyName5;
  widgets_table.name_table[5] = ui_lblPartyName6;
  widgets_table.name_table[6] = ui_lblPartyName7;

  widgets_table.panel_table[0] = ui_panelParty1;
  widgets_table.panel_table[1] = ui_panelParty2;
  widgets_table.panel_table[2] = ui_panelParty3;
  widgets_table.panel_table[3] = ui_panelParty4;
  widgets_table.panel_table[4] = ui_panelParty5;
  widgets_table.panel_table[5] = ui_panelParty6;
  widgets_table.panel_table[6] = ui_panelParty7;

  widgets_table.name_rslt_table[0] = ui_lblBarParty1;
  widgets_table.name_rslt_table[1] = ui_lblBarParty2;
  widgets_table.name_rslt_table[2] = ui_lblBarParty3;
  widgets_table.name_rslt_table[3] = ui_lblBarParty4;
  widgets_table.name_rslt_table[4] = ui_lblBarParty5;
  widgets_table.name_rslt_table[5] = ui_lblBarParty6;
  widgets_table.name_rslt_table[6] = ui_lblBarParty7;

  widgets_table.logo_table[0] = ui_imgPartyLogo1;
  widgets_table.logo_table[1] = ui_imgPartyLogo2;
  widgets_table.logo_table[2] = ui_imgPartyLogo3;
  widgets_table.logo_table[3] = ui_imgPartyLogo4;
  widgets_table.logo_table[4] = ui_imgPartyLogo5;
  widgets_table.logo_table[5] = ui_imgPartyLogo6;
  widgets_table.logo_table[6] = ui_imgPartyLogo7;

  widgets_table.vote_btn_table[0] = ui_btnVoteParty1;
  widgets_table.vote_btn_table[1] = ui_btnVoteParty2;
  widgets_table.vote_btn_table[2] = ui_btnVoteParty3;
  widgets_table.vote_btn_table[3] = ui_btnVoteParty4;
  widgets_table.vote_btn_table[4] = ui_btnVoteParty5;
  widgets_table.vote_btn_table[5] = ui_btnVoteParty6;
  widgets_table.vote_btn_table[6] = ui_btnVoteParty7;

  widgets_table.vote_rslt_table[0] = ui_lblPartyTotalVotes1;
  widgets_table.vote_rslt_table[1] = ui_lblPartyTotalVotes2;
  widgets_table.vote_rslt_table[2] = ui_lblPartyTotalVotes3;
  widgets_table.vote_rslt_table[3] = ui_lblPartyTotalVotes4;
  widgets_table.vote_rslt_table[4] = ui_lblPartyTotalVotes5;
  widgets_table.vote_rslt_table[5] = ui_lblPartyTotalVotes6;
  widgets_table.vote_rslt_table[6] = ui_lblPartyTotalVotes7;

  widgets_table.vote_per_table[0] = ui_lblPartyTotalVotesPercentage1;
  widgets_table.vote_per_table[1] = ui_lblPartyTotalVotesPercentage2;
  widgets_table.vote_per_table[2] = ui_lblPartyTotalVotesPercentage3;
  widgets_table.vote_per_table[3] = ui_lblPartyTotalVotesPercentage4;
  widgets_table.vote_per_table[4] = ui_lblPartyTotalVotesPercentage5;
  widgets_table.vote_per_table[5] = ui_lblPartyTotalVotesPercentage6;
  widgets_table.vote_per_table[6] = ui_lblPartyTotalVotesPercentage7;

  // get the maximum number of political parties
  num_of_parties = get_number_of_parties();
  ESP_LOGI( TAG, "Fetched Party Number: %d", num_of_parties );

  for( idx=0; idx < num_of_parties; idx++ )
  {
    name = get_name_of_party(idx);
    // ESP_LOGI( TAG, "Fetched Party Name: %s", name );
    // search the list
    for( jdx=0; jdx<NUM_ELEMENTS(party_logo_db_table); jdx++ )
    {
      if( strcmp( party_logo_db_table[jdx].name, name) == 0 )
      {
        // ESP_LOGI( TAG, "Found!!");
        lv_label_set_text(widgets_table.name_table[idx], name);
        lv_label_set_text(widgets_table.name_rslt_table[idx], name);
        lv_img_set_src( widgets_table.logo_table[idx], party_logo_db_table[jdx].logo );
        // register the callback for vote button press
        lv_obj_add_event_cb( widgets_table.vote_btn_table[idx], gui_vote_button_event, LV_EVENT_PRESSED, NULL );
        break;
      }
    }
  }
  // updating the remaining entries
  for( idx=num_of_parties; idx < MAX_NUM_OF_PARTY; idx++ )
  {
    lv_label_set_text( widgets_table.name_table[idx], "-"  );
    lv_label_set_text( widgets_table.name_rslt_table[idx], "-" );
    lv_img_set_src( widgets_table.logo_table[idx], (const lv_img_dsc_t*)&ui_img_na_png );
  }

  // initialize the bar chart series
  ui_chartResults_series = lv_chart_add_series(ui_chartResults, lv_color_hex(0x5B7C72), LV_CHART_AXIS_PRIMARY_Y);
  // for now keep the range 0-10, but in get results section the range will be updated automatically based on the votes
  lv_chart_set_range(ui_chartResults, LV_CHART_AXIS_PRIMARY_Y, 0, 10 );
  lv_chart_set_ext_y_array( ui_chartResults, ui_chartResults_series, chart_series_array );

  // register callback for result button
  lv_obj_add_event_cb( ui_btnResults, gui_results_button_event, LV_EVENT_PRESSED, NULL );
  // register callback for reset button
  lv_obj_add_event_cb( ui_btnReset, gui_reset_button_event, LV_EVENT_PRESSED, NULL );
  // register callback for left and right swipe gesture (swipe events will be handled in the callback functions)
  lv_obj_add_event_cb( ui_MainScreen, gui_main_screen_event, LV_EVENT_ALL, NULL );
  lv_obj_add_event_cb( ui_ResultsBarScreen, gui_election_result_screen_event, LV_EVENT_ALL, NULL );
}

/**
 * @brief Process the events posted to GUI manager module
 *        This function calls the dedicated function based on the event posted
 *        to GUI manager queue,v I will think of moving this function to GUI manager
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
/**
 * @brief Callback Function configured for Voting Buttons
 * @param e 
 */
static void gui_vote_button_event(lv_event_t * e)
{
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t * target = lv_event_get_target(e);
  uint8_t idx;

  if( event_code == LV_EVENT_PRESSED )
  {
    for( idx=0; idx < MAX_NUM_OF_PARTY; idx++ )
    {
      if( target == widgets_table.vote_btn_table[idx] )
      {
        votes[idx]++;
        // ESP_LOGI( TAG, "Party Name: %s, Votes: %d", party_name_table[idx], votes[idx] );
        break;
      }
    }
  }
}

/**
 * @brief Callback Function configured for Result Buttons
 * @param e 
 */
static void gui_results_button_event( lv_event_t * e )
{
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t * target = lv_event_get_target(e);
  uint8_t idx;
  uint16_t total_votes = 0;
  uint16_t max_votes = 0;
  uint8_t votes_percentage = 0;
  uint8_t votes_fraction = 0;

  if( (event_code == LV_EVENT_PRESSED) && (target == ui_btnResults) )
  {
    for( idx = 0; idx < MAX_NUM_OF_PARTY; idx++ )
    {
      if( max_votes < votes[idx] )
      {
        max_votes = votes[idx];
        winner_idx = idx;
      }
      lv_label_set_text_fmt( widgets_table.vote_rslt_table[idx], "%d", votes[idx] );
      total_votes += votes[idx];
      // update the votes for chart bar
      chart_series_array[idx] = (lv_coord_t)(votes[idx]);
    }

    for( idx = 0; idx < MAX_NUM_OF_PARTY; idx++ )
    {
      if( total_votes )
      {
        votes_percentage = (votes[idx]*100)/total_votes;
        votes_fraction = (votes[idx]*100)%total_votes;
        lv_label_set_text_fmt( widgets_table.vote_per_table[idx], "%2d.%.1d %%", votes_percentage, votes_fraction );
      }
      else
      {
        // no votes casted and user presses the Results button, this is added to protect divide by 0 condition
        lv_label_set_text_fmt( widgets_table.vote_per_table[idx], "0 %%" );
      }
    }

    // update bar chart data
    lv_chart_set_range(ui_chartResults, LV_CHART_AXIS_PRIMARY_Y, 0, ((max_votes/10)+1)*10 );

    // Play Animation Using Timer
    ESP_LOGI( TAG, "Starting Timer, Winner Index: %d", winner_idx );
    lv_timer_t * winner_timer_anim = lv_timer_create( winning_timer_anim_cb, 300, NULL );
    // repeat animation 20 times
    lv_timer_set_repeat_count( winner_timer_anim, 20 );
  }
}

/**
 * @brief Callback Function configured for Reset Button
 * @param e 
 */
static void gui_reset_button_event( lv_event_t *e )
{
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t * target = lv_event_get_target(e);
  uint8_t idx;

  if( (event_code == LV_EVENT_PRESSED) && (target == ui_btnReset) )
  {
    for( idx = 0; idx < MAX_NUM_OF_PARTY; idx++ )
    {
      votes[idx] = 0;
      winner_idx = 0;
      chart_series_array[idx] = 0;
      lv_label_set_text_fmt( widgets_table.vote_rslt_table[idx], "%d", votes[idx] );
      lv_label_set_text_fmt( widgets_table.vote_per_table[idx], "0 %%" );
      lv_chart_set_range(ui_chartResults, LV_CHART_AXIS_PRIMARY_Y, 0, 10);
      lv_obj_set_style_bg_color( widgets_table.panel_table[winner_idx], lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_opa( widgets_table.panel_table[winner_idx], 255, LV_PART_MAIN | LV_STATE_DEFAULT );
    }
  }
}

/**
 * @brief Callback Function for changing screen to Election Results Screen
 * @param e
 */
static void gui_main_screen_event( lv_event_t *e )
{
  lv_event_code_t event_code = lv_event_get_code(e);
  // lv_obj_t * target = lv_event_get_target(e);    // not used

  if( (event_code == LV_EVENT_GESTURE) &&  (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) )
  {
    lv_indev_wait_release(lv_indev_get_act());
    _ui_screen_change(&ui_ResultsBarScreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_ResultsBarScreen_screen_init);
  }
}

/**
 * @brief Callback Function changing screen back to main screen
 * @param e
 */
static void gui_election_result_screen_event( lv_event_t *e )
{
  lv_event_code_t event_code = lv_event_get_code(e);
  // lv_obj_t * target = lv_event_get_target(e);    // not used

  if( (event_code == LV_EVENT_GESTURE) &&  (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) )
  {
    lv_indev_wait_release(lv_indev_get_act());
    _ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_MainScreen_screen_init);
  }
}

/**
 * @brief Winning Timer Animation Function Callback
 *        Here for winning party a small manual animation is displayed
 * @param timer 
 */
static void winning_timer_anim_cb( lv_timer_t *timer )
{
  static bool toggle = true;

  if( toggle )
  {
    toggle = false;
    lv_obj_set_style_bg_color( widgets_table.panel_table[winner_idx], lv_color_hex(0xFFA500), LV_PART_MAIN | LV_STATE_DEFAULT );
    // NOTE: this bg_opa function is mandatory to see the effects, no idea but will read about it
    lv_obj_set_style_bg_opa( widgets_table.panel_table[winner_idx], 255, LV_PART_MAIN | LV_STATE_DEFAULT );
  }
  else
  {
    toggle = true;
    lv_obj_set_style_bg_color( widgets_table.panel_table[winner_idx], lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa( widgets_table.panel_table[winner_idx], 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  }
}
