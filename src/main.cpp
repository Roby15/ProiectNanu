#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <DHT.h>
#include <ESP_Mail_Client.h>
#include <Preferences.h>
#include <time.h>
#include <ArduinoJson.h>

Preferences prefs;
unsigned long Id = 0;
const char* ssid = "Network";
const char* pass = "Password";
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL    "trifasrobert0615@gmail.com"
#define AUTHOR_PASSWORD "pfjb zzrq erdb hzff"
#define RECIPIENT_EMAIL "fskpeter2@gmail.com"
SMTPSession smtp;
SMTP_Message message;
#define Led_Pin 2
#define Button_pin 5
int payloadIndex = 0;

using namespace websockets;
WebsocketsClient client;
const char* azure_root_ca = R"EOF(-----BEGIN CERTIFICATE-----
MIIJsDCCB5igAwIBAgITMwGnUApYzCXzm7PrtwAAAadQCjANBgkqhkiG9w0BAQwF
ADBdMQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
MS4wLAYDVQQDEyVNaWNyb3NvZnQgQXp1cmUgUlNBIFRMUyBJc3N1aW5nIENBIDAz
MB4XDTI1MDIwMTE4MzkwN1oXDTI1MDczMTE4MzkwN1owajELMAkGA1UEBhMCVVMx
CzAJBgNVBAgTAldBMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3Nv
ZnQgQ29ycG9yYXRpb24xHDAaBgNVBAMMEyouYXp1cmV3ZWJzaXRlcy5uZXQwggEi
MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCoduj8DTt2bZJRBcSK321uGE4O
KXg1Od5IDYD6hi0q93uul4W/TCKoMDA2EmNkoQg6lXJ6j8+Ibkn6Q2qA3szxYZSC
srmGVPhoUe+V6alo2kHKfBgUYeSQqXTE6HTRYTfHrApojqobl6mQSCmCqmpreRCZ
8Hs8eSrQBALaDvw6ZigAHc18UvPFzqKNY1NfCkVDeerCgUGB8+9hZ4ilRaV744ZO
3fVzz33ffRxwc7X/zWPVlDrSGzIp5Z39XxTpZfKVrNCIFQ1hoL4zCWCc1HcT2DBc
SU8nSGgCBBYg3eZ7cKdtGQ8LzE8ICknUyX2ebESXKKr73AsRvotkpzNN4p1dAgMB
AAGjggVaMIIFVjCCAYAGCisGAQQB1nkCBAIEggFwBIIBbAFqAHYA3dzKNJXX4RYF
55Uy+sef+D0cUN/bADoUEnYKLKy7yCoAAAGUwthqxgAABAMARzBFAiEAg7kDiA+I
gHDHoYxLxygU7gYaYku6iEc1PabPzSyjS3gCIANxwOdDBe31PE2G2eb/Rp/8OMtX
nUf70HLSbsH9Kq2KAHcAfVkeEuF4KnscYWd8Xv340IdcFKBOlZ65Ay/ZDowuebgA
AAGUwthqbQAABAMASDBGAiEAysEn3hPubNQIF+HZAe5nOEEHUbYpHDc3DNs7K/nD
xQoCIQDlTk61kyrdOuQfHwwYpsuGJ1tSkr/w47v47DrVuoOIcwB3ABoE/0nQVB1A
r/agw7/x2MRnL07s7iNAaJhrF0Au3Il9AAABlMLYau8AAAQDAEgwRgIhAOG71feT
YwRfXzku6SO517fMKiO1Q7vlxtOoOGAG/MIAAiEA2eIxQgu0V2QZMTjiPQqrQtxn
T39eLL/qwa+ihoODO1UwJwYJKwYBBAGCNxUKBBowGDAKBggrBgEFBQcDAjAKBggr
BgEFBQcDATA8BgkrBgEEAYI3FQcELzAtBiUrBgEEAYI3FQiHvdcbgefrRoKBnS6O
0AyH8NodXYKr5zCH7fEfAgFkAgEtMIG0BggrBgEFBQcBAQSBpzCBpDBzBggrBgEF
BQcwAoZnaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9jZXJ0cy9NaWNy
b3NvZnQlMjBBenVyZSUyMFJTQSUyMFRMUyUyMElzc3VpbmclMjBDQSUyMDAzJTIw
LSUyMHhzaWduLmNydDAtBggrBgEFBQcwAYYhaHR0cDovL29uZW9jc3AubWljcm9z
b2Z0LmNvbS9vY3NwMB0GA1UdDgQWBBSrD+MP5qOfxk5NyFMNUeIgnX/7sTAOBgNV
HQ8BAf8EBAMCBaAwggFfBgNVHREEggFWMIIBUoITKi5henVyZXdlYnNpdGVzLm5l
dIIXKi5zY20uYXp1cmV3ZWJzaXRlcy5uZXSCFyouc3NvLmF6dXJld2Vic2l0ZXMu
bmV0giEqLndlc3RldXJvcGUtMDEuYXp1cmV3ZWJzaXRlcy5uZXSCJSouc2NtLndl
c3RldXJvcGUtMDEuYXp1cmV3ZWJzaXRlcy5uZXSCJSouc3NvLndlc3RldXJvcGUt
MDEuYXp1cmV3ZWJzaXRlcy5uZXSCICoud2VzdGV1cm9wZS5jLmF6dXJld2Vic2l0
ZXMubmV0giQqLnNjbS53ZXN0ZXVyb3BlLmMuYXp1cmV3ZWJzaXRlcy5uZXSCJCou
c3NvLndlc3RldXJvcGUuYy5henVyZXdlYnNpdGVzLm5ldIISKi5henVyZS1tb2Jp
bGUubmV0ghYqLnNjbS5henVyZS1tb2JpbGUubmV0MAwGA1UdEwEB/wQCMAAwagYD
VR0fBGMwYTBfoF2gW4ZZaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9j
cmwvTWljcm9zb2Z0JTIwQXp1cmUlMjBSU0ElMjBUTFMlMjBJc3N1aW5nJTIwQ0El
MjAwMy5jcmwwZgYDVR0gBF8wXTBRBgwrBgEEAYI3TIN9AQEwQTA/BggrBgEFBQcC
ARYzaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9Eb2NzL1JlcG9zaXRv
cnkuaHRtMAgGBmeBDAECAjAfBgNVHSMEGDAWgBT+CXFAVQUQRNikgXW4nhrpSgaI
yDAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwDQYJKoZIhvcNAQEMBQAD
ggIBAHe5d7RSxX0F4igGuEmL9uTh+lB8FaFh0kB+vy0DDvEWGZFwHhr6GvhIN55f
y4s0DW6ZA1Mucmp9gT2ioyk+Dvf4f0t+asx/c7eLT5buZvdvn966vaZrP8MmJjP9
iPsjy5Y1IygzjSwrvVtebqMxATyd1RU/cE14lQ8S55nQRtLM5f0ul10AXflFC7L9
12L4/fhREMpMmA6osloKE83NP5v7ArIr+gwjeUXbYQF0abpFE6UwgO8WJ655OGKq
MCBtJF4LZ7anXcHbzQlA7m9x1bqCrxpdjM73yH1CFW1Z8vYZ903NuJ29fzmZ27cT
crz/MKk+BWwITEf7jzkm6J/J94506YvCIUOxiIfukKHRVSoNSVwggi6+i0tACChb
xVlLHP8vFqnvp8llkJgb+jIzRUQlD7SHRelJDxYwJdm/Jtq+5t1wvuUSVa2jdGvL
g/yD+kIo3lBeHn5QyIoi8vpADY09aIy5IwrEBW6w6fTPdctkieZf17sCcBNRx7nA
lZn7mShZq0nd+AG933+wQ0+x4fyz6+O1uwOvBaLMmz/O/Ta3vKlM1NI5CjOfE7tt
Y+LU09ALCjkLnSPXc+g0MG3LENjUPB6lT13WvMOXv6+ITkLLKW1EkWO4juznZqAY
pYwHhCmvogYSivZhEIy9tRAx8wakiYof8thDccMTfRCPh4qv
-----END CERTIFICATE-----
)EOF";

