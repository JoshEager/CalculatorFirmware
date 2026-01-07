#include "gui/main_menu.h"

#include <lvgl.h>

namespace gui {
    void create_main_menu() {

        // Getting the current active screen
        lv_obj_t *screen = lv_scr_act();

        // Setting the background of the current active screen to blue grey
        lv_obj_set_style_bg_color(screen, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);

        // Creating a label
        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, "Hello world!");
    }
}
