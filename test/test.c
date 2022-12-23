#include "gtest/gtest.h"
#include "cgi.h"

TEST(test_all, test_configStream){
    char *cmd = "{\"url\":\"control\",\"targetFPS\":30,\"resolution\":\"HD\",\"deviceName\":\"XSTREAM\",\"sleep\":\"on\"}";
    struct cgi_para para;

    EXPECT_EQ(configStream(&para))
}

int main(){
    return 0;
}
