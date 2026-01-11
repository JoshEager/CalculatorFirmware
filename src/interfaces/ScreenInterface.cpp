#include "interfaces/ScreenInterface.h"

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "config.h"
#include "tools/debug.h"
#include "interfaces/KeypadInterface.h"
#include "gui/main_menu.h"


namespace ScreenInterface {
    TFT_eSPI *tft = nullptr;
    lv_display_t *display;

    void initTFT_eSPI() {
        tft = new TFT_eSPI();

        PRINTDBG("Initializing tft...");
        tft->init();
        PRINTDBG("TFT initialized!");
        tft->setRotation(2);
        tft->fillScreen(TFT_BLACK);
    }

    void print_cb(lv_log_level_t level, const char *buf) {
        LV_UNUSED(level);
        PRINTDBG(buf);
    }

    uint32_t tick_get_cb() {
        return millis();
    }

    static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);

        // Use TFT_eSPI optimized functions to push the color data
        tft->startWrite();                                  // Start new TFT transaction
        tft->setAddrWindow(area->x1, area->y1, w, h);       // Set the working window
        
        // Push the pixel map buffer to the display
        // The 'false' parameter indicates the data is not byte-swapped, which is generally correct for LVGL color formats
        tft->pushColors((uint16_t *)px_map, w * h, false); 

        tft->endWrite();                                    // Terminate TFT transaction

        // Tell LVGL that the flushing is done
        lv_display_flush_ready(disp); 
    }

    static void keypad_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
        static uint32_t last_key = 0; // needs to be static so that it can be used again
        char key = KeypadInterface::getKey(); // non blocking

        if (key != 0) { // getKey() returns 0 if no key was pressed
            data->state = LV_INDEV_STATE_PRESSED;

            switch (key) {
                // The number in the case represents the row number concatenated to the column number
                case 14: 
                    last_key = LV_KEY_ENTER;
                    PRINTDBG("Enter key pressed!");
                break;
                
                case 73: 
                    last_key = LV_KEY_UP; 
                    PRINTDBG("Up key pressed!");
                break;

                case 53:
                    last_key = LV_KEY_DOWN;
                    PRINTDBG("Down key pressed!");
                break;

                case 62:
                    last_key = LV_KEY_LEFT;
                    PRINTDBG("Left key pressed!");
                break;

                case 64:
                    last_key = LV_KEY_RIGHT;
                    PRINTDBG("Right key pressed");
                break;

                default: 
                    last_key = (uint32_t)key;
                break;
            }

        } else { // no key was pressed
            data->state = LV_INDEV_STATE_RELEASED;
        }
        data->key = last_key;
    }

    // Touchpad read callback for LVGL
    static void touchpad_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
        uint16_t touchX, touchY;
        bool touched = tft->getTouch(&touchX, &touchY);

        if (touched) {
            data->state = LV_INDEV_STATE_PRESSED;
            data->point.x = touchX;
            data->point.y = touchY;
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }

    void initLVGL() {
        lv_init();

        lv_log_register_print_cb(print_cb);

        lv_tick_set_cb(tick_get_cb);

        display = lv_display_create(ILI9341_WIDTH, ILI9341_HEIGHT);
        lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565_SWAPPED);

        static uint8_t buf[ILI9341_WIDTH * ILI9341_HEIGHT / 10 * 2];
        lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
        lv_display_set_flush_cb(display, flush_cb);

        // Register keypad input device
        lv_indev_t *keypad_indev = lv_indev_create();
        lv_indev_set_type(keypad_indev, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(keypad_indev, keypad_read_cb);

        // Register touchscreen input device
        lv_indev_t *touchpad_indev = lv_indev_create();
        lv_indev_set_type(touchpad_indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(touchpad_indev, touchpad_read_cb);
    }
    

    void uiTask(void *pvParameters) {
        PRINTDBG("Entered uiTask()...");
        while (1) {
            lv_timer_handler(); // The timer task actually renders the objects on the screen
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }

        vTaskDelete(NULL);
    }

    void startUI() {
        xTaskCreate(
            uiTask,     // Task function
            "UI Task",  // Task Name
            8192,       // Stack depth (may need to increase this)
            NULL,       // Parameters
            1,          // Priority
            NULL        // Task Handle
        );
    }
    
    void begin() {
        initTFT_eSPI();
        initLVGL();

        gui::create_main_menu();

        startUI();
    }

}