void smtpCallback(SMTP_Status status) {
    Serial.println(status.info());
    if (status.success()) {
        Serial.println("----------------");
        MailClient.printf("Message sent success: %d\n", status.completedCount());
        MailClient.printf("Message sent failed: %d\n", status.failedCount());
        Serial.println("----------------\n");
        for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
            SMTP_Result result = smtp.sendingResult.getItem(i);
            MailClient.printf("Message No: %d\n", i + 1);
            MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
            MailClient.printf("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
            MailClient.printf("Recipient: %s\n", result.recipients.c_str());
            MailClient.printf("Subject: %s\n", result.subject.c_str());
        }
        Serial.println("----------------\n");
        smtp.sendingResult.clear();
    }
}

void sendAlertEmail() {
    smtp.debug(1);
    smtp.callback(smtpCallback);
    message.sender.name = F("ESP Mail");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = F("Alerta!");
    message.addRecipient(F("Someone"), RECIPIENT_EMAIL);
    String textMsg = "Alerta! Senzorul de inundatie a detectat o inundatie!";
    message.text.content = textMsg;
    message.text.charSet = F("us-ascii");
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    Session_Config config;
    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;
    if (!smtp.connect(&config)) {
        MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
        return;
    }
    if (!smtp.isLoggedIn()) {
        Serial.println("Error, Not yet logged in.");
    } else {
        if (smtp.isAuthenticated())
            Serial.println("Successfully logged in.");
        else
            Serial.println("Connected with no Auth.");
    }
    if (!MailClient.sendMail(&smtp, &message))
        MailClient.printf("Error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    MailClient.printf("Free Heap: %d\n", MailClient.getFreeHeap());
}

void webSocketEvent(const char* payload, size_t length) {
    // No switch(type) needed with ArduinoWebsockets, just handle text messages
    Serial.print("WebSocket message: ");
    Serial.println(payload);

    if (strcmp((char*)payload, "LED_ON") == 0) {
        digitalWrite(Led_Pin, HIGH);
    } else if (strcmp((char*)payload, "LED_OFF") == 0) {
        digitalWrite(Led_Pin, LOW);
    } else if (strcmp((char*)payload, "Inundatie") == 0) {
        StaticJsonDocument<2048> doc;
        JsonArray arr = doc.to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            String key = "payload";
            key.concat(i);
            String storedPayload = prefs.getString(key.c_str(), "");
            if (storedPayload.length() > 0) {
                StaticJsonDocument<256> entry;
                DeserializationError error = deserializeJson(entry, storedPayload);
                if (!error) {
                    arr.add(entry.as<JsonObject>());
                }
            }
        }
        String jsonArray;
        serializeJson(arr, jsonArray);
        client.send(jsonArray);
        Serial.println(jsonArray);
    } else if (strncmp((char*)payload, "DELETE_INUNDATIE:", 17) == 0) {
        int idToDelete = atoi((char*)payload + 17);
        for (int i = 0; i < 10; i++) {
            String key = "payload";
            key.concat(i);
            String storedPayload = prefs.getString(key.c_str(), "");
            if (storedPayload.length() > 0) {
                String idField1 = "\"Id\":"; idField1.concat(idToDelete);
                String idField2 = "\"Id\": "; idField2.concat(idToDelete);
                if (storedPayload.indexOf(idField1) != -1 || storedPayload.indexOf(idField2) != -1) {
                    prefs.remove(key.c_str());
                    Serial.printf("Deleted payload with Id %d at key %s\n", idToDelete, key.c_str());
                }
            }
        }
    }
}

