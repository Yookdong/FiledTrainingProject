Create Database Opentutorials;

show databases;

use Opentutorials;

CREATE TABLE Topic(ID INT(11) NOT NULL AUTO_INCREMENT, Title VARCHAR(100) NOT NULL, Descript TEXT NULL, CreateTime DATETIME NOT NULL, Author VARCHAR(30) NULL,  PRIMARY KEY(ID));

desc topic;

insert into topic (title,descript,createtime,author) values("MySQL", "MySQL is ...",now(),"dong");

insert into topic (title,descript,createtime,author) values("visual", "visual is ...",now(),"asdf");
insert into topic (title,descript,createtime,author) values("dong", "dong is ...",now(),"qwer");
insert into topic (title,descript,createtime,author) values("ghnghn", "ghnghn is ...",now(),"zxcvz");

select * from topic; -- 모든 것을 다 보여라

select id,title,Author from topic; -- 원하는 것만 골라서 보여라

select id,title,Author from topic where title="MySQL";

select id,title,Author from topic where title="MySQL" order by id desc;

select id,title,Author from topic where title="MySQL" order by id desc limit 2;

