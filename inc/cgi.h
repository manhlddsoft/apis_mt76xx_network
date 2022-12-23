#include "cJSON.h"
#include "common.h"
#include "crc16.h"

#define HAL_UART_1 1

typedef enum {
    IS_STRING = 10,
    IS_NUMBER,
} KEY_VALUE_STATUS;
typedef enum {
    FUNC_OK = 0,
    API_OK = 0,
} API_STATUS;

typedef enum {
    CMD_WRONG_FORMAT = 400,
    CMD_INVALID_URL,
} CLIENT_ERRORS;

typedef int (*CGI_SEND)(int sd, const char *buff, size_t len);

struct cgi_para {
    int sd;         // socket id
    CGI_SEND func;  // send function
    char *cmd;      // cmd
    int cmd_len;    // cmd length
    bool need_restart;
    char *uart_recv;  // data recv from T31
    char *uart_send;  // data send to T31
    char *APIs_send;  // data send to client/mobile app
    char *APIs_recv;  // data recv from client/mobile app
};

typedef int (*CGI_FUNC)(struct cgi_para *para);

struct cgi {
    char *name;
    CGI_FUNC func;
};

typedef struct cJSON_Value {
    char *string_val;
    int number_val;
} cJSON_Value;

CGI_FUNC cgi_filename_match(char *path_info);

int getKeyValue(cJSON *json, char *obj, cJSON_Value *val);

/**
 * @brief Get the Device Info object
 *
 * @param para
 * @return int
 */
static int getDeviceInfo(struct cgi_para *para);

static int configWifi(struct cgi_para *para);

static int configStream(struct cgi_para *para);

static int setStream(struct cgi_para *para);

static int getStreamStatus(struct cgi_para *para);

static int getVideoInfo(struct cgi_para *para);

static int factoryReset(struct cgi_para *para);