void ConnectToWifi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Wifi Connected!");
    Serial.println("Ip Address:");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(9600);
    dht.begin();
    pinMode(Led_Pin, OUTPUT);
    pinMode(Button_pin, INPUT);
    ConnectToWifi();
    

    client.setCACert(azure_root_ca); // Ignore SSL cert errors (for testing, remove for production)

    client.onMessage([](websockets::WebsocketsMessage message) {
        webSocketEvent(message.data().c_str(), message.length());
    });

    bool connected = client.connect("ws://homeaapi.azurewebsites.net/ws");
    if (connected) {
        Serial.println("WebSocket Connected!");
    } else {
        Serial.println("WebSocket Connection Failed!");
    }

    prefs.begin("my-app", false);
    Id = prefs.getULong("lastId", 0);
    payloadIndex = prefs.getInt("payloadIndex", 0);
    setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
    tzset();
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    while (time(nullptr) < 100000) {
        delay(100);
    }
}

unsigned long lastButtonTime = 0;
unsigned long lastTime = 0;

void loop() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char timeString[25];
    strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    
    client.poll();

    // Only proceed if WebSocket is connected
    if (!client.available()) {
        // Optionally print a message once or blink an LED to indicate disconnected
        delay(500);
        Serial.println("WebSocket not available, waiting...");
        client.connect("ws://homeaapi.azurewebsites.net/ws");
        return;
    }

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    bool led = digitalRead(Led_Pin);
    bool button = digitalRead(Button_pin);

    StaticJsonDocument<256> doc;
    doc["Id"] = Id;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["Inundatie"] = button;
    doc["LedState"] = led;
    doc["datetime"] = timeString;
    String JsonPayload;
    serializeJson(doc, JsonPayload);

    if ((millis() - lastTime) > 10000) {
        Id++;
        Serial.println(JsonPayload);
        client.send(JsonPayload);
        lastTime = millis();
        prefs.putULong("lastId", Id);
    }

    if (button == true && (millis() - lastButtonTime > 10000)) {
        sendAlertEmail();
        String key = "payload";
        key.concat(payloadIndex);
        prefs.putString(key.c_str(), JsonPayload);
        client.send(JsonPayload);

        payloadIndex = (payloadIndex + 1) % 10;
        prefs.putInt("payloadIndex", payloadIndex);

        lastButtonTime = millis();
    }

    delay(500);
}