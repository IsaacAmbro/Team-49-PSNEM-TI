//Yay
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h" // needed for gpio pins
#include "driver/uart.h" // Needed for Uart
#include "driver/spi_master.h" // For spi

#include "time.h"
#include "sys/time.h"


#define SPP_TAG "ESP_SPP_PROJ" //For logs
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_BT_PSNEM" //What shows up on phone
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA    /*Choose show mode: show data or speed*/

#define UART_TX2 17 //uart to msp430
#define UART_RX2 16 // Same as above, probably going unused
const uart_port_t uart_MSP = UART_NUM_2; //


#define ESP_HOST VSPI_HOST // THIS SHOULD WORK AS IS, IF NOT, REORDER BOARD W/ THESE PINS SETUP FOR VSPI
#define SPI_TAG "spi_protocol" 
#define ADC_SCLK 21 // output pin to adc
#define ADC_DIN 19 //this is MOSI output pin to adc.
#define ADC_DOUT 18 //this is MISO input pin from adc  
#define ADC_CS 5 // output pin from adc

static uint32_t Gooboo=0; //Used to be a dummy value. I liked the name so I'm keeping it, its my good luck charm
// In short, needed a global value to store my connected devices handle so I can send BT data from any function

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const bool esp_spp_enable_l2cap_ertm = true;

static struct timeval time_new, time_old;
static long data_num = 0;  // can ignore these

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE; //shows pop up on phone on first connection
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
spi_device_handle_t adc_spi; //name of our spi slave device

