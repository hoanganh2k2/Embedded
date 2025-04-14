#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include <WebServer.h>
#include <vector>

#define WIFI_SSID "SH3 P402"
#define WIFI_PASSWORD "studenthouse402"
WebServer webServer(80);

#define SS   5
#define RST  22
MFRC522 rfid(SS, RST);
String Web_App_URL = "https://script.google.com/macros/s/AKfycbygmyHHv9tn3htDrXUNSLZyFJ7z2luDuoHZOPZr6Fp9PW-7WUd2DOSGFHNfWFJ3H9Nh/exec";

#define BUZZER_PIN 16
#define BUZZER_PWM_CHANNEL 0
#define PWM_FREQ 2000     
#define PWM_RESOLUTION 8  

struct Employee {
    char code[12];
    char name[30];

    Employee(const char* _code, const char* _name){
        strncpy(code, _code, sizeof(code) - 1);
        code[sizeof(code) - 1] = '\0';  

        strncpy(name, _name, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0'; 
    }
};
std::vector<Employee> employees;
int employeeCount = 0;
String scannedUID;

//=========Biến chứa mã HTLM Website==//
const char MainPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Quản Lý Nhân Viên</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
        .container { max-width: 500px; margin: auto; }
        input { display: block; width: calc(100% - 23.2px); margin: 10px 0; padding: 10px; }
        button { display: block; width: 100%; margin: 10px 0; padding: 10px; }
        .rfid-container { display: flex; gap: 10px; align-items: center; }
        .rfid-container input { flex: 1; }
        .rfid-container button { width: auto; padding: 10px; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f4f4f4; }
        .actions { display: flex; gap: 5px; }
        .delete-btn, .edit-btn { padding: 5px 10px; font-size: 12px; cursor: pointer; border: none; border-radius: 3px; }
        .delete-btn { background-color: red; color: white; }
        .edit-btn { background-color: blue; color: white; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Quản Lý Nhân Viên</h2>
        <input type="text" id="name" placeholder="Tên nhân viên">
        <div class="rfid-container">
            <input type="text" id="rfid" placeholder="Mã RFID" onkeydown="detectRFID(event)" readonly="true">
            <button onclick="scanCard()">Quét Card</button>
        </div>
        <button onclick="addEmployee()">Thêm Nhân Viên</button>
        
        <table>
            <thead>
                <tr>
                    <th>STT</th>
                    <th>Tên</th>
                    <th>Mã RFID</th>
                    <th>Hành động</th>
                </tr>
            </thead>
            <tbody id="employeeTable">
                <!-- EMPLOYEE_DATA -->
            </tbody>
        </table>
    </div>

    <div id="loadingPopup" style="display:none; position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%); padding: 20px; background-color: white; border-radius: 8px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); text-align: center;">
    <p>Đang xử lý...</p>
    <img src="https://i.gifer.com/ZZ5H.gif" width="50" height="50" alt="loading">
    </div>

    <script>
        let employeeCount = 0;

        const addEmployee = () => {
            const name = document.getElementById("name").value.trim();
            const rfid = document.getElementById("rfid").value.trim();
            
            if (!name || !rfid) {
                alert("Vui lòng nhập đầy đủ thông tin");
                return;
            }

            showLoadingPopup();

            fetch(`/add?name=${encodeURIComponent(name)}&rfid=${encodeURIComponent(rfid)}`)
                .then(response => response.text())
                .then(data => {
                    hideLoadingPopup();
                    if (data.includes("successfully")) {
                        document.getElementById("name").value = "";
                        document.getElementById("rfid").value = "";
                        location.reload();
                    } else {
                        alert("Lỗi: " + data);
                    }
                })
                .catch(error => alert("Lỗi kết nối: " + error));
        };

        function deleteEmployee(index) {
            if (confirm("Bạn có chắc chắn muốn xóa nhân viên này?")) { // Xác nhận trước khi xóa
                showLoadingPopup();
                fetch(`/delete?index=${index}`)
                    .then(response => {
                        hideLoadingPopup();
                        if (!response.ok) {
                            throw new Error("Lỗi khi xóa nhân viên!"); // Xử lý lỗi HTTP
                        }
                        return response.text();
                    })
                    .then(data => {
                        location.reload();
                    })
                    .catch(error => {
                        alert("Lỗi: " + error.message); // Thông báo lỗi
                    });
            }
        }

        const editEmployee = (index) => {
            const newName = prompt("Nhập tên mới:");
            if (!newName) return;
            
            showLoadingPopup();

            fetch(`/edit?index=${index}&name=${encodeURIComponent(newName)}`)
                .then(response => response.text())
                .then(data => {
                    hideLoadingPopup();
                    if (data.includes("successfully")) {
                        location.reload();
                    } else {
                        alert("Hệ thống thông báo: Lỗi: " + data);
                    }
                })
                .catch(error => alert("Hệ thống thông báo: Lỗi kết nối: " + error));
        };
        
        function detectRFID(event) {
            console.log("RFID nhập: ", event.target.value);
        }

        function scanCard() {
            // Nếu popup đã tồn tại thì không tạo mới
            if (document.getElementById("rfidPopup")) return;

            const popup = document.createElement("div");
            popup.innerHTML = `
                <div style="position:fixed; top:50%; left:50%; transform:translate(-50%, -50%);
                            background:white; padding:20px; border-radius:8px; box-shadow:0 4px 8px rgba(0,0,0,0.2);
                            text-align:center;">
                    <p>Vui lòng quét thẻ RFID...</p>
                    <button onclick="closePopup()">Đóng</button>
                </div>`;
            popup.id = "rfidPopup";
            document.body.appendChild(popup);

            // Liên tục kiểm tra xem có dữ liệu RFID không
            checkRFID();
        }

        function checkRFID() {
            fetch("/scan").then(response => response.text()).then(rfid => {
                if (rfid === "Card has been used") {
                    // Hiển thị thông báo lỗi trong popup
                    const popup = document.getElementById("rfidPopup");
                    if (popup) {
                        popup.innerHTML = `
                            <div style="position:fixed; top:50%; left:50%; transform:translate(-50%, -50%);
                                        background:white; padding:20px; border-radius:8px; box-shadow:0 4px 8px rgba(0,0,0,0.2);
                                        text-align:center;">
                                <p style="color: red; font-weight: bold;">Thẻ này đã được sử dụng!</p>
                                <button onclick="closePopup()">Đóng</button>
                            </div>`;
                    }
                    setTimeout(checkRFID, 1000); // Tiếp tục kiểm tra sau 1 giây
                } else if (rfid !== "No Card Detected" && rfid.trim() !== "") {
                    document.getElementById("rfid").value = rfid;
                    closePopup(); // Đóng popup nếu quét thành công
                } else {
                    setTimeout(checkRFID, 1000); // Kiểm tra lại sau 1 giây
                }
            });
        }

        function closePopup() {
            const popup = document.getElementById("rfidPopup");
            if (popup) {
                popup.remove();
            }
        }

        function showLoadingPopup() {
            document.getElementById("loadingPopup").style.display = "block";
        }

        function hideLoadingPopup() {
            document.getElementById("loadingPopup").style.display = "none";
        }

    </script>
</body>
</html>
)=====";
//=========================================//

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

// Đọc dữ liệu từ Google Sheets
bool readDataSheet() {
    if (WiFi.status() == WL_CONNECTED) {
        String Read_Data_URL = Web_App_URL + "?sts=read";
        Serial.println("Reading from Google Sheet...");
        
        HTTPClient http;
        http.begin(Read_Data_URL);
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            employees.clear();
            std::vector<String> data = splitString(payload, ',');
            for (size_t i = 0; i < data.size() / 3; i++) {
                employees.push_back(Employee(
                    data[i * 3 + 1].c_str(),
                    data[i * 3 + 2].c_str()
                ));
            }         
            http.end();
            return !employees.empty();
        }
        http.end();
    }
    return false;
}

