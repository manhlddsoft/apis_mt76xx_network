#include "cgi.h"

char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}

const struct cgi cgi_cmds[] = {
    {"info", &getDeviceInfo},
    {"wifi", &configWifi},
    {"stream", &setStream},
    {"status", &getStreamStatus},
    {"control", &configStream},
    {"factoryReset", &factoryReset},
    {"videoInfo", &getVideoInfo}};

CGI_FUNC cgi_filename_match(char *path_info) {
    int i;

    // LOG_I(lwip_socket_example, "[HTTPD] cgi_filename_match() - path_info = %s\n", path_info);

    for (i = 0; i < (sizeof(cgi_cmds) / sizeof(struct cgi)); i++) {
        if (!strcmp(cgi_cmds[i].name, path_info)) {
            // LOG_I(common, "Function name: %s\n", cgi_cmds[i].name);
            return cgi_cmds[i].func;
        }
    }
    // LOG_I(common, "Path info name: %s\n", path_info);
    return NULL;
}

void addToResponseJSON(cJSON *json_file, const char *object, const char *item) {
    cJSON *new_item;
    new_item = cJSON_CreateString(item);

    cJSON_AddItemToObject(json_file, object, new_item);
}

void jsonAdd_NumberItemToObj(cJSON *json_file, const char *object, const int number) {
    cJSON *new_item;
    new_item = cJSON_CreateNumber(number);

    cJSON_AddItemToObject(json_file, object, new_item);
}

int getKeyValue(cJSON *json, char *obj, cJSON_Value *val) {
    if (cJSON_HasObjectItem(json, obj)) {
        if (cJSON_IsString(cJSON_GetObjectItem(json, obj))) {
            val->string_val = cJSON_GetStringValue(cJSON_GetObjectItem(json, obj));
            val->number_val = 0;
            LOG_I("With key %s, value is: %s\n", obj, val->string_val);
            return IS_STRING;
        }
        if (cJSON_IsNumber(cJSON_GetObjectItem(json, obj))) {
            val->number_val = cJSON_GetObjectItem(json, obj)->valueint;
            LOG_I("With key %s, value is: %d\n", obj, val->number_val);
            return IS_NUMBER;
        }
    } else {
        val->number_val = 0;
        printf("json has no obj %s\n", obj);
        return -1;
    }
    return -1;
}

cJSON *prepareJsonResponse(struct cgi_para *para) {
    cJSON *temp = cJSON_CreateObject();

    cJSON *recv_cmd = cJSON_Parse(para->APIs_recv);
    cJSON_Value url;
    getKeyValue(recv_cmd, "url", &url);
    addToResponseJSON(temp, "url", url.string_val);
    cJSON_Delete(recv_cmd);
    cJSON_AddNullToObject(temp, "status");
    return temp;
}

char *render_checkSum(char *json_file) {
    char *check_sum = malloc(8 * sizeof(char));
    if (NULL == check_sum) {
        printf("malloc check_sum failed\n");
        return NULL;
    }
    uint16_t crc = crc16(json_file, strlen(json_file));
    sprintf(check_sum, "*%x\r\n", crc);
    return check_sum;
}

static int getDeviceInfo(struct cgi_para *para) {
    cJSON *response_json = prepareJsonResponse(para);

    cJSON *data = cJSON_CreateObject();

    addToResponseJSON(data, "productName", "hehehehe");
    addToResponseJSON(data, "productCode", "heheheh123123e");

    cJSON_AddItemReferenceToObject(response_json, "data", data);

    char *temp = cJSON_Print(response_json);
    printf("%s\n", temp);

    cJSON_free(temp);
    cJSON_Delete(data);
    cJSON_Delete(response_json);
    return API_OK;
}

static int configWifi(struct cgi_para *para) {
    return API_OK;
}

int uartReceive(char *temp){
    strcpy(temp, "{\"status\":\"SUCCESS\"}");
}

int hal_uart_send_dma(int x,char *send, int lenght){
    return strlen(send);
}

void lwip_write(int x, char *temp, int len){
    LOG_I("lwip_write send: %s", temp);
}

static int configStream(struct cgi_para *para) {
    cJSON *recv_json;
    recv_json = cJSON_Parse(para->cmd);

    cJSON *response = cJSON_CreateObject();

    /* Handle data send to T31 */
    cJSON *uart = cJSON_CreateObject();
    cJSON *videoControl = cJSON_CreateObject();
    if(cJSON_HasObjectItem(recv_json, "resolution")){
        char *resolution = cJSON_GetStringValue(cJSON_GetObjectItem(recv_json, "resolution"));
        cJSON_AddStringToObject(videoControl, "resolution", resolution);
    }
    if(cJSON_HasObjectItem(recv_json, "targetFps")){
        int targetFps = cJSON_GetObjectItem(recv_json, "targetFps")->valueint;
        cJSON_AddNumberToObject(videoControl, "targetFps", targetFps);
    }
    cJSON_AddItemToObject(uart, "videoControl", videoControl);
    char *uart_send_buffer = (char *)malloc(sizeof(char) * 256);
    char *temp = cJSON_PrintUnformatted(uart);
    strcpy(uart_send_buffer, temp);
    cJSON_free(temp);
    char *checksum = render_checkSum(uart_send_buffer);
    strcat(uart_send_buffer, checksum);
    free(checksum);
    /* [End]Handle data send to T31 */

    /* Send to T31 */
    int snd_cnt = 0;
    snd_cnt = hal_uart_send_dma(HAL_UART_1, uart_send_buffer, strlen(uart_send_buffer));
    LOG_I(common, "MT7682 -> T31: [length %d] %s", snd_cnt, uart_send_buffer);
    /* [End]Send to T31*/

    /* Handle Receive data from T31 */
    char uart_recv_buffer[256] = {0};

    uartReceive(uart_recv_buffer);
    LOG_I(common, "T31 -> MT7682: [length %ld] %s", strlen(uart_recv_buffer), uart_recv_buffer);
    cJSON *uart_recv;

    uart_recv = cJSON_Parse(uart_recv_buffer);
    
    char *status;
    if(cJSON_HasObjectItem(uart_recv, "status")){
        status = cJSON_GetStringValue(cJSON_GetObjectItem(uart_recv, "status"));
        LOG_I(common, "T31 -> MT7682: status: %s", status);
    } else {
        status = "e1000";
        LOG_I(common, "uart_recv json has no key status");
    }
    cJSON_Delete(uart_recv);

    /* [End]Handle Receive data from T31 */
    cJSON *json_apis_response;
    json_apis_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_apis_response, "status", strlwr(status));
    char apis_response[256] = {0};

    char *temp2 = cJSON_PrintUnformatted(json_apis_response); 
    // printf("5 %s", temp2); // Expect {"status":"success"}
    strcpy(apis_response, temp2);
    // LOG_I(common, "6");
    free(temp2);
    // LOG_I(common, "7");
    lwip_write(para->sd, apis_response, strlen(apis_response));

    cJSON_Delete(uart);
    cJSON_Delete(recv_json);
    cJSON_Delete(response);
    cJSON_Delete(json_apis_response);
    // free(apis_response);
    free(uart_send_buffer);
    return API_OK;
}

static int setStream(struct cgi_para *para) {
    return API_OK;
}

static int getStreamStatus(struct cgi_para *para) {
    return API_OK;
}

static int getVideoInfo(struct cgi_para *para) {
    return API_OK;
}

static int factoryReset(struct cgi_para *para) {
    return API_OK;
}
