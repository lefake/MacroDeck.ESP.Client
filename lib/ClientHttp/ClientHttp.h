#ifndef CLIENT_HTTP_H_
#define CLIENT_HTTP_H_

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ErrorCode.h"
#include "Constants.h"

class ClientHttp
{
private:
    String serverURL;
    
public:
    ClientHttp(String ip, String port);

    uint16_t httpGETRequest(String uri, String* response);
    uint16_t httpPOSTRequest(String uri, String body);
};


#endif 