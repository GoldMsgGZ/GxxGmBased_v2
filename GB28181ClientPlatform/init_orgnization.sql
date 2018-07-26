DROP TABLE IF EXISTS 'orgnization';
CREATE TABLE orgnization(
id INTEGER PRIMARY KEY NOT NULL,
name VARVHAR(32) NOT NULL,
gbcode VARCHAR(32) NOT NULL,
father_gbcode VARCHAR(32)
);

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('广东省公安厅', '44000000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('广州市公安局', '44010000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('韶关市公安局', '44020000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('深圳市公安局', '44030000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('珠海市公安局', '44040000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('汕头市公安局', '44050000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('佛山市公安局', '44060000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('江门市公安局', '44070000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('湛江市公安局', '44080000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('茂名市公安局', '44090000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('肇庆市公安局', '44120000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('惠州市公安局', '44130000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('梅州市公安局', '44140000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('汕尾市公安局', '44150000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('河源市公安局', '44160000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('阳江市公安局', '44170000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('清远市公安局', '44180000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('东莞市公安局', '44190000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('中山市公安局', '44200000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('潮州市公安局', '44510000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('揭阳市公安局', '44520000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('云浮市公安局', '44530000', '44000000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('荔湾公安分局', '44010300', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('越秀公安分局', '44010400', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('海珠公安分局', '44010500', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('天河公安分局', '44010600', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('白云公安分局', '44010700', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('黄埔公安分局', '44011200', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('番禺公安分局', '44011300', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('花都公安分局', '44011400', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('南沙公安分局', '44011500', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('萝岗公安分局', '44011600', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('增城公安分局', '44018300', '44010000');

INSERT 
INTO orgnization(name, gbcode, father_gbcode)
VALUES('从化公安分局', '44018400', '44010000');