# HTTP  简介

该例程演示了如何使用涂鸦 API 获取，涂鸦云的数据，在该示例具体 API 和数据进行了打码处理，所以直接执行该例程无法得到预期效果。

**重要：在执行该示例之前需要先调用 `example_soc_init` 进入配网模式，并使用涂鸦智能 APP 对设备完成配网后，方可调用。**

## 参数含义介绍

| 参数名            | 说明                                  |
|:--------------:|:-----------------------------------:|
| tempw.temp     | 大气温度，单位：摄氏度 ℃                       |
| w.pm25         | 细颗粒物 PM2.5，单位：µg/m³                 |
| w.sunset       | 日落时间                                |
| w.sunrise      | 日出时间                                |
| t.local        | 本地时间，搭配日出（w.sunrise）和日落（w.sunset）使用 |
| w.humidity     | 空气湿度                                |
| condition      | 实时天气                                |
| w.windSpeed    | 风速，单位：米/秒（m/s）                      |
| w.windDir      | 风向                                  |
| w.windLevel    | 风级                                  |
| w.aqi          | 空气质量指数，采用美国 EPA 标准：0 ~ 500          |
| rank           | 详细 AQI 实况及全国排名                      |
| w.pm10         | 可吸入颗粒物 PM10，单位：µg/m³                |
| w.o3           | 臭氧浓度，单位：µg/m³                       |
| w.no2          | 二氧化氮浓度，单位：µg/m³                     |
| w.co           | 一氧化碳浓度，单位：µg/m³                     |
| w.so2          | 二氧化硫浓度 ，单位：µg/m³                    |
| w.thigh        | 最高温度，单位：摄氏度 ℃                       |
| w.tlow         | 最低温度 ，单位：摄氏度 ℃                      |
| w.conditionNum | 天气概况数字编码                            |
| w.realFeel     | 体感温度                                |
| w.date.n       | 需要预报的天数，用 n 来表示天数，要求 1<= n <=7      |

## 运行结果

```c
[09-21 17:37:08 TUYA D][lr:0x74b59] Post Data: {xxxxxxxxxxxxxxx}
[09-21 17:37:08 TUYA D][lr:0x74b99] Post URL: xxxxxxxxxxxxxxxxx
...
[09-21 17:37:09 TUYA D][lr:0x75549] Decode Rev:{"result":{"data":{"w.temp":24},"expiration":30},"t":1663753029,"success":true}
[09-21 17:37:09 TUYA D][lr:0xafa89] tls transporter close socket fd:4
[09-21 17:37:09 TUYA D][lr:0xc1ce1] tcp transporter close socket fd:4
[09-21 17:37:09 TUYA D][lr:0xafaa3] tls transporter close tls handler:0x41a938
[09-21 17:37:09 TUYA D][lr:0x793db] TUYA_TLS Disconnect ENTER
[09-21 17:37:09 TUYA D][lr:0x7942b] TUYA_TLS Disconnect Success
[09-21 17:37:09 TUYA D][lr:0x79083] tuya_tls_connect_destroy.
[09-21 17:37:09 TUYA N][example_http.c:56] print_data: {"data":{"w.temp.0":9,"w.pm25.0":12,"w.sunSet.0":"2023-03-17 18:08","w.sunRise.0":"2023-03-17 06:07","w.humidity.0":87,"w.condition.0":"小雨","w.windSpeed.0":"2.4","w.windDir.0":"N","w.aqi.0":26,"w [03-17 16:29:38 TUYA D][lr:0x84753] next schema get slot 55376.
```

注意这里的天气服务返回的内容（内容为 json 格式）太多了，所以在日志中显示可能会不全。

## 技术支持

您可以通过以下方法获得涂鸦的支持:

- TuyaOS 论坛： https://www.tuyaos.com

- 开发者中心： https://developer.tuya.com

- 帮助中心： https://support.tuya.com/help

- 技术支持工单中心： https://service.console.tuya.com
