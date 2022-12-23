#include "cgi.h"
#include "common.h"

void protocolSend(struct cgi_para *para){
    printf("Send back to client: %s\n", para->APIs_send);
}


static int proccgi_lwip(int socket_id, char *cmd) {
    struct cgi_para para;
    CGI_FUNC cgi_func;

    cJSON *json;
    json = cJSON_Parse(cmd);

    if (!json) {
        cJSON_Delete(json);
        return -1;
    } else {
        cJSON_Value url;
        if (IS_STRING != getKeyValue(json, "url", &url)) {
            return -1;
        }
        LOG_I("url value: [%s] [%d]\n", url.string_val, url.number_val);
        cgi_func = cgi_filename_match(url.string_val);
        if (NULL == cgi_func){
            return -1;
        }
        para.sd = socket_id;
        para.APIs_recv = cmd;
        para.need_restart = false;
        cgi_func(&para);
        // protocolSend(&para);
    }
    free(para.APIs_send);
    cJSON_Delete(json);
    return FUNC_OK;
}

int main() {
    int c = 1;
    while (1) {
        char *cmd = "{\"url\":\"control\",\"targetFPS\":30,\"resolution\":\"HD\",\"deviceName\":\"XSTREAM\",\"sleep\":\"on\"}";
        char *cmd_2 = "{\"url\":qwe}";
        LOG_I("Client request: %s\n", cmd);
        proccgi_lwip(c, cmd);
        break;
    }
    return 0;
}