static void start_pins(void){ //setting up LEDS, UART, SPI 
    
    gpio_reset_pin(UART_TX2);
    gpio_reset_pin(UART_RX2);
    uart_config_t uart_config = { //setting up uart
        .source_clk = UART_SCLK_DEFAULT, 
        .baud_rate = 115200, //from issac
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE, 
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //we hopefully not going to use, our uart is super basic, just two devices, abc output, maybe a 1 or 0 input type deal.
        .rx_flow_ctrl_thresh = 122, //ill keep this, but bc the above line is disable it shouldnt matter at all.
    };
    ESP_ERROR_CHECK(uart_param_config(uart_MSP, &uart_config));
    uart_set_pin(uart_MSP,UART_TX2,UART_RX2,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE); //for this uart, name the tx and rx pinms
    const int uart_buffer_MSP = (1024*2);
    QueueHandle_t MSP_queue; //pointer for data to be queued at
    uart_driver_install(uart_MSP,uart_buffer_MSP,uart_buffer_MSP,10,&MSP_queue,0); //buffer and queue above, 10 is the queue size? Likely increase this? 0 is queue flags
    ESP_LOGI(SPP_TAG,"UART PINS READY TO GO");

    vTaskDelay(2000/portTICK_PERIOD_MS); //Wait to allow ADC setup time, subject to change
    gpio_set_direction(ADC_CS,GPIO_MODE_OUTPUT);
    gpio_reset_pin(ADC_SCLK);
    gpio_reset_pin(ADC_DIN);
    gpio_reset_pin(ADC_DOUT);
    gpio_reset_pin(ADC_CS);
    spi_bus_config_t Spibus = {//setting up our pins, cant use default as I didnt set that right on board design. Technically limits data rate, but theres other limits too
        .miso_io_num = ADC_DOUT,
        .mosi_io_num = ADC_DIN,
        .sclk_io_num = ADC_SCLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1, //show unused basically
        .max_transfer_sz = 32 , // i think 4092 is the default, but might as well track it. Im only ever looking at ~12 bits of 2s complement so idc about this too much
    };
    spi_device_interface_config_t SpiDev = {//setup slave device
        .mode = 1,
        .spics_io_num = ADC_CS,
        .clock_speed_hz = 1000 *1000, //ADC internal oscil at 1mhz
        .queue_size = 8, //kinda artbitrary for now..
    };
    ESP_ERROR_CHECK( spi_bus_initialize(ESP_HOST,&Spibus,SPI_DMA_CH_AUTO) ); //NOT SURE ABOUT LAST TERM...
    ESP_ERROR_CHECK( spi_bus_add_device(ESP_HOST,&SpiDev,&adc_spi) ); //initialize everything
    ESP_LOGI(SPP_TAG,"ADC PINS READY TO GO");
}
void looper(){ //for reading and writing spi
    int outlen = 0;
    esp_err_t ret;
    uint8_t step[4];
    uint8_t* step2;
    vTaskDelay(3000/portTICK_PERIOD_MS); //delay to ensure no issues
    int i = 1; //this is for looping
    uint8_t* test_data = (uint8_t*) malloc(50); //setup data buffer
    uint8_t* BT_output = (uint8_t*) malloc(50); //second buffer
    while (1){ //forever loop
        int l = i % 2;
        if (l==0){ //prevent as much spam of test statements, every other loop
            printf("Upload UART, In loop %d \n", i ); //print from the upload uart, show logs
            int len = sprintf((char*)test_data, "From sprintf, loop no. %d\n", i); //loads the buffer with output text that increments
            uart_write_bytes(uart_MSP,(const char*)test_data,len); //send same data as above to uart2 (which will be the msp430). Used to check that im connected properly and Im sending data from b/t as well
        
        }
        //above are two test statements to show working behavior on both the upload and UART pins
        
        uint16_t config = 0x8583; //Config reg is 16 bits 0-15
                //0b1000010110001011 ->0x858B //briefly changed
        // In single shot mode. should require a transmission each time. 
        spi_transaction_t trans = { //

            .length = 16, //16 bits len
            .tx_buffer= &config, //This is the data I send to the adc, point to it?

        };
        ret = spi_device_transmit(adc_spi,&trans);
        assert(ret==ESP_OK);
        outlen = sprintf((char*)BT_output, "SPI TRANS: %d \n",config);
        esp_spp_write(Gooboo,outlen,BT_output);  //Another print statement to check that I sent it should show 34187 to match my sent data
        

        spi_transaction_t recv; //receive data
        memset(&recv,0,sizeof(recv) ) ; //make sure our recv data stuff is zero before i do anything
        recv.length =32; //still 16 bits total, but RX is 32, ill dig through to find what i want
        recv.flags = SPI_TRANS_USE_RXDATA; //sennding to rxdata not buffer
            ret = spi_device_transmit(adc_spi,&recv); 
        assert(ret==ESP_OK);//ends if broken

        //My received data should only be 16 bits, but because Im using the rx data part, its being sent to an array of 4 uint8t, thus the following lines
        step[0] = recv.rx_data[0]; // 8 bits x 4 of my received data. which should be 16 bits. 
        step[1] = recv.rx_data[1]; // 
        step[2] = recv.rx_data[2]; // 
        step[3] = recv.rx_data[3]; //
        //I'm looking to see what data I'm actually transmitting/receiveing so I can properly parse it. I should be able to convert 2s comp, but I need to find where its going. Thats my current step
        //I'm trying to narrow down whats not working/I'm missing. Theres a chance my spi rx tx isnt working, my ADC is setup wrong, or that my board is. 1 I can maybe fix, the other two may need a board redesign

        step2 = &step[0]; //step2 grabs the addr of where data is
        esp_spp_write(Gooboo,8, step2); //Actual data transmission. Gooboo is the connected devices handle, 8 is length, the step is an array of uint8 t so it should be const. Step2 should be one of the 4 received datas
            outlen = sprintf((char*)BT_output, "Sent Data %d ,%d \n",1,step[0]); //show from what i sent and what I should have sent
            esp_spp_write(Gooboo,outlen,BT_output); //sending my test statement
        vTaskDelay( 50 / portTICK_PERIOD_MS); //delay X/1000 sec. These delays were to ensure I wasnt losing data. step[2] and 3 would sometimes not transmitt over BT this helped. 

            step2 = &step[1];
        esp_spp_write(Gooboo,8,step2 );
            outlen = sprintf((char*)BT_output, "Sent Data %d ,%d \n",2,step[1]);
            esp_spp_write(Gooboo,outlen,BT_output);
        vTaskDelay( 50 / portTICK_PERIOD_MS); //delay X/1000 sec

        step2 = &step[2];  
        esp_spp_write(Gooboo,8,step2 );
            outlen = sprintf((char*)BT_output, "Sent Data %d ,%d \n",3,step[2]);
            esp_spp_write(Gooboo,outlen,BT_output);
        vTaskDelay( 50 / portTICK_PERIOD_MS); //delay X/1000 sec
        
        step2 = &step[3];  
        esp_spp_write(Gooboo,8, step2  );  
            outlen = sprintf((char*)BT_output, "Sent Data %d ,%d \n",4,step[3]);
            esp_spp_write(Gooboo,outlen,BT_output); 
        vTaskDelay( 50 / portTICK_PERIOD_MS); //delay X/1000 sec
        
        i++;
        vTaskDelay( 1500 / portTICK_PERIOD_MS); //delay X/1000 secs between loop
    }



    free(test_data); //mem stuff
    free(BT_output);
}

static char *bda2str(uint8_t * bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }
    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}
