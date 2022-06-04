#include <ESP8266WiFiMulti.h>   //  ESP8266WiFiMulti库
#include <ESP8266WebServer.h>   //  ESP8266WebServer库

 
ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,对象名称是 'wifiMulti'
 
ESP8266WebServer esp8266_server(80);// 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）

//设置固定IP地址
IPAddress local_IP(192, 168, 238, 123); // 设置ESP8266-NodeMCU联网后的IP
IPAddress gateway(192, 168, 238, 1);    // 设置网关IP（通常网关IP是WiFI路由IP）
IPAddress subnet(255, 255, 255, 0);   // 设置子网掩码
IPAddress dns(192,168,238,1);           // 设置局域网DNS的IP

//网页代码
String html="<!DOCTYPE html><head><meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'></head><style>form{display:flex;flex-direction:column;align-items:center;justify-content:space-around;height:60vh;color:#4cc9f0;font-size:30px}input{border:2px solid;background-color:transparent;text-transform:uppercase;font-size:20px;padding:10px 20px;font-weight:300;color:#4cc9f0}input:hover{color:#fff;border:0;background-color:#4cc9f0;-webkit-box-shadow:10px 10px 99px 6px rgba(76,201,240,1);-moz-box-shadow:10px 10px 99px 6px rgba(76,201,240,1);box-shadow:10px 10px 99px 6px rgba(76,201,240,1)}</style><body><form action='/LED' method='POST\'>LampController<input type='submit' value='turn on/off'></form></body>";
void setup(void){
  Serial.begin(9600);   // 启动串口通讯
 
//   设置开发板网络环境
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to Config ESP8266 IP"); 
  }
  pinMode(D4, OUTPUT); //设置D4引脚为输出模式以便控制LED
  //通过addAp函数存储  WiFi名称       WiFi密码
  wifiMulti.addAP("faster", "123456789");
  wifiMulti.addAP("faster1", "12345678");
                                                
  Serial.println("正在连接Wifi,请稍等：");  
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 通过wifiMulti.run()，NodeMCU将会在当前
    delay(1000);                             // 环境中搜索addAP函数所存储的WiFi。如果搜到多个存储的WiFi那么NodeMCU
    Serial.print(i++); Serial.print(' ');    // 将会连接信号最强的那一个WiFi信号。
  }                                          // 一旦连接WiFI成功，wifiMulti.run()将会返回“WL_CONNECTED”。这也是
                                          // 此处while循环判断是否跳出循环的条件。
                                          
  // WiFi连接成功后将通过串口监视器输出连接成功信息 
  Serial.println('\n');
  Serial.print("Connected to \t");
  Serial.println(WiFi.SSID());              // 通过串口监视器输出连接的WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // 通过串口监视器输出ESP8266-NodeMCU的IP
  
 //  当ESP8266开发板建立网络服务器以后，每当有客户端向服务器发送HTTP请求时，利用on函数设置HTTP请求回调函数。
  esp8266_server.begin();                           // 启动网站服务
  esp8266_server.on("/", HTTP_GET, handleRoot);     // 设置服务器根目录即'/'的函数'handleRoot'
  esp8266_server.on("/LED", HTTP_POST, handleLED);  // 设置处理LED控制请求的函数'handleLED'
  esp8266_server.onNotFound(handleNotFound);        // 设置处理404情况的函数'handleNotFound'
 
  Serial.println("HTTP esp8266_server started");//  告知用户ESP8266网络服务功能已经启动
}  
void loop(void){
  esp8266_server.handleClient();                     // 检查有没有客户端设备通过网络向ESP8266网络服务器发送请求
}
 
/*设置服务器根目录的函数'handleRoot'，每当有客户端访问NodeMCU服务器根目录时，
  NodeMCU都会向访问设备发送 HTTP 状态 200 ，同时NodeMCU向浏览器发送HTML代码，send函数中第三个参数，
  即html，就是NodeMCU发送的HTML代码。该代码在网页中显示LED控制按钮。 
  当用户按下按钮时，浏览器将会向NodeMCU的/LED页面发送HTTP请求，请求方式为POST。
  NodeMCU接收到此请求后将会执行handleLED函数内容*/
void handleRoot() {   
//  send函数向客户端发送响应信息。    
  esp8266_server.send(200, "text/html", html);//响应状态码200：请求成功
}
 
//处理LED控制请求的函数'handleLED'
void handleLED() {                          
  digitalWrite(D4,!digitalRead(D4));// 改变LED的点亮或者熄灭状态 
  esp8266_server.sendHeader("Location","/");          // 跳转回页面根目录
  esp8266_server.send(303);                           // 发送Http相应代码303 跳转 ，响应状态码303：浏览器会自动定向到新URI
}
 
// 设置处理404情况的函数'handleNotFound'
void handleNotFound(){
  esp8266_server.send(404, "text/plain", "404: Not found"); // 发送 HTTP 状态 404 (未找到页面) 并向浏览器发送文字 "404: Not found"
}
