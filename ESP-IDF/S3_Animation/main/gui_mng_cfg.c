/* Include Header Files */
#include <stdio.h>
#include "ui.h"
#include "gui_mng_cfg.h"

/* Private Macros */
#define NUM_ELEMENTS(x)                 (sizeof(x)/sizeof(x[0]))

/* Each GUI event can be connected to one handler function with this type. */
typedef void (*gui_mng_callback)(const gui_mng_event_data_t *data);

/* One row in the event-to-callback table. */
typedef struct _gui_mng_event_cb_t
{
  gui_mng_event_t   event;
  gui_mng_callback  callback;
} gui_mng_event_cb_t;

/* Structure to hold the pointers related to dog 4 sprite images */
typedef struct
{
  lv_obj_t *r1;
  lv_obj_t *r2;
  lv_obj_t *l1;
  lv_obj_t *l2;
  int32_t   last_x;
} dog_ctx_t;

/* Private Function Prototypes */
static void gui_mng_cloud_animation( lv_obj_t *screen );
static void gui_mng_balloon_animation( lv_obj_t *screen );
static void gui_mng_dog_animation( lv_obj_t * screen );
static void gui_mng_dog_animation_callback( lv_anim_t * a, int32_t x );
static void gui_startup( const gui_mng_event_data_t *data );

/* Private Variables */
static dog_ctx_t dog_ctx = { 0 };
static const gui_mng_event_cb_t gui_mng_event_cb[] =
{
  { GUI_MNG_EV_STARTUP,                 gui_startup                     },
};

/* Public Function Definitions */
/**
 * @brief GUI Configurable Initialization Function
 * @param  None
 */
void gui_cfg_init( void )
{
  ui_init( "A:" );

  #if 0
  /* NOTE: Actual GUI Screen is loaded by GUI MNG Events */
  /* Create and load the LVGL Pro generated main screen */
  lv_obj_t *screen = main_screen_create();
  lv_screen_load( screen );
  #endif
}

/**
 * @brief Process the events posted to GUI manager module
 *        This function calls the dedicated function based on the event posted
 *        to GUI manager queue.
 *
 * Simple idea:
 * - an event number comes in
 * - we search the table
 * - if we find matching event, we call its function
 *
 * @param event Event name.
 * @param data Pointer to copied typed event data.
 */
void gui_cfg_mng_process( gui_mng_event_t event, const gui_mng_event_data_t *data )
{
  uint8_t idx = 0;
  for( idx=0; idx < NUM_ELEMENTS(gui_mng_event_cb); idx++ )
  {
    // check if event matches the table
    if( event == gui_mng_event_cb[idx].event )
    {
      // call the callback function with arguments, if not NULL
      if( gui_mng_event_cb[idx].callback != NULL )
      {
        gui_mng_event_cb[idx].callback(data);
      }

      break;
    }
  }
}

/**
 * @brief this is a custom refresh function called periodically by GUI manager
 *        in this function we can write our code which can be called periodically
 */
void gui_cfg_refresh( void )
{
}

/* Private Function Definitions */

/**
 * @brief Initialize and start the cloud gliding animation.
 *        Moves the cloud horizontally from left to right (0 -> 670 px) over 6 seconds,
 *        then reverses from right to left (670 -> 0 px) over 6 seconds in an infinite loop.
 *
 * @param screen Pointer to the active screen object containing the cloud widget.
 */
static void gui_mng_cloud_animation( lv_obj_t *screen )
{
  /* Find the cloud widget created by LVGL Pro */
  lv_obj_t * cloud = lv_obj_get_child_by_name( screen, "img_cloud" );
  if ( NULL == cloud )
  {
    return;
  }

  /* Initialize the Animation Structure */
  lv_anim_t a;
  lv_anim_init( &a );

  /* Set the target widget to animate */
  lv_anim_set_var( &a, cloud );

  /* Set Start and End horizontal coordinates (0 to 670 px) */
  lv_anim_set_values( &a, 0, 670 );

  /* Set the callback function that updates the widget X-coordinate */
  lv_anim_set_exec_cb( &a, (lv_anim_exec_xcb_t)lv_obj_set_x );

  /* Set forward duration: 6000 ms (Left -> Right) */
  lv_anim_set_duration( &a, 6000 );

  /* Set reverse playback duration: 6000 ms (Right -> Left) */
  lv_anim_set_reverse_duration( &a, 6000 );

  /* Repeat continuously */
  lv_anim_set_repeat_count( &a, LV_ANIM_REPEAT_INFINITE );

  /* Start the animation */
  lv_anim_start( &a );
}

