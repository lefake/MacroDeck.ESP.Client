#include "ClientHttp.h"

ClientHttp::ClientHttp(String ip, String port)
{
    serverURL = "http://" + ip + ":" + port;
}

bool ClientHttp::httpGETRequest(String uri, String* response)
{
    HTTPClient http;
    WiFiClient client;
 
    http.begin(client, serverURL + uri);
    int code = http.GET();

    if (code > 0)
        *response = http.getString();

    client.stop();
    http.end();

    return code > 0;
}

bool ClientHttp::httpPOSTRequest(String uri, String body)
{
    HTTPClient http;
    WiFiClient client;

    http.begin(client, serverURL + uri);
    bool ret = http.POST(body) > 0;

    client.stop();
    http.end();

    return ret;
}