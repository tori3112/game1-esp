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

#if CONFIG_EXAMPLE_WEB_DEPLOY_SD
#include "driver/sdmmc_host.h"
#endif

#define MDNS_INSTANCE "esp home web server"

static const char *TAG = "GAME1";
static const char *TEST_TAG = "esp-test";

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

void test_initialisation(bitboard bb){
    initialise_bitboard(&bb);
    TEST_ASSERT_MESSAGE(bb.position==0,
                        "01: position after init should be 0 not %llu", bb.position);
    TEST_ASSERT_MESSAGE(bb.mask==0,
                        "02: mask after init should be 0 not %llu", bb.mask);
    TEST_ASSERT_MESSAGE(bb.no_moves==0,
                        "03: no of moves after init should be 0 not %d", bb.no_moves);
    bb.position=60;
    bb.mask=2652;
    bb.no_moves=6;
    TEST_ASSERT_MESSAGE(bb.position==60,
                        "04: position after assignment should be '60' not %llu", bb.position);
    TEST_ASSERT_MESSAGE(bb.mask==2652,
                        "05: mask after init should be '2652' no %llu", bb.mask);
    TEST_ASSERT_MESSAGE(bb.no_moves==6,
                        "06: no of moves after init should be '6' not %d", bb.no_moves);
    initialise_bitboard(&bb);
    TEST_ASSERT_MESSAGE(bb.position==0,
                        "07: position at the end is %llu",bb.position);
    TEST_ASSERT_MESSAGE(bb.mask==0,
                        "08: mask at the end is %llu",bb.position);
    TEST_ASSERT_MESSAGE(bb.no_moves==0,
                        "09: moves at the end is %d",bb.no_moves);
}
void test_can_play(bitboard bb) {
    initialise_bitboard(&bb);
    bb.mask = 138521415198;
    TEST_ASSERT_MESSAGE(can_play(bb.mask,2)==false,
                  "01: test expected to fail");
    TEST_ASSERT_MESSAGE(can_play(bb.mask,6)==true,
                  "02: test expected to pass");


    initialise_bitboard(&bb);
    bb.mask = 138521149956;
    TEST_ASSERT_MESSAGE(can_play(bb.mask,2)==false,
                  "03: test expected to fail");
    TEST_ASSERT_MESSAGE(can_play(bb.mask,1),
                  "04: test expected to pass");
    initialise_bitboard(&bb);
    bb.mask = 33884764;
    bb.position = 33557064;
    TEST_ASSERT_MESSAGE(can_play(bb.mask,2),
                  "05: test expected to pas");
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.mask==42273372,
                  "06: add coin into '4' failed, mask is %llu",bb.mask);
    TEST_ASSERT_MESSAGE(can_play(bb.mask,2),
                  "07: test expected to pas");
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.mask==1116015196,
                  "08: add coin into '4' failed");
    play(&bb,2);
    TEST_ASSERT_MESSAGE(can_play(bb.mask,2)==false,
                  "09: expected to fail\nMASK: %llu\nTOPC: %llu",bb.mask, top_cell(2));
    initialise_bitboard(&bb);
    bb.mask = 34630287489;
    bb.no_moves = 7;
    TEST_ASSERT_MESSAGE(can_play(bb.mask,0)==false,
                  "10: expected to fail");
}
void test_play(bitboard bb){
    initialise_bitboard(&bb);
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.no_moves==1,
                  "01: the first move was supposed to be made");
    TEST_ASSERT_MESSAGE(bb.mask==4,
                  "02: the first move produced wrong mask");
    TEST_ASSERT_MESSAGE(bb.position==4,
                  "03: the first move produced wrong position");
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.mask==516,
                  "04: fault after move 2, mask is %llu",bb.mask);
    TEST_ASSERT_MESSAGE(bb.position==512,
                  "05: fault after move 2, position is %llu",bb.position);
    TEST_ASSERT_MESSAGE(bb.no_moves==2,
                  "06: fault after move 2, wrong no of moves %d",bb.no_moves);
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.no_moves==3,
                  "07: fault after move 3, wrong no of moves %d",bb.no_moves);
    TEST_ASSERT_MESSAGE(bb.position==65540,
                  "08: fault after move 3 position is %llu",bb.position);
    TEST_ASSERT_MESSAGE(bb.mask==66052,
                  "09: fault after move 3, mask should be %llu",bb.mask);
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.mask==8454660,
                  "10: fault after move 4");
    TEST_ASSERT_MESSAGE(bb.position==8389120,
                  "11: fault after move 4");
    play(&bb,2);
    TEST_ASSERT_MESSAGE(bb.mask==1082196484,
                  "12: expected different mask");
    TEST_ASSERT_MESSAGE(bb.no_moves==5,
                  "13: expected to make '5' moves but got %d",bb.no_moves);
    TEST_ASSERT_MESSAGE(bb.position==1073807364,
                  "14: expected different position, %llu",bb.position);
    TEST_ASSERT_MESSAGE(can_play(bb.mask,5),
                  "15: move into 5 should be allowed");
    play(&bb,5);
    TEST_ASSERT_MESSAGE(bb.no_moves==6,
                  "16: there should be 6 moves");
    TEST_ASSERT_MESSAGE(bb.position==8389152,
                  "17: expected different position, got %llu",bb.position);
    TEST_ASSERT_MESSAGE(bb.mask==1082196516,
                  "18: expected different mask, got %llu",bb.mask);

}
void test_check_win(bitboard bb){
    initialise_bitboard(&bb);
    bb.position = 0b000000000010000001000000100000010000000000;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==true,
                  "01: expected vertical check to be true");
    initialise_bitboard(&bb);
    bb.position = 0b000000000000000000000000000001111000000000;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==true,
                  "02: expected horizontal check to be true");
    initialise_bitboard(&bb);
    bb.position = 0b000000000000000000010000010000010000010000;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==true,
                  "03: expected positive check to be true");
    initialise_bitboard(&bb);
    bb.position = 0b000000000000000100000001000000010000000100;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==true,
                  "04: expected negative check to be true");
    initialise_bitboard(&bb);
    bb.position = 0b000000000000000000000001000000010000000100;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==false,
                  "05: expected negative check to be fail");
    initialise_bitboard(&bb);
    bb.position = 0b000000000000000000000000000000000000111100;
    bb.mask = 0b000000000000000000000000000000100001111110;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==true,
                  "06: expected to find a four");
    initialise_bitboard(&bb);
    bb.mask=0b000000000000000001000100100010010001001000;
    bb.position=0b000000000000000001000000100000010000001000;
    TEST_ASSERT_MESSAGE(check_win(bb.position)==true,
                  "07: expected to find a four");
}
void test_is_win(bitboard bb){
    initialise_bitboard(&bb);
    play(&bb,3); play(&bb,2);
    play(&bb,4);
    TEST_ASSERT_MESSAGE(is_win(bb,5)==false,
                  "01: move into '5' should produce a win");
    initialise_bitboard(&bb);
    bb.mask = 0b000000000000000001000000100000110000111010;
    bb.position = 0b000000000000000001000000000000110000010000;
    TEST_ASSERT_MESSAGE(is_win(bb,4)==false,
                  "02: move into '4' should not produce a win");
    initialise_bitboard(&bb);
    bb.position = 0b000000000000110000011000100000010110000001;
    bb.mask = 0b000000100000110001011000101100010111101111;
    TEST_ASSERT_MESSAGE(is_win(bb,1)==false,
                  "03: move into '1' should not produce a win");
    TEST_ASSERT_MESSAGE(is_win(bb,2),
                  "04: move into '2' should produce a win");
}
void test_evaluate(bitboard bb){
    initialise_bitboard(&bb);
    bb.position=16780304;
    bb.mask=16911418;
    bb.no_moves=8;
    TEST_ASSERT_MESSAGE(evaluate_bb(bb.position)==60,
                  "01: evaluation failed got %d",
                  evaluate_bb(bb.position));
}
void test_negamax(bitboard bb){
    initialise_bitboard(&bb);
    bb.mask=132233;
    bb.position=132104;
    bb.no_moves=5;
    TEST_ASSERT_MESSAGE(negamax_ab_bb(bb,INT_MAX,INT_MIN,10).score==WIN_SCORE,
                  "01: expected score to be '%d', got %d",-WIN_SCORE,
                  negamax_ab_bb(bb,INT_MAX,INT_MIN,10).score);
    TEST_ASSERT_MESSAGE(negamax_ab_bb(bb,INT_MAX,INT_MIN,10).col==3,
                  "02: expected column output to be '%d', got %d",3,
                  negamax_ab_bb(bb,INT_MAX,INT_MIN,10).col);
    initialise_bitboard(&bb);
    bb.position=16780304;
    bb.mask=16911418;
    bb.no_moves=8;
    TEST_ASSERT_MESSAGE(negamax_ab_bb(bb,INT_MAX,INT_MIN,0).score==60,
                  "03: expected score to be '60' but got %d",
                  negamax_ab_bb(bb,INT_MAX,INT_MIN,0).score);
}

void app_main(void)
{
    bitboard bb = {0,0,0};
    test_initialisation(bb);
    test_can_play(bb);
    test_play(bb);
    test_check_win(bb);
    test_is_win(bb);
    test_evaluate(bb);
    test_negamax(bb);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initialise_mdns();
    netbiosns_init();
    netbiosns_set_name(CONFIG_EXAMPLE_MDNS_HOST_NAME);

    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(init_fs());
    ESP_ERROR_CHECK(start_rest_server(CONFIG_EXAMPLE_WEB_MOUNT_POINT));
}