void addRfidSheet(String code, String name) {
    if ( name.length() > 0 && code.length() > 0) {
        String Send_Data_URL = Web_App_URL + "?sts=add_rfid";
        Send_Data_URL += "&rfid=" + urlencode(code);
        Send_Data_URL += "&name=" + urlencode(name);

        HTTPClient http;
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Payload: " + payload);
        }
        http.end();
    }
    else {
        Serial.print("Something wrong");
    }
}

void deleteRfidSheet(int index) {
    if (index >= 0 && index < employees.size()) {
        String Send_Data_URL = Web_App_URL + "?sts=delete_rfid";
        Send_Data_URL += "&index=" + urlencode(String(index));

        HTTPClient http;
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Payload: " + payload);
        }
        http.end();
    }
    else {
        Serial.print("Something wrong");
    }
}

void editRfidSheet(int index, String name) {
    if (index >= 0 && index < employees.size()) {
        String Send_Data_URL = Web_App_URL + "?sts=edit_rfid";
        Send_Data_URL += "&index=" + urlencode(String(index));
        Send_Data_URL += "&name=" + urlencode(name);

        HTTPClient http;
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Payload: " + payload);
        }
        http.end();
    }
    else {
        Serial.print("Something wrong");
    }
}

// Quét thẻ RFID
void scanRFID() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
    scannedUID = "";
    for (byte i = 0; i < rfid.uid.size - 1; i++) {
        scannedUID.concat(String(rfid.uid.uidByte[i], HEX));
        scannedUID += " ";
    }
    scannedUID.concat(String(rfid.uid.uidByte[rfid.uid.size - 1], HEX));
    Serial.println("Scanned RFID: " + scannedUID);
    rfid.PICC_HaltA();
}

