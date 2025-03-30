#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include <vector>

#define SS_1   5    // SDA cho module 1
#define RST_1  22   // RST cho module 1

#define SS_2   15   // SDA cho module 2
#define RST_2  32   // RST cho module 

#define BUZZER_PIN 16
#define BUZZER_PWM_CHANNEL 0
#define PWM_FREQ 2000     
#define PWM_RESOLUTION 8  

MFRC522 rfid1(SS_1, RST_1);
MFRC522 rfid2(SS_2, RST_2);

#define WIFI_SSID "SH3 P402"
#define WIFI_PASSWORD "studenthouse402"
String Web_App_URL = "https://script.google.com/macros/s/AKfycby39LrFo8b-_5IPUVA9CAbYRq7mIaVyy0rbNAshcoeBjR_zpd5hiRkgaF1UIBrvTjtU/exec";
#define MAX_EMPLOYEES 100
struct Employee {
    int id;
    char code[12];
    char name[30];
};

Employee employees[MAX_EMPLOYEES];
int employeeCount = 0;
String uidString;

unsigned long timeDelay1 = millis();
unsigned long timeDelay2 = millis();
unsigned long currentTime = 0;

std::vector<String> splitString(String data, char dellimiter){
    std::vector<String> result;
    int start = 0, end = 0;

    while((end = data.indexOf(dellimiter, start)) != -1){
        if(end > start) result.push_back(data.substring(start, end));
        start = end + 1;
    }

    String tailString = data.substring(start);
    if(tailString != ",") result.push_back(tailString);
    return result;
}

bool readDataSheet(){
    if (WiFi.status() == WL_CONNECTED) {
        // Create a URL for reading or getting data from Google Sheets.
        String Read_Data_URL = Web_App_URL + "?sts=read";

        Serial.println();
        Serial.println("-------------");
        Serial.println("Read data from Google Spreadsheet...");
        Serial.print("URL : ");
        Serial.println(Read_Data_URL);

        //::::::::::::::::::The process of reading or getting data from Google Sheets.
        HTTPClient http;

        // HTTP GET Request.
        http.begin(Read_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        // Gets the HTTP status code.
        int httpCode = http.GET(); 
        Serial.print("HTTP Status Code : ");
        Serial.println(httpCode);

        // Getting response from google sheet.
        String payload;
        employeeCount = 0;
        if (httpCode > 0) {
            payload = http.getString();
            //Serial.println("Payload : " + payload); 

            std::vector<String> data = splitString(payload, ','); 

            //Serial.println("Number of elements: "+String(data.size() - 1));
            while (employeeCount < data.size()/3) {
                employees[employeeCount].id = data[employeeCount * 3].toInt();
                strcpy(employees[employeeCount].code, data[employeeCount * 3 + 1].c_str());
                strcpy(employees[employeeCount].name, data[employeeCount * 3 + 2].c_str());
                employeeCount ++;
            }
            
            // for (int i = 0; i < employeeCount; i++) {
            //     Serial.print("ID: ");
            //     Serial.println(employees[i].id);
            //     Serial.print("Code: ");
            //     Serial.println(employees[i].code);
            //     Serial.print("Name: ");
            //     Serial.println(employees[i].name);
            // }
        }

        http.end();
        //::::::::::::::::::
        
        Serial.println("-------------");
        if(employeeCount > 0) return true;
        else return false;
    } 
    return false;
}

char* getEmployeeNameByUid(String uid) {
    for (int i = 0; i < employeeCount; i++) {
        if (strcmp(employees[i].code, uid.c_str()) == 0) {
            return employees[i].name;
        }
    }
    return nullptr; 
}

void beep(){
    ledcWrite(BUZZER_PWM_CHANNEL, 128);
    delay(200);
    ledcWrite(BUZZER_PWM_CHANNEL, 0);
}

void beepFail(){
    ledcWrite(BUZZER_PWM_CHANNEL, 128);
    delay(100);
    ledcWrite(BUZZER_PWM_CHANNEL, 0);
    delay(50);
    ledcWrite(BUZZER_PWM_CHANNEL, 128);
    delay(100);
    ledcWrite(BUZZER_PWM_CHANNEL, 0);
    delay(200);
    ledcWrite(BUZZER_PWM_CHANNEL, 128);
    delay(100);
    ledcWrite(BUZZER_PWM_CHANNEL, 0);
    delay(50);
    ledcWrite(BUZZER_PWM_CHANNEL, 128);
    delay(100);
    ledcWrite(BUZZER_PWM_CHANNEL, 0);
}

String urlencode(String str) {
    String encodedString = "";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == ' ') {
        encodedString += '+';
        } else if (isalnum(c)) {
        encodedString += c;
        } else {
        code1 = (c & 0xf) + '0';
        if ((c & 0xf) > 9) {
            code1 = (c & 0xf) - 10 + 'A';
        }
        c = (c >> 4) & 0xf;
        code0 = c + '0';
        if (c > 9) {
            code0 = c - 10 + 'A';
        }
        code2 = '\0';
        encodedString += '%';
        encodedString += code0;
        encodedString += code1;
        }
        yield();
    }
    return encodedString;
}

void writeLogSheet(int module){
    char* employeeName = getEmployeeNameByUid(uidString);
    if (employeeName != nullptr) {
        beep();
        Serial.print("Tên nhân viên với ID ");
        Serial.print(uidString);
        Serial.print(" là: ");
        Serial.println(employeeName);

        String Send_Data_URL = Web_App_URL + "?sts=writelog";
        Send_Data_URL += "&uid=" + urlencode(uidString);;
        Send_Data_URL += "&name=" + urlencode(String(employeeName));
        if(module == 2){
        Send_Data_URL += "&inout="+urlencode("Go In");
        }else{
        Send_Data_URL += "&inout="+urlencode("Go Out");
        }


        Serial.println();
        Serial.println("-------------");
        Serial.println("Send data to Google Spreadsheet...");
        Serial.print("URL : ");
        Serial.println(Send_Data_URL);

        HTTPClient http;

        // HTTP GET Request.
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        int httpCode = http.GET(); 
        Serial.print("HTTP Status Code : ");
        Serial.println(httpCode);

        // Getting response from google sheets.
        String payload;
        if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload : " + payload);    
        }
        
        http.end();
    } else {
        Serial.print("Không tìm thấy nhân viên với ID ");
        Serial.println(uidString);
        beepFail();
    }
}

void readRFID(MFRC522 &rfid, int ssPin, const char* moduleName, int module) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        Serial.print(moduleName);
        Serial.print(" - UID: ");
        uidString="";
        for (byte i = 0; i < rfid.uid.size - 1; i++) {
            uidString.concat(String(rfid.uid.uidByte[i], HEX));
            uidString += " ";
        }
        uidString.concat(String(rfid.uid.uidByte[rfid.uid.size - 1], HEX));
        Serial.println(uidString);
        rfid.PICC_HaltA();

        writeLogSheet(module);
    }
}

void setup() {
    Serial.begin(9600);

    ledcSetup(BUZZER_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    if(!readDataSheet()) Serial.println("Can't read data from google sheet!");

    SPI.begin();
    
    rfid1.PCD_Init();
    rfid2.PCD_Init();
    

    Serial.println("Đang chờ thẻ RFID...");
}

void loop() {
    currentTime = millis();
    if(currentTime - timeDelay1 > 200){
        readRFID(rfid1, SS_1, "Module 1", 1);
        timeDelay1 = currentTime;
    }
    if(currentTime - timeDelay2 > 200){
        readRFID(rfid2, SS_2, "Module 2", 2);
        timeDelay2 = currentTime;
    }
}
