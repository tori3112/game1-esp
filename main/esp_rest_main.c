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

#define TEST_ASSERT_MESSAGE( condition, ...)                        \
    if (condition)                                                          \
    {                                                                       \
        ESP_LOGI(TEST_TAG, "test passed");                                  \
    }                                                                       \
    else                                                                    \
    {                                                                                 \
        ESP_LOGE(TEST_TAG, __VA_ARGS__);               \
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

void test_WinMove(int *myBoard) {
    //1ST TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 2, HUMAN);
    TEST_ASSERT_MESSAGE(check_vertical(myBoard, 2,HUMAN)==true,
                "1st test: move makes vertical four");
    //2ND TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 2, HUMAN);
    TEST_ASSERT_MESSAGE(check_vertical(myBoard,2, HUMAN)==false,
                "2nd test: move doesn't make vertical four");
    //3RD TEST
    initializeBoard(myBoard);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,5,HUMAN); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,3,HUMAN);
    TEST_ASSERT_MESSAGE(check_vertical(myBoard,3,COMPUTER)==false,
                        "3rd test: move doesn't make vertical four");
    //4TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 1, HUMAN);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 3, HUMAN);
    TEST_ASSERT_MESSAGE(check_horizontal(myBoard,4,HUMAN) == true,
                        "4th test: move makes horizontal four");
    //5TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 1, HUMAN);
    add_coin(myBoard, 2, COMPUTER);
    add_coin(myBoard, 3, HUMAN);
    TEST_ASSERT_MESSAGE(check_horizontal(myBoard,4,HUMAN) == false,
                        "5th test: move doesn't make horizontal four");
    //6TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 1, HUMAN);
    add_coin(myBoard, 4, HUMAN);
    add_coin(myBoard, 3, HUMAN);
    TEST_ASSERT_MESSAGE(check_horizontal(myBoard,2,HUMAN) == true,
                        "6th test: move makes a horizontal four");
    //7TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 2, HUMAN); add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard, 2, HUMAN); add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard, 3, HUMAN); add_coin(myBoard, 4, COMPUTER);
    add_coin(myBoard, 2, HUMAN); add_coin(myBoard, 5, COMPUTER);
    add_coin(myBoard, 3, HUMAN);
    TEST_ASSERT_MESSAGE(check_horizontal(myBoard,6,COMPUTER)==true,
                        "7th test: move makes a horizontal four");
    //8TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,5,HUMAN); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,3,HUMAN);
    TEST_ASSERT_MESSAGE(check_horizontal(myBoard,3,COMPUTER)==false,
                        "8th test: move makes a horizontal four");
    //9th TEST
    initializeBoard(myBoard);
    add_coin(myBoard,0,HUMAN); add_coin(myBoard,0,COMPUTER);
    add_coin(myBoard,0,COMPUTER); add_coin(myBoard,0,HUMAN);
    add_coin(myBoard,1,COMPUTER); add_coin(myBoard,1,HUMAN);
    add_coin(myBoard,1,HUMAN); add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,2,HUMAN);
    TEST_ASSERT_MESSAGE(check_diagonal_negative(myBoard,3,HUMAN) == true,
                        "9th test: move makes diagonal (negative) four");
    //10TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,2,HUMAN); add_coin(myBoard,2,COMPUTER);
    add_coin(myBoard,2,COMPUTER); add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,3,COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,5,COMPUTER);
    TEST_ASSERT_MESSAGE(check_diagonal_negative(myBoard,3,HUMAN) == false,
                        "10th test: move doesn't make diagonal (negative) four");
    //11TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,5,HUMAN); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,3,HUMAN);
    TEST_ASSERT_MESSAGE(check_diagonal_negative(myBoard,3,COMPUTER)==false,
                        "11th test: move doesn't make diagonal (negative) four");
    //12TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,2,COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,4,COMPUTER); add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,2,COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,3,COMPUTER); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,5,COMPUTER); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,5,COMPUTER);
    TEST_ASSERT_MESSAGE(check_diagonal_positive(myBoard,5,COMPUTER) == true,
                        "12th test: move makes a diagonal (positive) four");
    //13TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,2,COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,4,COMPUTER); add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,2,COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,3,COMPUTER); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,5,COMPUTER); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,5,COMPUTER);
    TEST_ASSERT_MESSAGE(check_diagonal_positive(myBoard,1,COMPUTER) == true,
                        "13th test: move makes a diagonal (positive) four");
    //14TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,6,COMPUTER); add_coin(myBoard,6,COMPUTER);
    add_coin(myBoard,6,HUMAN); add_coin(myBoard,6,HUMAN);
    add_coin(myBoard,5,COMPUTER); add_coin(myBoard,5,COMPUTER);
    add_coin(myBoard,4,COMPUTER); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,3,HUMAN);
    TEST_ASSERT_MESSAGE(check_diagonal_positive(myBoard,5,HUMAN)==true,
                        "14th test: move makes a diagonal (positive) four");
    //15TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,3,HUMAN);
    TEST_ASSERT_MESSAGE(check_diagonal_positive(myBoard,3,COMPUTER)==false,
                        "15th test: move doesn't make a diagonal (positive) four");
    //16TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 2, HUMAN); add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard, 2, HUMAN); add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard, 3, HUMAN); add_coin(myBoard, 4, COMPUTER);
    add_coin(myBoard, 2, HUMAN); add_coin(myBoard, 5, COMPUTER);
    add_coin(myBoard, 3, HUMAN);
    TEST_ASSERT_MESSAGE(is_it_win_move(myBoard,6,COMPUTER)==true,
                        "16th test: move makes a diagonal (positive) four");
    //17TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,2,COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,3,HUMAN); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,4,HUMAN); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,5,COMPUTER);
    TEST_ASSERT_MESSAGE(check_four(myBoard)==false,
                        "17th test: check win should have failed");
    //18TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,0,COMPUTER); add_coin(myBoard,0,COMPUTER);
    add_coin(myBoard,0,HUMAN); add_coin(myBoard,1,COMPUTER);
    add_coin(myBoard,1,COMPUTER); add_coin(myBoard,1,HUMAN);
    add_coin(myBoard,1,COMPUTER); add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,2,HUMAN); add_coin(myBoard,2,COMPUTER);
    add_coin(myBoard,2,HUMAN); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,3,HUMAN); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,4,COMPUTER); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,4,COMPUTER); add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,5,COMPUTER); add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,6,HUMAN); add_coin(myBoard,6,COMPUTER);
    TEST_ASSERT_MESSAGE(check_four(myBoard)==false,
                        "18th test: check win should have failed");
}
void test_LittleGame(int *myBoard) {
    initializeBoard(myBoard);
    add_coin(myBoard,0,COMPUTER); add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,2,HUMAN); add_coin(myBoard,2,COMPUTER);
    add_coin(myBoard,4,COMPUTER); add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,5,HUMAN); add_coin(myBoard,6,COMPUTER);
    add_coin(myBoard,6,HUMAN);
    TEST_ASSERT_MESSAGE(can_add_coin(myBoard,4)==true,
                        "test 1: should be able to add coin into '4'");
    TEST_ASSERT_MESSAGE(is_it_win_move(myBoard,4,COMPUTER)==false,
                        "test 2: move into '4' does not lead to win");
    TEST_ASSERT_MESSAGE(negamax(myBoard,2,COMPUTER).score==MEDIUM_SCORE,
                        "test 3: expected score for depth '2' is '%d' but got %d",
                        MEDIUM_SCORE, negamax(myBoard,2,COMPUTER).score);
}

