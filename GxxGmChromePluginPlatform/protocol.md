# GxxGm Chrome 插件交互协议定义

|  作者  | 文档版本号 |    时间    |    说明     |
|:------:|:---------:|:----------:|:----------:|
| baggio |   v1.0.0  | 2019-06-16 |    初创    |

## 1. 协议规约
### 1.1. 协议通信对象
- 本地插件管理服务：
  - 负责提供与Web前端交互的WebSocket接口以及HTTP接口
  - 负责响应Web前端提交的插件控制请求，插件控制请求包含但不限于以下请求：
    - 初始化插件
    - 销毁插件
    - 插件控制命令
  - 本地插件管理接口监听10500（HTTP端口）和10501（WebSocket端口）
- Web端页面：
  -

### 1.2. 协议通信规范

- 协议通信的协议体采用json格式数据传输
- 协议通信采用请求应答式通信

## 2. 协议定义
### 2.1. 初始化插件

本协议用于向本地插件管理服务系统初始化指定插件的功能。
#### 请求协议
{
  "request_id":"{9F23C15F-C525-4f38-BE7F-C50CF498C610}",
  "request_time":1560670029000,
  "request_method":"CreatePlugin",
  "request_param":{
    "plugin_type":"realplay_player",
    "page_title":""
    "player_position_left":8,
    "player_position_top":8,
    "player_position_right":208,
    "player_position_bottom":208,
  }
}

- 参数说明

|        参数名称         | 参数说明 | 备注 |
|:----------------------:|:-------:|:----:|
| request_id             |  请求ID  |
| request_time           | 请求时间 |
| request_method         | 请求方法 |
| request_param          | 请求参数 |
| plugin_type            | 插件类型 |
| page_title             | 页面标题 | 用于定位加载此页面的插件
| player_position_left   |
| player_position_top    |
| player_position_right  |
| player_position_bottom |