/**
 * @brief Initialize and start the Hot Air Balloon animation.
 *        Coordinates horizontal and vertical movement across a 12-second cycle:
 *        - 0s to 5s:   Glides from Right to Left (736 -> 0 px) at initial height (y = 150).
 *        - 5s to 6s:   Holds at left edge and descends 40 px (150 -> 190 px).
 *        - 6s to 11s:  Glides from Left to Right (0 -> 736 px) at lower height (y = 190).
 *        - 11s to 12s: Holds at right edge and ascends 40 px (190 -> 150 px) back to start.
 *
 * @param screen Pointer to the active screen object containing the balloon widget.
 */
static void gui_mng_balloon_animation( lv_obj_t *screen )
{
  /* Find the balloon widget created by LVGL Pro */
  lv_obj_t * balloon = lv_obj_get_child_by_name( screen, "img_balloon" );
  if ( NULL == balloon )
  {
    return;
  }

  /* ------------------------------------------------------------- */
  /* 1. Horizontal Animation (X coordinate)                        */
  /* ------------------------------------------------------------- */
  lv_anim_t a_x;
  lv_anim_init( &a_x );
  lv_anim_set_var( &a_x, balloon );

  /* Move from Right edge (736 px) to Left edge (0 px) */
  lv_anim_set_values( &a_x, 736, 0 );
  lv_anim_set_exec_cb( &a_x, (lv_anim_exec_xcb_t)lv_obj_set_x );

  /* Forward movement: Right to Left over 5000 ms */
  lv_anim_set_duration( &a_x, 5000 );

  /* Hold at left edge for 1000 ms while balloon descends */
  lv_anim_set_reverse_delay( &a_x, 1000 );

  /* Reverse movement: Left to Right over 5000 ms */
  lv_anim_set_reverse_duration( &a_x, 5000 );

  /* Hold at right edge for 1000 ms while balloon ascends before repeating */
  lv_anim_set_repeat_delay( &a_x, 1000 );

  /* Repeat continuously */
  lv_anim_set_repeat_count( &a_x, LV_ANIM_REPEAT_INFINITE );
  lv_anim_start( &a_x );

  /* ------------------------------------------------------------- */
  /* 2. Vertical Animation (Y coordinate)                          */
  /* ------------------------------------------------------------- */
  lv_anim_t a_y;
  lv_anim_init( &a_y );
  lv_anim_set_var( &a_y, balloon );

  /* Move from normal height (150 px) down to lower height (190 px) */
  lv_anim_set_values( &a_y, 150, 190 );
  lv_anim_set_exec_cb( &a_y, (lv_anim_exec_xcb_t)lv_obj_set_y );

  /* Initial delay: wait 5000 ms while balloon travels across the sky */
  lv_anim_set_delay( &a_y, 5000 );

  /* Repeat delay: wait 5000 ms on every subsequent cycle */
  lv_anim_set_repeat_delay( &a_y, 5000 );

  /* Descend 40 px over 1000 ms */
  lv_anim_set_duration( &a_y, 1000 );

  /* Hold at bottom height for 5000 ms while balloon travels right */
  lv_anim_set_reverse_delay( &a_y, 5000 );

  /* Ascend 40 px back to 150 over 1000 ms */
  lv_anim_set_reverse_duration( &a_y, 1000 );

  /* Repeat continuously */
  lv_anim_set_repeat_count( &a_y, LV_ANIM_REPEAT_INFINITE );
  lv_anim_start( &a_y );
}

/**
 * @brief Initialize and start the dog walking and stepping animation.
 *        Finds all 4 dog sprite frames, sets up a 12-second bidirectional animation
 *        (0 -> 632 px in 6s, and 632 -> 0 px in 6s), and registers the custom callback
 *        to handle leg stepping and direction switching.
 *
 * @param screen Pointer to the active screen object containing the dog widgets.
 */
static void gui_mng_dog_animation( lv_obj_t * screen )
{
  /* Find all 4 dog frames created by LVGL Pro */
  dog_ctx.r1 = lv_obj_get_child_by_name( screen, "img_dog_r1" );
  dog_ctx.r2 = lv_obj_get_child_by_name( screen, "img_dog_r2" );
  dog_ctx.l1 = lv_obj_get_child_by_name( screen, "img_dog_l1" );
  dog_ctx.l2 = lv_obj_get_child_by_name( screen, "img_dog_l2" );
  dog_ctx.last_x = 0;

  /* Verify that all 4 sprite frames exist */
  if ( (dog_ctx.r1 == NULL) || (dog_ctx.r2 == NULL) || (dog_ctx.l1 == NULL) || (dog_ctx.l2 == NULL) )
  {
    return;
  }

  /* Configure horizontal translation animation (0 to 632 px and reverse over 12 seconds) */
  lv_anim_t a_dog;
  lv_anim_init( &a_dog );
  lv_anim_set_var( &a_dog, &dog_ctx );
  lv_anim_set_values( &a_dog, 0, 632 );
  lv_anim_set_duration( &a_dog, 6000 );
  lv_anim_set_reverse_duration( &a_dog, 6000 );
  lv_anim_set_repeat_count( &a_dog, LV_ANIM_REPEAT_INFINITE );
  lv_anim_set_custom_exec_cb( &a_dog, gui_mng_dog_animation_callback );
  lv_anim_start( &a_dog );
}