void test_Evaluation(int *myBoard) {
    //1ST TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 0, COMPUTER); add_coin(myBoard, 0, COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_vertical(myBoard) == MEDIUM_SCORE,
                        "1st test: different vertical evaluation ('%d' : %d).", MEDIUM_SCORE, evaluate_vertical(myBoard));
    add_coin(myBoard,0,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_vertical(myBoard) == HIGH_SCORE,
                        "1st test: different vertical evaluation ('%d' : %d).", HIGH_SCORE, evaluate_vertical(myBoard));
    //2ND TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 3, COMPUTER); add_coin(myBoard,3,HUMAN);
    add_coin(myBoard, 3, COMPUTER); add_coin(myBoard, 3, COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_vertical(myBoard) == MEDIUM_SCORE,
                        "2nd test: different vertical evaluation ('%d' : %d).", MEDIUM_SCORE,evaluate_vertical(myBoard));
    //3RD TEST
    initializeBoard(myBoard);
    add_coin(myBoard,0,COMPUTER);
    add_coin(myBoard,1,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_horizontal(myBoard) == MEDIUM_SCORE,
                        "3rd test: different horizontal evaluation ('%d' : %d).", MEDIUM_SCORE, evaluate_horizontal(myBoard));
    add_coin(myBoard,2,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_horizontal(myBoard) == HIGH_SCORE,
                        "3rd test: different horizontal evaluation ('%d' : %d).", HIGH_SCORE, evaluate_horizontal(myBoard));
    //4TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard,3,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_horizontal(myBoard) == 0,
                        "4th test: different horizontal evaluation ('0' : %d).", evaluate_horizontal(myBoard));
    //5TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,0,COMPUTER);
    add_coin(myBoard,1,HUMAN);
    add_coin(myBoard,1,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_positive_diagonal(myBoard) == MEDIUM_SCORE,
                        "5th test: different (positive) diagonal evaluation ('20' : %d).", evaluate_positive_diagonal(myBoard));
    add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,2,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_positive_diagonal(myBoard) == HIGH_SCORE,
                        "5th test: different (positive) diagonal evaluation ('50' : %d).", evaluate_positive_diagonal(myBoard));
    //6TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,0,COMPUTER);
    add_coin(myBoard,1,HUMAN);
    add_coin(myBoard,1,HUMAN);
    TEST_ASSERT_MESSAGE(evaluate_positive_diagonal(myBoard) == 0,
                        "6th test: different (positive) diagonal evaluation ('0' : %d).", evaluate_positive_diagonal(myBoard));
    //7TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,1,HUMAN);
    add_coin(myBoard,0,COMPUTER);
    add_coin(myBoard,2,COMPUTER);
    add_coin(myBoard,1,COMPUTER);
    add_coin(myBoard,1,COMPUTER);
    add_coin(myBoard,3,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_positive_diagonal(myBoard) == 20,
                        "7th test: different (positive) diagonal evaluation ('20' : %d).", evaluate_positive_diagonal(myBoard));
    //8TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,6,HUMAN);
    add_coin(myBoard,5,COMPUTER);
    add_coin(myBoard,5,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_negative_diagonal(myBoard) == 0,
                        "8th test: different (negative) diagonal evaluation ('0' : %d).", evaluate_negative_diagonal(myBoard));
    //9TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,5,COMPUTER);
    add_coin(myBoard,6,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate_negative_diagonal(myBoard) == 20,
                        "9th test: different (negative) diagonal evaluation ('20' : %d).", evaluate_negative_diagonal(myBoard));
    //10TH TEST
    initializeBoard(myBoard);
    add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,2,COMPUTER);
    add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,2,COMPUTER);
    TEST_ASSERT_MESSAGE(evaluate(myBoard) == 80,
                        "10th test: different overall evaluation ('80' : %d).", evaluate(myBoard));
}
void test_Negamax_Draw(int *myBoard) {
    //DOESN'T WORK HERE
    int board[] = {3,3,2,2,3,3,2,
                   3,2,3,3,2,2,3,
                   2,3,2,2,3,3,2,
                   3,2,3,2,2,2,3,
                   2,3,2,2,3,3,2,
                   0,2,3,3,3,2,0};
    copy_board(board,myBoard);
    TEST_ASSERT_MESSAGE(negamax(myBoard,3,HUMAN).score==0,"Negamax draw check failed, expected draw, got %d", negamax(myBoard,3,HUMAN).score);
}
void test_Negamax_WinMove(int *myBoard) {
    initializeBoard(myBoard);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 3, COMPUTER);
    add_coin(myBoard, 3, HUMAN);
    add_coin(myBoard, 4, COMPUTER);
    add_coin(myBoard, 2, HUMAN);
    add_coin(myBoard, 5, COMPUTER);
    add_coin(myBoard, 3, HUMAN);
    TEST_ASSERT_MESSAGE(negamax(myBoard,3,COMPUTER).score==WIN_SCORE,
                        "01: Expected negamax method to score WIN SCORE.");

    initializeBoard(myBoard);
    add_coin(myBoard,4,HUMAN);
    add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,5,HUMAN);
    add_coin(myBoard,4,COMPUTER);
    add_coin(myBoard,3,HUMAN);
    TEST_ASSERT_MESSAGE(check_vertical(myBoard,5,COMPUTER)==false,
                        "02: Vertical check fails.");
    TEST_ASSERT_MESSAGE(check_horizontal(myBoard,5,COMPUTER)==false,
                        "03: Horizontal check fails.");
    TEST_ASSERT_MESSAGE(check_diagonal_positive(myBoard,5,COMPUTER)==false,
                        "04: Diagonal positive check fails.");
    TEST_ASSERT_MESSAGE(check_diagonal_negative(myBoard,5,COMPUTER)==false,
                        "05: Diagonal negative check fails.");
    TEST_ASSERT_MESSAGE(negamax(myBoard,3,COMPUTER).score!=WIN_SCORE,
                        "06: Not expected to score WIN SCORE.");

}
void test_Negamax_Search(int *myBoard) {
    initializeBoard(myBoard);
    add_coin(myBoard,2,HUMAN);
    add_coin(myBoard,3,COMPUTER);
    add_coin(myBoard,3,HUMAN);
    add_coin(myBoard,4,COMPUTER);
    TEST_ASSERT_MESSAGE(negamax(myBoard,2,COMPUTER).score==-WIN_SCORE,
                        "test 1: expected search to give WIN in 2 moves: '110' instead of %d",
                        negamax(myBoard,2,COMPUTER).score);
    TEST_ASSERT_MESSAGE(negamax(myBoard,4,COMPUTER).col==5 || negamax(myBoard,4,COMPUTER).col==6,
                        "test 2:expected search to give col '5' or '6' instead of %d",
                        negamax(myBoard,4,COMPUTER).col);
    add_coin(myBoard, negamax(myBoard,4,COMPUTER).col,COMPUTER);

    TEST_ASSERT_MESSAGE(myBoard[5]==COMPUTER,
                        "test 3: incorrect add coin");
}
void app_main(void)
{
    int *myBoard = malloc(COLS*ROWS* sizeof(int));

    test_WinMove(myBoard);
    test_LittleGame(myBoard);
    test_Evaluation(myBoard);
    test_Negamax_Draw(myBoard);
    test_Negamax_WinMove(myBoard);
    test_Negamax_Search(myBoard);

    free(myBoard);

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