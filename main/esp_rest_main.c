/* HTTP Restful API Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_vfs_semihost.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mdns.h"
#include "lwip/apps/netbiosns.h"
#include "protocol_examples_common.h"

/*
 * GAME HEADERS
 */
#include "alphabeta.h"
#include "board.h"
#include "game.h"
#include "limits.h"

#include "wifi.h"

#if CONFIG_EXAMPLE_WEB_DEPLOY_SD
#include "driver/sdmmc_host.h"
#endif

#define MDNS_INSTANCE "esp home web server"

static const char *TAG = "GAME1";
static const char *TEST_TAG = "esp-test";

char position_string[41] = {};

TaskHandle_t task_handler = NULL;

#define TEST_ASSERT_MESSAGE( condition, ...)                                \
    if (condition)                                                          \
    {                                                                       \
        ESP_LOGI(TEST_TAG, "test passed");                                  \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        ESP_LOGE(TEST_TAG, __VA_ARGS__);                                    \
    }

esp_err_t start_rest_server(const char *base_path);

static void initialise_mdns(void)
{
    mdns_init();
    mdns_hostname_set(CONFIG_EXAMPLE_MDNS_HOST_NAME);
    mdns_instance_name_set(MDNS_INSTANCE);

    mdns_txt_item_t serviceTxtData[] = {
            {"board", "esp32"},
            {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}

#if CONFIG_EXAMPLE_WEB_DEPLOY_SEMIHOST
esp_err_t init_fs(void)
{
    esp_err_t ret = esp_vfs_semihost_register(CONFIG_EXAMPLE_WEB_MOUNT_POINT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register semihost driver (%s)!", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    return ESP_OK;
}
#endif

#if CONFIG_EXAMPLE_WEB_DEPLOY_SD
esp_err_t init_fs(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY); // CMD
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);  // D0
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);  // D1
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY); // D2
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY); // D3

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 4,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(CONFIG_EXAMPLE_WEB_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    /* print card info if mount successfully */
    sdmmc_card_print_info(stdout, card);
    return ESP_OK;
}
#endif

#if CONFIG_EXAMPLE_WEB_DEPLOY_SF
esp_err_t init_fs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_EXAMPLE_WEB_MOUNT_POINT,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}
#endif


void run_game() {
    while (1) {
        vTaskDelay(100/portTICK_PERIOD_MS);
        //GAME INITIALISATION
        bitboard bb = {0, 0, 0};
        bool game_over = false;
        int turn = HUMAN;

        while (!game_over) {
            switch (turn) {
                // HUMAN INPUT
                case HUMAN:
                    /**
                     * PLACE FOR GETTING SIGNALS
                     * FROM SENSORS
                     */
                     char moveH = '2';
                    play(&bb, 2);
                    strncat(position_string,&moveH,1);
                    game_over = check_win(bb.position);
                    if (game_over) {
                        TEST_ASSERT_MESSAGE(false,"game won by human");
                        vTaskDelete(task_handler);
                        //initialise_bitboard(&bb);
                    } else {
                        turn = COMPUTER;
                    }
                    break;
                case COMPUTER:
                    // move next_move = negamax_ab_bb(bb,UINT64_MIN,UINT64_MAX,20);
                    /**
                     * PLACE FOR SENDING SIGNAL
                     * TO THE SERVOMOTORS
                     */
                    // play(&bb,next_move.col);
                    char moveC = '4';
                    play(&bb, 4);
                    strncat(position_string,&moveC,1);
                    // SEND MESSAGE
                    game_over = check_win(bb.position);
                    if (game_over) {
                        TEST_ASSERT_MESSAGE(false,"game won by computer");
                        vTaskDelete(task_handler);
                        //initialise_bitboard(&bb);
                    } else {
                        turn = HUMAN;
                    }
                    break;
                default:
                    TEST_ASSERT_MESSAGE(false, "something went wrong, shouldn't get to default");
            }
        }
    }
}

void app_main(void)
{
    initNvs();
    setupWifi();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initialise_mdns();
    netbiosns_init();
    netbiosns_set_name(CONFIG_EXAMPLE_MDNS_HOST_NAME);

    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(init_fs());
    ESP_ERROR_CHECK(start_rest_server(CONFIG_EXAMPLE_WEB_MOUNT_POINT));

    xTaskCreate(run_game,"GAME",4096,NULL,10,&task_handler);

}