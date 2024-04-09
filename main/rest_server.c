/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_chip_info.h"
#include "esp_random.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"

static const char *REST_TAG = "esp-rest";
#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(REST_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* MY SWEAT AND STRUGGLES */
static esp_err_t game_post_handler(httpd_req_t *req) {
    int total_len = req->content_len;
    int cur_len = 0;
    char *buffer = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    // prevent reading data that exceeds size of the buffer
    if (total_len >= SCRATCH_BUFSIZE) {
        httpd_resp_send_err(req,HTTPD_500_INTERNAL_SERVER_ERROR,"content exceeds buffer size");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req,buffer+cur_len,total_len);
        if (received <= 0) {
            httpd_resp_send_err(req,HTTPD_500_INTERNAL_SERVER_ERROR,"did not post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    cJSON *root = cJSON_Parse(buffer);
    cJSON *item = cJSON_GetObjectItemCaseSensitive(root,"message");
    char *msg = item->valuestring;
    ESP_LOGI(REST_TAG, "Received data: %s", msg);
    httpd_resp_sendstr(req, "data received successfully");
    return ESP_OK;
}

static esp_err_t game_get_handler (httpd_req_t *req) {
    const char message[] = "game initialised";
    ESP_LOGI(REST_TAG, "Sent data: %s", message);
    httpd_resp_send(req,message, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
/* END OF MY STRUGGLES */

esp_err_t start_rest_server(const char *base_path)
{
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(REST_TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    httpd_uri_t game_post_uri = {
            .uri = "/*",
            .method = HTTP_POST,
            .handler = game_post_handler,
            .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &game_post_uri);

    httpd_uri_t game_get_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = game_get_handler,
            .user_ctx = rest_context
    };
    httpd_register_uri_handler(server,&game_get_uri);

    return ESP_OK;
    err_start:
    free(rest_context);
    err:
    return ESP_FAIL;
}