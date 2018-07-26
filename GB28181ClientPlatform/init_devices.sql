DROP TABLE IF EXISTS 'devices';
CREATE TABLE devices(
id INTEGER PRIMARY KEY NOT NULL,
name VARVHAR(32) NOT NULL,
type INTEGER NOT NULL,
gbcode VARCHAR(32) NOT NULL,
father_gbcode VARCHAR(32) NOT NULL
);

INSERT
INTO devices(name, type, gbcode, father_gbcode)
VALUES ('陈映庭', 1, '44011600001290000001', '44011600');

INSERT
INTO devices(name, type, gbcode, father_gbcode)
VALUES ('欧阳浩哲', 1, '44011600001290000002', '44011600');

INSERT
INTO devices(name, type, gbcode, father_gbcode)
VALUES ('李祥明', 1, '44011600001290000003', '44011600');
