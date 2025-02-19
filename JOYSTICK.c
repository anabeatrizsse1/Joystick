#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306/ssd1306.h"

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define BUTTON_PIN 22
#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C 
#define ADC_MIDPOINT 2160

uint16_t joystick_x, joystick_y;
bool button_pressed;
bool led_state = false;
bool pwm_leds_enabled = true;
int rectangle_stage = 0;
ssd1306_t display;
bool display_color = true;

void init_display() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&display, WIDTH, HEIGHT, false, OLED_ADDR, I2C_PORT);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
    
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);
}

void update_display() {
    ssd1306_fill(&display, !display_color);
    int x_min = 0, x_max = 120, y_min = 0, y_max = 56;

    if (rectangle_stage == 0) {
        ssd1306_rect(&display, 0, 0, 128, 64, display_color, !display_color);
    } else if (rectangle_stage == 2) {
        ssd1306_rect(&display, 10, 10, 108, 54, display_color, !display_color);
        x_min = 10; x_max = 110; y_min = 10; y_max = 56;
    } else if (rectangle_stage == 3) {
        ssd1306_rect(&display, 20, 20, 88, 44, display_color, !display_color);
        x_min = 20; x_max = 100; y_min = 20; y_max = 56;
    }

    int x_pos = (joystick_x * (x_max - x_min)) / 4095 + x_min;
    int y_pos = (joystick_y * (y_max - y_min)) / 4095 + y_min;

    ssd1306_rect(&display, y_pos, x_pos, 8, 8, display_color, display_color);
    ssd1306_send_data(&display);
}

void init_joystick() {
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
}

uint init_pwm(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

void init_led_pwm() {
    uint pwm_wrap = 4095;
    init_pwm(LED_RED, pwm_wrap);
    init_pwm(LED_BLUE, pwm_wrap);
}

void update_leds() {
    if (pwm_leds_enabled) {
        pwm_set_gpio_level(LED_RED, joystick_x > ADC_MIDPOINT ? joystick_x : 0);
        pwm_set_gpio_level(LED_BLUE, joystick_y > ADC_MIDPOINT ? joystick_y : 0);
    } else {
        pwm_set_gpio_level(LED_RED, 0);
        pwm_set_gpio_level(LED_BLUE, 0);
    }

    if (!button_pressed) {
        led_state = !led_state;
        if (!pwm_leds_enabled) led_state = false;
        gpio_put(LED_GREEN, led_state);
        rectangle_stage = (rectangle_stage + 1) % 4;
        sleep_ms(200);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Iniciando sistema...\n");

    adc_init();
    init_joystick();
    init_led_pwm();
    init_display();

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    printf("Sistema pronto!\n");

    while (true) {
        adc_select_input(0);
        joystick_x = adc_read();
        adc_select_input(1);
        joystick_y = adc_read();
        button_pressed = gpio_get(BUTTON_PIN);

        update_leds();
        update_display();

        printf("X: %u, Y: %u, Button: %d\n", joystick_x, joystick_y, button_pressed);
        sleep_ms(100);
    }

    return 0;
}
