/* Jared Sutor 330009308, Hello world, blinky */


#include <stdio.h> //Include Standard Input output C lang
#include <inttypes.h>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h" // allow freeRTOS to run, this is a real time operating system kernel
#include "freertos/task.h" // Multitasking functionality 

#include "esp_chip_info.h"  // from an example, used to grab info on chip type, number of cores, etc. Won't use in this proj prolly
#include "esp_flash.h"  //I Think this one helps with reading and writing data in flash memory? General memory management ig
#include "esp_system.h" // Configures the peripherals in ESP. System initialization effectively 
#include "esp_log.h" // logging cap
#include "driver/gpio.h" // needed for gpio pins


#include "math.h"

// I didnt end up using most of the above includes., but was helpful during my learning 
#define PIN1 23
#define PIN2 22

void app_main(void) //main func
{
    ESP_LOGI("LOG", "Pin 23 LED blink and hello world"); // various types of log statements, loge error, w is warning, i is info. 
    //the E(XXX) is milisecond it happened at. 

    gpio_set_direction(PIN1,GPIO_MODE_OUTPUT);//pin 23 as output
     gpio_set_direction(PIN2,GPIO_MODE_OUTPUT);//pin 23 as output
    uint32_t isOn=0; //on and off set 

    int i = 1;  //for incrementing

    while (1){ //while true loop
        isOn = !isOn; //flip off and on per cycle
        gpio_set_level(PIN1,isOn); //set 23 to high or low which hopefully blinks my led!
        gpio_set_level(PIN2,!isOn); //set 23 to high or low which hopefully blinks my 2nd led!



        vTaskDelay( 1000 / portTICK_PERIOD_MS);// delay commandd so i can see the blinks
        //1000 is in ticks, portick is defined by chip, this gives ms. This prints as 1 per second

        printf("hello world! In loop %d \n", i ); //print from uart, show whats going on and if it works
        i++;
}
}
