DROP TABLE IF EXISTS 'config';
CREATE TABLE config(
id INTEGER PRIMARY KEY NOT NULL ,
current_ip VARCHAR(32) NOT NULL,
current_port VARCHAR(32) NOT NULL,
current_gbcode VARCHAR(32) NOT NULL,
remote_ip VARCHAR(32) NOT NULL,
remote_port VARCHAR(32) NOT NULL,
remote_gbcode VARCHAR(32) NOT NULL,
authen_username VARCHAR(32) NOT NULL,
authen_password VARCHAR(32) NOT NULL
);

INSERT 
INTO config(current_ip, current_port, current_gbcode, remote_ip, remote_port, remote_gbcode, authen_username, authen_password)
VALUES('192.168.199.102', '5090', '34020000001320000001', '192.168.199.102', '5060', '34020000002000000001', 'admin', '12345678')