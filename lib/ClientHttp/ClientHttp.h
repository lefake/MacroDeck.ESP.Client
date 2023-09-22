#ifndef CLIENT_HTTP_H_
#define CLIENT_HTTP_H_

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

class ClientHttp
{
private:
    String serverURL;
    
public:
    ClientHttp(String ip, String port);

    bool httpGETRequest(String uri, String* response);
    bool httpPOSTRequest(String uri, String body);
};


#endif 