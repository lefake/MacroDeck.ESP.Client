#include "ClientHttp.h"

ClientHttp::ClientHttp(String ip, String port)
{
    serverURL = "http://" + ip + ":" + port;
}

uint16_t ClientHttp::httpGETRequest(String uri, String* response)
{
    HTTPClient http;
    WiFiClient client;
 
    http.setTimeout(HTTP_REQUEST_TIMEOUT);
    http.begin(client, serverURL + uri);
    int code = http.GET();

    if (code > 0)
        *response = http.getString();

    client.stop();
    http.end();

    //Serial.println(code);

    if (code == 200)
        return OK;
    else if (code == HTTPC_ERROR_READ_TIMEOUT)
        return HTTP_TIMEOUT_ERROR; 
    else if (code == HTTPC_ERROR_CONNECTION_REFUSED)
        return HTTP_REFUSED;
    else
        return HTTP_GET_ERROR;
}

uint16_t ClientHttp::httpPOSTRequest(String uri, String body)
{
    HTTPClient http;
    WiFiClient client;

    http.setTimeout(HTTP_REQUEST_TIMEOUT);
    http.begin(client, serverURL + uri);
    int code = http.POST(body);

    client.stop();
    http.end();
    

    if (code == 200)
        return OK;
    else if (code == HTTPC_ERROR_READ_TIMEOUT)
        return HTTP_TIMEOUT_ERROR; 
    else if (code == HTTPC_ERROR_CONNECTION_REFUSED)
        return HTTP_REFUSED;
    else
        return HTTP_POST_ERROR;
}