/**
 * @brief Custom animation execution callback for dog walking motion.
 *        Called continuously by LVGL's animation engine as 'x' changes between 0 and 632.
 *
 *        Logic Breakdown:
 *        1. Compares current 'x' with 'last_x' to determine direction (Right vs Left).
 *        2. Calculates stride toggle: alternates stepping frame every 20 pixels of travel.
 *        3. When walking Right:
 *           - Hides left-facing images (l1, l2).
 *           - Updates position of right-facing images (r1, r2) to current 'x'.
 *           - Toggles opacity between r1 (Frame 1) and r2 (Frame 2) to animate leg steps.
 *        4. When walking Left:
 *           - Hides right-facing images (r1, r2).
 *           - Updates position of left-facing images (l1, l2) to current 'x'.
 *           - Toggles opacity between l1 (Frame 1) and l2 (Frame 2) to animate leg steps.
 *
 * @param a Pointer to the animation structure (contains dog_ctx in a->var).
 * @param x Current animated X-coordinate value (0 to 632).
 */
static void gui_mng_dog_animation_callback( lv_anim_t * a, int32_t x )
{
  dog_ctx_t *ctx = (dog_ctx_t *)a->var;

  if ( NULL == ctx )
  {
    return;
  }

  /* Detect walking direction based on position delta */
  static bool walking_right = true;
  if ( x > ctx->last_x )
  {
    walking_right = true;
  }
  else if ( x < ctx->last_x )
  {
    walking_right = false;
  }
  ctx->last_x = x;

  /* Alternate stepping frame every 20 pixels of horizontal distance */
  bool step_toggle = ( ( (x < 0 ? -x : x) / 20 ) % 2 ) != 0;

  if ( walking_right )
  {
    /* Hide left-facing images */
    lv_obj_set_style_opa( ctx->l1, LV_OPA_0, 0 );
    lv_obj_set_style_opa( ctx->l2, LV_OPA_0, 0 );

    /* Update right-facing positions */
    lv_obj_set_x( ctx->r1, x );
    lv_obj_set_x( ctx->r2, x );

    /* Alternate between right-facing Frame 1 and Frame 2 */
    if ( step_toggle )
    {
      lv_obj_set_style_opa( ctx->r1, LV_OPA_0, 0 );
      lv_obj_set_style_opa( ctx->r2, LV_OPA_COVER, 0 );
    }
    else
    {
      lv_obj_set_style_opa( ctx->r1, LV_OPA_COVER, 0 );
      lv_obj_set_style_opa( ctx->r2, LV_OPA_0, 0 );
    }
  }
  else
  {
    /* Hide right-facing images */
    lv_obj_set_style_opa( ctx->r1, LV_OPA_0, 0 );
    lv_obj_set_style_opa( ctx->r2, LV_OPA_0, 0 );

    /* Update left-facing positions */
    lv_obj_set_x( ctx->l1, x );
    lv_obj_set_x( ctx->l2, x );

    /* Alternate between left-facing Frame 1 and Frame 2 */
    if ( step_toggle )
    {
      lv_obj_set_style_opa( ctx->l1, LV_OPA_0, 0 );
      lv_obj_set_style_opa( ctx->l2, LV_OPA_COVER, 0 );
    }
    else
    {
      lv_obj_set_style_opa( ctx->l1, LV_OPA_COVER, 0 );
      lv_obj_set_style_opa( ctx->l2, LV_OPA_0, 0 );
    }
  }
}

/**
 * @brief Build a simple LVGL starter screen.
 *
 * @param data Pointer to event data structure.
 */
static void gui_startup( const gui_mng_event_data_t *data )
{
  (void)data;
  lv_obj_t *screen = main_screen_create();
  lv_screen_load( screen );

  gui_mng_cloud_animation( screen );
  gui_mng_balloon_animation( screen );
  gui_mng_dog_animation( screen );
}
