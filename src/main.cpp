#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>

const char *mqttServer = "192.168.124.22";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiManager wifiManager;

const char *mqttUserName = "";                       // 服务端连接用户名
const char *mqttPassword = "";                       // 服务端连接密码
String clientIdStr = "esp8266-" + WiFi.macAddress(); // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
const char *clientId = clientIdStr.c_str();          // 客户端id
const char *subTopic = "fisher/sub";                 // 订阅主题
const char *pubTopic = "fisher/pub";                 // 订阅主题
const char *willTopic = "willTopic";                 // 遗嘱主题名称

// 遗嘱相关信息
const char *willMsg = "esp8266 offline"; // 遗嘱主题信息
const int willQos = 0;                   // 遗嘱QoS
const int willRetain = false;            // 遗嘱保留

const int subQoS = 1;            // 客户端订阅主题时使用的QoS级别（截止2020-10-07，仅支持QoS = 1，不支持QoS = 2）
const bool cleanSession = false; // 清除会话（如QoS>0必须要设为false）

bool D2Status = LOW;

// 发布信息
void pubMQTTmsg()
{
  char *pubMessage;

  if (D2Status == LOW)
  {
    pubMessage = "D2 LOW";
  }
  else
  {
    pubMessage = "D2 HIGH";
  }

  // 实现ESP8266向主题发布信息
  if (mqttClient.publish(pubTopic, pubMessage))
  {
    Serial.println("Publish Topic:");
    Serial.println(pubTopic);
    Serial.println("Publish message:");
    Serial.println(pubMessage);
  }
  else
  {
    Serial.println("Message Publish Failed.");
  }
}

// 订阅指定主题
void subscribeTopic()
{

  // 通过串口监视器输出是否成功订阅主题以及订阅的主题名称
  // 请注意subscribe函数第二个参数数字为QoS级别。这里为QoS = 1
  if (mqttClient.subscribe(subTopic, subQoS))
  {
    Serial.print("Subscribed Topic: ");
    Serial.println(subTopic);
  }
  else
  {
    Serial.print("Subscribe Fail...");
  }
}

// 连接MQTT服务器并订阅信息
void connectMQTTserver()
{
  /* 连接MQTT服务器
  boolean connect(const char* id, const char* user,
                  const char* pass, const char* willTopic,
                  uint8_t willQos, boolean willRetain,
                  const char* willMessage, boolean cleanSession);
  若让设备在离线时仍然能够让qos1工作，则connect时的cleanSession需要设置为false
                  */
  if (mqttClient.connect(clientId))
  {
    Serial.print("MQTT Server Connected. ClientId: ");
    Serial.println(clientId);
    Serial.print("MQTT Server: ");
    Serial.println(mqttServer);

    subscribeTopic(); // 订阅指定主题
  }
  else
  {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(5000);
  }
}

// 收到信息后的回调函数
void receiveCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  Serial.print("Message Length(Bytes) ");
  Serial.println(length);

  if ((char)payload[0] == '1')
  { // 如果收到的信息以“1”为开始
    D2Status = HIGH;
    digitalWrite(D2, D2Status);
  }
  else
  {
    D2Status = LOW;
    digitalWrite(D2, D2Status);
  }

  pubMQTTmsg();
}

void setup()
{
  pinMode(D2, OUTPUT); // 设置板上D2引脚为输出模式
  Serial.begin(9600);                   // 启动串口通讯

  // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
  wifiManager.autoConnect("AutoConnectAP");

  // 如果您希望该WiFi添加密码，可以使用以下语句：
  // wifiManager.autoConnect("AutoConnectAP", "12345678");
  // 以上语句中的12345678是连接AutoConnectAP的密码

  // WiFi连接成功后将通过串口监视器输出连接成功信息
  Serial.println("");
  Serial.print("ESP8266 Connected to ");
  Serial.println(WiFi.SSID()); // WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // IP

  // 设置MQTT服务器和端口号
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(receiveCallback);

  // 连接MQTT服务器
  connectMQTTserver();
}

void loop()
{
  // 如果开发板未能成功连接服务器，则尝试连接服务器
  if (!mqttClient.connected())
  {
    connectMQTTserver();
  }

  // 处理信息以及心跳
  mqttClient.loop();
}