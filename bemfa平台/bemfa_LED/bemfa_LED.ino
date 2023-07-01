/*
* 智能语言控制控制，支持同时天猫、小爱、小度、google Assistent控制
* 也同时支持web控制、小程序控制、app控制，定时控制等
* QQ群：566565915
* 项目示例：通过发送on或off控制开关
* 官网：bemfa.com
*/

#include <ESP8266WiFi.h>               //默认，加载WIFI头文件
#include "PubSubClient.h"              //默认，加载MQTT库文件
#include <ESP8266httpUpdate.h>
//********************需要修改的部分*******************//
const char* ssid = "SADY";           //修改，你的路由去WIFI名字
const char* password = "1q2w3e4r..";     //你的WIFI密码
#define ID_MQTT  "438c094dd3d94f70a4ef1c3b5e00997e"     //用户私钥，控制台获取
const char*  topic = "ESP01S002";        //主题名字，可在巴法云控制台自行创建，名称随意
const int B_led = 0;       //单片机LED引脚值，D系列是NodeMcu引脚命名方式，其他esp8266型号将D2改为自己的引脚
//**************************************************//
String upUrl = "http://bin.bemfa.com/b/1BcNDM4YzA5NGRkM2Q5NGY3MGE0ZWYxYzNiNWUwMDk5N2U=ESP01S002.bin";

const char* mqtt_server = "bemfa.com"; //默认，MQTT服务器
const int mqtt_server_port = 9501;      //默认，MQTT服务器
WiFiClient espClient;
PubSubClient client(espClient);

//灯光函数及引脚定义
void turnOnLed();
void turnOffLed();

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic:");
  Serial.println(topic);
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Msg:");
  Serial.println(msg);
  if (msg == "on") {//如果接收字符on，亮灯
    turnOnLed();//开灯函数
  } else if (msg == "off") {//如果接收字符off，亮灯
    turnOffLed();//关灯函数
  }else if(msg == "update"){  //如果收到指令update
      updateBin();//执行升级函数
    }
  msg = "Nothing to do";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID_MQTT)) {
      Serial.println("connected");
      Serial.print("subscribe:");
      Serial.println(topic);
      //订阅主题，如果需要订阅多个主题，可发送多条订阅指令client.subscribe(topic2);client.subscribe(topic3);
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  pinMode(B_led, OUTPUT); //设置引脚为输出模式
  digitalWrite(B_led, 0);//默认引脚上电高电平
  Serial.begin(115200);     //设置波特率115200
  setup_wifi();           //设置wifi的函数，连接wifi
  client.setServer(mqtt_server, mqtt_server_port);//设置mqtt服务器
  client.setCallback(callback); //mqtt消息处理
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}

//打开灯泡
void turnOnLed() {
  Serial.println("turn on light");
  digitalWrite(B_led, 0);
}
//关闭灯泡
void turnOffLed() {
  Serial.println("turn off light");
  digitalWrite(B_led, 1);
}



//当升级开始时，打印日志
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

//当升级结束时，打印日志
void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

//当升级中，打印日志
void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

//当升级失败时，打印日志
void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}



/**
 * 固件升级函数
 * 在需要升级的地方，加上这个函数即可，例如setup中加的updateBin(); 
 * 原理：通过http请求获取远程固件，实现升级
 */
void updateBin(){
  Serial.println("start update");    
  WiFiClient UpdateClient;

  ESPhttpUpdate.onStart(update_started);//当升级开始时
  ESPhttpUpdate.onEnd(update_finished); //当升级结束时
  ESPhttpUpdate.onProgress(update_progress); //当升级中
  ESPhttpUpdate.onError(update_error); //当升级失败时

  t_httpUpdate_return ret = ESPhttpUpdate.update(UpdateClient, upUrl);
  switch(ret) {
    case HTTP_UPDATE_FAILED:      //当升级失败
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:  //当无升级
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:         //当升级成功
        Serial.println("[update] Update ok.");
        break;
  }
}