static void print_speed(void) //UNUSED
{
    float time_old_s = time_old.tv_sec + time_old.tv_usec / 1000000.0;
    float time_new_s = time_new.tv_sec + time_new.tv_usec / 1000000.0;
    float time_interval = time_new_s - time_old_s;
    float speed = data_num * 8 / time_interval / 1000.0;
    ESP_LOGI(SPP_TAG, "speed(%fs ~ %fs): %f kbit/s" , time_old_s, time_new_s, speed);
    data_num = 0;
    time_old.tv_sec = time_new.tv_sec;
    time_old.tv_usec = time_new.tv_usec;
}
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) // CALLBACKS, ONLY EDITED DATA TRANSFER ONE
{
    char bda_str[18] = {0};
    switch (event) {
    case ESP_SPP_INIT_EVT:
        if (param->init.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
            esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_INIT_EVT status:%d", param->init.status);
        }
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT status:%d handle:%"PRIu32" close_by_remote:%d", param->close.status,
                 param->close.handle, param->close.async);
        break;
    case ESP_SPP_START_EVT:
        if (param->start.status == ESP_SPP_SUCCESS) {
            Gooboo = param->start.handle;
            ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT handle:%"PRIu32" sec_id:%d scn:%d", param->start.handle, param->start.sec_id,
                     param->start.scn);
            esp_bt_gap_set_device_name(EXAMPLE_DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_START_EVT status:%d", param->start.status);
        }
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        /*
         * We only show the data in which the data length is less than 128 here. If you want to print the data and
         * the data rate is high, it is strongly recommended to process them in other lower priority application task   //not my comment, good note for later though
         * rather than in this callback directly. Since the printing takes too much time, it may stuck the Bluetooth
         * stack and also have a effect on the throughput!
         */
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len:%d handle:%"PRIu32,
                 param->data_ind.len, param->data_ind.handle);
        if (param->data_ind.len < 128) { // send 126 characters as 2 are resevered for 0d 0a for ending it. This should be ample for the minimal b/t receiving i will do
            esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);
        }
#else
        gettimeofday(&time_new, NULL);
        data_num += param->data_ind.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
    uint8_t* output_data = (uint8_t*) malloc(30); //adj size to fix issues
    char received_data[128];
    snprintf(received_data,sizeof(received_data),"%s", param->data_ind.data); // this line takes the input data in data_ind,
    //turns it to a string, then places up to (what should be) 126 characters of it to the received data buffer / location. 126 bc it counts the \n characters too

    if (strncmp(received_data, "LDO", 3 ) == 0){//comparing for LDO all caps, first 3 digits of received data
        ESP_LOGI(SPP_TAG, "RECEIVED \"LDO\""); //log info to upl uart
        esp_spp_write(param->data_ind.handle,param->data_ind.len, param->data_ind.data); //should just spew back input data as 
        uart_write_bytes(uart_MSP,param->data_ind.data,param->data_ind.len); //send same data as above to uart2, should be the msp430


    }
    else if(strncmp(received_data,"SWTCHREG",8) == 0){//looking for swtchreg
        ESP_LOGI(SPP_TAG, "RECEIVED \"SWTCHREG\""); //log info
        esp_spp_write(param->data_ind.handle,param->data_ind.len, param->data_ind.data); //should just spew back input data as output
        uart_write_bytes(uart_MSP,param->data_ind.data,param->data_ind.len); //send same data as above to uart2, should be the msp430
    }

    else if(strncmp(received_data,"PWRSUP",6) == 0){//looking for pwrsup
        ESP_LOGI(SPP_TAG, "RECEIVED \"PWRSUP\""); //log info
        esp_spp_write(param->data_ind.handle,param->data_ind.len, param->data_ind.data); //should just spew back input data as output
        uart_write_bytes(uart_MSP,param->data_ind.data,param->data_ind.len); //send same data as above to uart2, should be the msp430.should only be 6, dont want to send extra if there is any
    }
    else{
        ESP_LOGI(SPP_TAG, "DID NOT RECOGNIZE"); //log info
        int len = sprintf((char*)output_data, "IGNORE/INVALID\n");
        esp_spp_write(param->data_ind.handle,param->data_ind.len, param->data_ind.data); //should just spew back input data as output? REMOVE/REPLACE LATER?
        uart_write_bytes(uart_MSP,(const char*)output_data,len); //sending something else instead of junk, showing ignore
    }
        free(output_data); //this may need to move

        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT status:%d handle:%"PRIu32", rem_bda:[%s]", param->srv_open.status,
                 param->srv_open.handle, bda2str(param->srv_open.rem_bda, bda_str, sizeof(bda_str)));
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) // this just handles authentification, dont touch
{
    char bda_str[18] = {0};
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{ //for successful or failed connection atm
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s bda:[%s]", param->auth_cmpl.device_name,
                     bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)));
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{ // dont know about this case yet
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_EXAMPLE_SSP_ENABLED == true) //true for mine
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %"PRIu32, param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%"PRIu32, param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d bda:[%s]", param->mode_chg.mode,
                 bda2str(param->mode_chg.bda, bda_str, sizeof(bda_str)));
        break;
    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}
void app_main(void) //half I dont touch, dont want to break
{
    start_pins(); //setup pins
    char bda_str[18] = {0};
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
#if (CONFIG_EXAMPLE_SSP_ENABLED == false)
    bluedroid_cfg.ssp_en = false;
#endif
    if ((ret = esp_bluedroid_init_with_cfg(&bluedroid_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    esp_spp_cfg_t bt_spp_cfg = {
        .mode = esp_spp_mode,
        .enable_l2cap_ertm = esp_spp_enable_l2cap_ertm,
        .tx_buffer_size = 0, /* Only used for ESP_SPP_MODE_VFS mode */
    };
    if ((ret = esp_spp_enhanced_init(&bt_spp_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
#if (CONFIG_EXAMPLE_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif
    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
    ESP_LOGI(SPP_TAG, "Own address:[%s]", bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
    looper(); //start our loop to grab ADC data, if this works i would be suprised
}