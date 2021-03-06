DROP TABLE IF EXISTS 'orgnization';
CREATE TABLE orgnization(
id INTEGER PRIMARY KEY NOT NULL, 
name VARVHAR(64) NOT NULL,            -- 组织架构名称
gbcode VARCHAR(64) NOT NULL,          -- 国标编码
father_gbcode VARCHAR(64) NOT NULL,   -- 父部门编码
manufacturer VARCHAR(64) NOT NULL,    -- 制造商
model VARCHAR(64) NOT NULL,           -- 型号
owner VARCHAE(64) NOT NULL,           -- 所有者
civilcode VARCHAR(64) NOT NULL,       -- 行政区域
block VARCHAR(64) NOT NULL,           -- 警区
address VARCHAR(64) NOT NULL,         -- 地址
parental INTEGER NOT NULL,            -- 是否是父对象
safety_way INTEGER NOT NULL,          -- 信令安全模式
register_way INTEGER NOT NULL,        -- 注册方式，默认：1
cert_num VARCHAR(64) NOT NULL,        -- 证书序列号
certifiable INTEGER NOT NULL,         -- 证书有效标志，有证书时为1，没有为0
errcode INTEGER NOT NULL,             -- 有证书且该证书无效的设备必选 证书无效原因码，一般为400
cert_endtime VARCHAR(64) NOT NULL,    -- 
secrecy INTEGER NOT NULL,             -- 保密属性 0不涉密 1涉密
ip_address VARCHAR(64) NOT NULL,      -- IP地址
port INTEGER NOT NULL,                -- 端口号
password VARCHAR(64) NOT NULL,        -- 
status INTEGER NOT NULL,              -- 在线状态，1-在线，0-不在线
longitude FLOAT NOT NULL,             -- 经度坐标
latitude FLOAT NOT NULL               -- 纬度坐标
);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('广东省公安厅', '44', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('广州市公安局', '4401', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('韶关市公安局', '4402', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('深圳市公安局', '4403', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('珠海市公安局', '4404', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('汕头市公安局', '4405', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('佛山市公安局', '4406', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('江门市公安局', '4407', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('湛江市公安局', '4408', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('茂名市公安局', '4409', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('肇庆市公安局', '4412', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('惠州市公安局', '4413', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('梅州市公安局', '4414', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('汕尾市公安局', '4415', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('河源市公安局', '4416', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('阳江市公安局', '4417', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('清远市公安局', '4418', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('东莞市公安局', '4419', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('中山市公安局', '4420', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('潮州市公安局', '4451', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('揭阳市公安局', '4452', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('云浮市公安局', '4453', '44', 'GOSUNCN', 'V3.3', '广东省', '44', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('荔湾公安分局', '440103', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('越秀公安分局', '440104', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('海珠公安分局', '440105', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('天河公安分局', '440106', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('白云公安分局', '440107', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('黄埔公安分局', '440112', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('番禺公安分局', '440113', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('花都公安分局', '440114', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('南沙公安分局', '440115', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('萝岗公安分局', '440116', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('增城公安分局', '440183', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT 
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES('从化公安分局', '440184', '4401', 'GOSUNCN', 'V3.3', '广东省', '4401', '44010000', '44010000', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

-- 下面是设备列表
INSERT
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES ('陈映庭的设备', '44011600001290000001', '440116', 'GOSUNCN', 'V3.3', '440116', '44011600', '44011600', '44011600', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES ('欧阳浩哲的设备', '44011600001290000002', '440116', 'GOSUNCN', 'V3.3', '440116', '44011600', '44011600', '44011600', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);

INSERT
INTO orgnization(name, gbcode, father_gbcode, manufacturer, model, owner, civilcode, block, address, parental, safety_way, register_way, cert_num, certifiable, errcode, cert_endtime, secrecy, ip_address, port, password, status, longitude, latitude)
VALUES ('李祥明的设备', '44011600001290000003', '440116', 'GOSUNCN', 'V3.3', '440116', '44011600', '44011600', '44011600', 0, 0, 1, 'cert_01', 0, 400, '2010-11-11T19:46:17', 0, '192.168.1.1', 5090, '12345678', 1, 113.283198, 23.143136);