bool checkRfid(){
    for(int i = 0; i < employees.size(); i++){
        if(strcmp(employees[i].code, scannedUID.c_str()) == 0) return false;
    }
    return true;
}

void handleRoot() {
    String page = FPSTR(MainPage);
    String employeeData = "";
    for (size_t i = 0; i < employees.size(); i++) {
        employeeData += "<tr><td>" + String(i + 1) + "</td>";
        employeeData += "<td>" + String(employees[i].name) + "</td>";
        employeeData += "<td>" + String(employees[i].code) + "</td>";
        employeeData += "<td><div class=\"actions\">"
                        "<button class=\"edit-btn\" onclick=\"editEmployee(" + String(i) + ")\">Sửa</button>"
                        "<button class=\"delete-btn\" onclick=\"deleteEmployee(" + String(i) + ")\">Xóa</button>"
                        "</div></td></tr>";
    }
    page.replace("<!-- EMPLOYEE_DATA -->", employeeData);
    webServer.send(200, "text/html", page);
}

void handleScanRFID() {
    scanRFID();
    if (scannedUID.length() > 0) {
        if(checkRfid()){
            webServer.send(200, "text/plain", scannedUID);
            scannedUID = "";
            beep();
        }
        else{
            webServer.send(200, "text/plain", "Card has been used");
            scannedUID = "";
            beepFail();
        }
    } else {
        webServer.send(200, "text/plain", "No Card Detected");
    }
}

void handleDeleteEmployee() {
    if (webServer.hasArg("index")) {
        int index = webServer.arg("index").toInt();

        if (index >= 0 && index < employees.size()) {
            employees.erase(employees.begin() + index);
            deleteRfidSheet(index);
        }
    }
    webServer.send(200, "text/plain", "Deleted successfully");
}

void handleAddEmployee() {
    if (webServer.hasArg("name") && webServer.hasArg("rfid")) {
        String name = webServer.arg("name");
        String rfid = webServer.arg("rfid");
        
        if (name.length() > 0 && rfid.length() > 0) {
            employees.push_back(Employee(rfid.c_str(), name.c_str()));
            addRfidSheet(rfid, name);
            webServer.send(200, "text/plain", "Employee added successfully");
        } else {
            webServer.send(400, "text/plain", "Invalid input");
        }
    } else {
        webServer.send(400, "text/plain", "Missing parameters");
    }
}

void handleEditEmployee() {
    if (webServer.hasArg("index") && webServer.hasArg("name")) {
        int index = webServer.arg("index").toInt();
    Serial.print("index1 : " );
    Serial.println(index);
        
        String name = webServer.arg("name");
        
        if (index >= 0 && index < employees.size() && name.length() > 0) {
            strncpy(employees[index].name, name.c_str(), sizeof(employees[index].name) - 1);
            employees[index].name[sizeof(employees[index].name) - 1] = '\0';
            editRfidSheet(index, name);
            webServer.send(200, "text/plain", "Employee updated successfully");
        } else {
            webServer.send(400, "text/plain", "Invalid index or name");
        }
    } else {
        webServer.send(400, "text/plain", "Missing parameters");
    }
}

void setup() {
  Serial.begin(9600);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);

  ledcSetup(BUZZER_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());

  if(!readDataSheet()) Serial.println("Can't read data from google sheet!");

  SPI.begin();
  rfid.PCD_Init();


  webServer.on("/", handleRoot);
  webServer.on("/scan", handleScanRFID);
  webServer.on("/add", handleAddEmployee);
  webServer.on("/edit", handleEditEmployee);
  webServer.on("/delete", handleDeleteEmployee);
  webServer.begin();
  Serial.println("Server started. Waiting for RFID...");
}

void loop() {
    webServer.handleClient();
}