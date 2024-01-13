## esp8266 远程开关

> 基于Arduino框架、platformio

> 基础知识学习于 [太极创客](http://www.taichi-maker.com/)

## 功能
* [x] 网页配置wifi

## 硬件
1. esp8266 nodemcu
2. 继电器

## 使用本地docker部署 MQTT
1. [emqx](https://www.emqx.com/zh)
2. [MQTTX 客户端](https://mqttx.app/zh?utm_source=mqttx&utm_medium=referral&utm_campaign=about-to-mqttx)


- 官方 Docker 镜像快速安装和运行 EMQX 文档 https://www.emqx.io/docs/zh/latest/deploy/install-docker.html

### docker 拉取 emqx
```
docker pull emqx/emqx:5.4.1
```

### docker 运行 emqx
```
docker run -d --name emqx -p 1883:1883 -p 8083:8083 -p 8084:8084 -p 8883:8883 -p 18083:18083 emqx/emqx:5.4.1
```

## 使用免费的公共 MQTT 服务器
1. broker.emqx.io https://www.emqx.com/zh/mqtt/public-mqtt5-broker


## 待完善
**手机app开关控制，mqtt服务器**

### 解决方案
1. 自己编写app、自己部署mqtt
2. 使用第三方app、mqtt 如[点灯科技](https://www.diandeng.tech/home)
