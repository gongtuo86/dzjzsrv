use xopensdb;

-- 区域
DROP TABLE IF EXISTS xopensdb.低周减载区域参数表;
CREATE TABLE xopensdb.低周减载区域参数表 (
    编号 INT NOT NULL,
    名称 VARCHAR(256) NULL,
    类型 TINYINT UNSIGNED NULL,
    CONSTRAINT pk_低周减载区域参数表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载区域值表;
CREATE TABLE xopensdb.低周减载区域值表 (
    编号 INT NOT NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    实际投运馈线数 INT NULL,
    计划投运馈线数 INT NULL,
    实际馈线投运率 FLOAT NULL,
    计划馈线投运率 FLOAT NULL,
    应切荷量 FLOAT NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    实际投运完成率 FLOAT NULL,
    计划投运完成率 FLOAT NULL,
    实际投运总负荷占比 FLOAT NULL,
    计划投运总负荷占比 FLOAT NULL,
    实际备用馈线数 INT NULL,
    计划备用馈线数 INT NULL,
    CONSTRAINT pk_低周减载区域值表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载区域厂站关联表;
CREATE TABLE xopensdb.低周减载区域厂站关联表 (
    厂站编号 char(4) NOT NULL,
    所属分区 INT NULL,
    CONSTRAINT pk_低周减载区域厂站关联表 PRIMARY KEY (厂站编号)
);

DELETE FROM 实时库表模式 WHERE 代码 = 'dzjzarea';
INSERT INTO 实时库表模式 VALUES('dzjzarea','低周减载区域表',1064,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','低周减载区域参数表','低周减载区域值表','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM 实时库列模式 
WHERE
    表代码 = 'dzjzarea';
INSERT INTO 实时库列模式 VALUES('id','编号','dzjzarea',0,5,0,1,0,0,0,0);
INSERT INTO 实时库列模式 VALUES('name','名称','dzjzarea',256,10,0,1,10,10,0,0);
INSERT INTO 实时库列模式 VALUES('type','类型','dzjzarea',0,0,0,1,20,20,0,0);
INSERT INTO 实时库列模式 VALUES('judgepower','投运切荷量','dzjzarea',0,8,0,2,30,30,0,0);
INSERT INTO 实时库列模式 VALUES('planpower','计划切荷量','dzjzarea',0,8,0,2,40,40,0,0);
INSERT INTO 实时库列模式 VALUES('realfeedernum','实际投运馈线数','dzjzarea',0,5,0,2,50,50,0,0);
INSERT INTO 实时库列模式 VALUES('planfeedernum','计划投运馈线数','dzjzarea',0,5,0,2,60,60,0,0);
INSERT INTO 实时库列模式 VALUES('realfeederrate','实际馈线投运率','dzjzarea',0,8,0,2,70,70,0,0);
INSERT INTO 实时库列模式 VALUES('planfeederrate','计划馈线投运率','dzjzarea',0,8,0,2,80,80,0,0);
INSERT INTO 实时库列模式 VALUES('requirepower','应切荷量','dzjzarea',0,8,0,2,90,90,0,0);
INSERT INTO 实时库列模式 VALUES('standbypower','实际备用切荷量','dzjzarea',0,8,0,2,100,100,0,0);
INSERT INTO 实时库列模式 VALUES('planstandbypwr','计划备用切荷量','dzjzarea',0,8,0,2,110,110,0,0);
INSERT INTO 实时库列模式 VALUES('realcomprate','实际投运完成率','dzjzarea',0,8,0,2,120,120,0,0);
INSERT INTO 实时库列模式 VALUES('plancomprate','计划投运完成率','dzjzarea',0,8,0,2,130,130,0,0);
INSERT INTO 实时库列模式 VALUES('realpowerrate','实际投运总负荷占比','dzjzarea',0,8,0,2,140,140,0,0);
INSERT INTO 实时库列模式 VALUES('planpowerrate','计划投运总负荷占比','dzjzarea',0,8,0,2,150,150,0,0);
INSERT INTO 实时库列模式 VALUES('realstandfn','实际备用馈线数','dzjzarea',0,5,0,2,160,160,0,0);
INSERT INTO 实时库列模式 VALUES('planstandfn','计划备用馈线数','dzjzarea',0,5,0,2,170,170,0,0);


-- 轮次
DROP TABLE IF EXISTS xopensdb.低周减载轮次参数表;
CREATE TABLE xopensdb.低周减载轮次参数表 (
    编号 INT NOT NULL,
    名称 VARCHAR(256) NULL,
    所属地区 INT NULL,
    功能类型 TINYINT UNSIGNED NULL,
    轮类型 INT NULL,
    频率或电压整定值 FLOAT NULL,
    动作延时整定值 FLOAT NULL,
    下发应切荷量 FLOAT NULL,
    计划切荷比例 FLOAT NULL,
    CONSTRAINT pk_低周减载轮次参数表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载轮次值表;
CREATE TABLE xopensdb.低周减载轮次值表 (
    编号 INT NOT NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    应切荷量 FLOAT NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    减载容量研判 TINYINT UNSIGNED NULL,
    上次告警时间 INT NULL,
    实际切荷比例 FLOAT NULL,
    CONSTRAINT pk_低周减载轮次参数表 PRIMARY KEY (编号)
);

delete from 实时库表模式 
where
    代码 = 'dzjzround';
insert into 实时库表模式 values('dzjzround','低周减载轮次表',1063,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','低周减载轮次参数表','低周减载轮次值表','','','',0,1,'',0,'','','','',0,0,'','');
delete from 实时库列模式 
where
    表代码 = 'dzjzround';
insert into 实时库列模式 values('id','编号','dzjzround',0,5,0,1,0,0,0,0);
insert into 实时库列模式 values('name','名称','dzjzround',256,10,0,1,10,10,0,0);
insert into 实时库列模式 values('functype','功能类型','dzjzround',0,0,0,1,20,20,0,0);
insert into 实时库列模式 values('roundtype','轮类型','dzjzround',0,5,0,1,30,30,0,0);
insert into 实时库列模式 values('judgepower','投运切荷量','dzjzround',0,8,0,2,40,40,0,0);
insert into 实时库列模式 values('planpower','计划切荷量','dzjzround',0,8,0,2,50,50,0,0);
insert into 实时库列模式 values('requirepower','应切荷量','dzjzround',0,8,0,2,60,60,0,0);
insert into 实时库列模式 values('issuepower','下发应切荷量','dzjzround',0,8,0,1,70,70,0,0);
insert into 实时库列模式 values('standbypower','实际备用切荷量','dzjzround',0,8,0,2,80,80,0,0);
insert into 实时库列模式 values('planstandbypwr','计划备用切荷量','dzjzround',0,8,0,2,90,90,0,0);
insert into 实时库列模式 values('judgerequire','减载容量研判','dzjzround',0,0,0,2,100,100,0,0);
insert into 实时库列模式 values('lastalarm','上次告警时间','dzjzround',0,5,0,2,110,110,0,0);
insert into 实时库列模式 values('planpwrrate','计划切荷比例','dzjzround',0,8,0,1,120,120,0,0);
insert into 实时库列模式 values('realpwrrate','实际切荷比例','dzjzround',0,8,0,2,130,130,0,0);

-- 轮次类型
DROP TABLE IF EXISTS xopensdb.低周减载轮次类型表;
CREATE TABLE xopensdb.低周减载轮次类型表 (
    编号 INT NOT NULL,
    名称 VARCHAR(256),
    CONSTRAINT pk_低周减载轮次类型表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载轮次类型值表;
CREATE TABLE xopensdb.低周减载轮次类型值表 (
    编号 INT NOT NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    实际投运馈线数 INT NULL,
    计划投运馈线数 INT NULL,
    实际馈线投运率 FLOAT NULL,
    计划馈线投运率 FLOAT NULL,
    应切荷量 FLOAT NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    实际投运完成率 FLOAT NULL,
    计划投运完成率 FLOAT NULL,
    实际投运总负荷占比 FLOAT NULL,
    计划投运总负荷占比 FLOAT NULL,
    实际备用馈线数 INT NULL,
    计划备用馈线数 INT NULL,
    CONSTRAINT pk_低周减载轮次类型值表 PRIMARY KEY (编号)
);

delete from 低周减载轮次类型表;
insert into 低周减载轮次类型表 values(1,'第一轮');
insert into 低周减载轮次类型表 values(2,'第二轮');
insert into 低周减载轮次类型表 values(3,'第三轮');
insert into 低周减载轮次类型表 values(4,'第四轮');
insert into 低周减载轮次类型表 values(5,'第五轮');
insert into 低周减载轮次类型表 values(6,'第六轮');
insert into 低周减载轮次类型表 values(7,'特一轮');
insert into 低周减载轮次类型表 values(8,'特二轮');


delete from 实时库表模式 
where
    代码 = 'dzjzroundsta';
insert into 实时库表模式 values('dzjzroundsta','低周减载轮次类型统计表',1065,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','低周减载轮次类型表','低周减载轮次类型值表','','','',0,1,'',0,'','','','',0,0,'','');
delete from 实时库列模式 
where
    表代码 = 'dzjzroundsta';
insert into 实时库列模式 values('id','编号','dzjzroundsta',0,5,0,1,0,0,0,0);
insert into 实时库列模式 values('name','名称','dzjzroundsta',256,10,0,1,30,30,0,0);
insert into 实时库列模式 values('judgepower','投运切荷量','dzjzroundsta',0,8,0,2,40,40,0,0);
insert into 实时库列模式 values('planpower','计划切荷量','dzjzroundsta',0,8,0,2,50,50,0,0);
insert into 实时库列模式 values('realfeedernum','实际投运馈线数','dzjzroundsta',0,5,0,2,60,60,0,0);
insert into 实时库列模式 values('planfeedernum','计划投运馈线数','dzjzroundsta',0,5,0,2,70,70,0,0);
insert into 实时库列模式 values('realfeederrate','实际馈线投运率','dzjzroundsta',0,8,0,2,80,80,0,0);
insert into 实时库列模式 values('planfeederrate','计划馈线投运率','dzjzroundsta',0,8,0,2,90,90,0,0);
insert into 实时库列模式 values('requirepower','应切荷量','dzjzroundsta',0,8,0,2,100,100,0,0);
insert into 实时库列模式 values('standbypower','实际备用切荷量','dzjzroundsta',0,8,0,2,110,110,0,0);
insert into 实时库列模式 values('planstandbypwr','计划备用切荷量','dzjzroundsta',0,8,0,2,120,120,0,0);
insert into 实时库列模式 values('realcomprate','实际投运完成率','dzjzroundsta',0,8,0,2,130,130,0,0);
insert into 实时库列模式 values('plancomprate','计划投运完成率','dzjzroundsta',0,8,0,2,140,140,0,0);
insert into 实时库列模式 values('realpowerrate','实际投运总负荷占比','dzjzroundsta',0,8,0,2,150,150,0,0);
insert into 实时库列模式 values('planpowerrate','计划投运总负荷占比','dzjzroundsta',0,8,0,2,160,160,0,0);
insert into 实时库列模式 values('realstandfn','实际备用馈线数','dzjzroundsta',0,5,0,2,170,170,0,0);
insert into 实时库列模式 values('planstandfn','计划备用馈线数','dzjzroundsta',0,5,0,2,180,180,0,0);


DROP TABLE IF EXISTS xopensdb.低周减载轮次项参数表;
CREATE TABLE xopensdb.低周减载轮次项参数表 (
    编号 INT NOT NULL,
    名称 VARCHAR(256) NULL,
    所属分区 INT NULL,
    所属轮次 INT NULL,
    关联馈线 VARCHAR(22) NULL,
    负荷类型 TINYINT UNSIGNED NULL,
    投退计划 TINYINT UNSIGNED NULL,
    关联开关 VARCHAR(24) NULL,
    有功代码 VARCHAR(24) NULL,
    关联装置 INT NULL,
    关联出口 TINYINT UNSIGNED NULL,
    CONSTRAINT pk_低周减载轮次项参数表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载轮次项值表;
CREATE TABLE xopensdb.低周减载轮次项值表 (
    编号 INT NOT NULL,
    开关状态 TINYINT UNSIGNED NULL,
    功能压板实际状态 TINYINT UNSIGNED NULL,
    功能压板状态研判 TINYINT UNSIGNED NULL,
    频率或电压实际定值 FLOAT NULL,
    动作延时实际定值 FLOAT NULL,
    定值研判 TINYINT UNSIGNED NULL,
    有功功率 FLOAT NULL,
    有源馈线研判 TINYINT UNSIGNED NULL,
    减载方案研判 TINYINT UNSIGNED NULL,
    功能研判 TINYINT UNSIGNED NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    装置告警 TINYINT UNSIGNED NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    上次告警时间 INT NULL,
    出口矩阵研判 TINYINT UNSIGNED NULL,
    CONSTRAINT pk_低周减载轮次项值表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载装置参数表;
CREATE TABLE xopensdb.低周减载装置参数表 (
    编号 INT NOT NULL,
    名称 VARCHAR(256) NULL,
    装置类型 TINYINT UNSIGNED NULL,
    功能类型 TINYINT UNSIGNED NULL,
    关联轮次项数 INT NULL DEFAULT 0,
    所属厂站 CHAR(4) NULL,
    CONSTRAINT pk_低周减载装置参数表 PRIMARY KEY (编号)
);

DROP TABLE IF EXISTS xopensdb.低周减载装置参数值表;
CREATE TABLE xopensdb.低周减载装置参数表 (
    编号 INT NOT NULL,
    告警状态 TINYINT UNSIGNED NULL,
    软压板研判结论 TINYINT UNSIGNED NULL,
    软压板研判失败原因 TINYINT UNSIGNED NULL,
    硬压板研判结论 TINYINT UNSIGNED NULL,
    硬压板研判失败原因 TINYINT UNSIGNED NULL,
    定值研判结论 TINYINT UNSIGNED NULL,
    定值研判失败原因 TINYINT UNSIGNED NULL,
    CONSTRAINT pk_低周减载装置参数值表 PRIMARY KEY (编号)
);

DELETE FROM 实时库表模式 WHERE 代码 = 'dzjzdev';
INSERT INTO 实时库表模式 VALUES('dzjzdev','低周减载装置参数表',1068,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','低周减载装置参数表','低周减载装置参数值表','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM 实时库列模式 WHERE 表代码 = 'dzjzdev';
INSERT INTO 实时库列模式 VALUES('id','编号','dzjzdev',0,5,0,1,0,0,0,0);
INSERT INTO 实时库列模式 VALUES('name','名称','dzjzdev',256,10,0,1,10,10,0,0);
INSERT INTO 实时库列模式 VALUES('ofsta','所属厂站','dzjzdev',4,10,0,1,20,20,0,0);
INSERT INTO 实时库列模式 VALUES('type','装置类型','dzjzdev',0,0,0,1,30,30,0,0);
INSERT INTO 实时库列模式 VALUES('alarm','告警状态','dzjzdev',0,0,0,2,40,40,0,0);
INSERT INTO 实时库列模式 VALUES('sfret','软压板研判结论','dzjzdev',0,0,0,2,50,50,0,0);
INSERT INTO 实时库列模式 VALUES('sfreason','软压板研判失败原因','dzjzdev',0,0,0,2,60,60,0,0);
INSERT INTO 实时库列模式 VALUES('diret','硬压板研判结论','dzjzdev',0,0,0,2,70,70,0,0);
INSERT INTO 实时库列模式 VALUES('direason','硬压板研判失败原因','dzjzdev',0,0,0,2,80,80,0,0);
INSERT INTO 实时库列模式 VALUES('sgret','定值研判结论','dzjzdev',0,0,0,2,90,90,0,0);
INSERT INTO 实时库列模式 VALUES('sgreason','定值研判失败原因','dzjzdev',0,0,0,2,100,100,0,0);


DROP TABLE IF EXISTS xopensdb.低周减载装置参数设定表;
CREATE TABLE xopensdb.低周减载装置参数设定表 (
    所属装置 INT NOT NULL,
    轮次编号 INT NOT NULL,
    压板ID VARCHAR(256) NULL,
    频率或电压定值ID VARCHAR(24) NULL,
    动作延时定值ID VARCHAR(24) NULL,
    告警信号ID VARCHAR(256) NULL,
    动作信号ID VARCHAR(24) NULL,
    出口矩阵ID VARCHAR(24) NULL,
    CONSTRAINT pk_低周减载装置参数设定表 PRIMARY KEY (所属装置 , 轮次编号)
);

DROP VIEW IF EXISTS xopensdb.低周减载轮次项视图;
CREATE VIEW xopensdb.低周减载轮次项视图 (编号 , 名称 , 所属分区 , 分区名称 , 所属轮次 , 轮次名称 , 所属厂站 , 厂站名称 , 关联馈线 , 线路名称 , 负荷类型 , 投退计划 , 关联开关 , 开关名称 , 所属装置 , 装置名称 , 装置类型 , 功能类型 , 装置关联轮次项数 , 压板ID , 频率或电压定值ID , 动作延时定值ID , 告警信号ID , 动作信号ID , 频率或电压整定值 , 动作延时整定值 , 下发应切荷量 , 轮类型 , 轮类型名称 , 有功代码, 关联出口, 出口矩阵ID) AS
    SELECT 
        a.编号,
        a.名称,
        a.所属分区,
        i.名称,
        a.所属轮次,
        h.名称,
        b.厂站代码,
        g.名称,
        a.关联馈线,
        c.名称,
        a.负荷类型,
        a.投退计划,
        a.关联开关,
        b.描述,
        a.关联装置,
        e.名称,
        e.装置类型,
        h.功能类型,
        e.关联轮次项数,
        f.压板ID,
        f.频率或电压定值ID,
        f.动作延时定值ID,
        f.告警信号ID,
        f.动作信号ID,
        h.频率或电压整定值,
        h.动作延时整定值,
        h.下发应切荷量,
        h.轮类型,
        j.名称,
        a.有功代码,
        a.关联出口,
        f.出口矩阵ID
    FROM
        低周减载轮次项参数表 a
            LEFT JOIN 低周减载装置参数表 e ON e.编号 = a.关联装置
            LEFT JOIN 低周减载装置参数设定表 f ON f.所属装置 = a.关联装置 AND f.轮次编号=a.所属轮次
            LEFT JOIN 遥信参数表 b ON a.关联开关 = b.代码
            LEFT JOIN 线路参数表 c ON a.关联馈线 = c.编号
            LEFT JOIN 厂站参数表 g ON g.编号 = b.厂站代码
            LEFT JOIN 低周减载轮次参数表 h ON h.编号 = a.所属轮次
            LEFT JOIN 低周减载区域参数表 i ON i.编号 = a.所属分区
            LEFT JOIN 低周减载轮次类型表 j ON j.编号 = h.轮类型;

DELETE FROM 实时库表模式 WHERE 代码 = 'dzjzrounditem';
INSERT INTO 实时库表模式 VALUES('dzjzrounditem','低周减载轮次项表',1062,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','低周减载轮次项视图','低周减载轮次项值表','低周减载轮次项参数表','低周减载轮次参数表','低周减载装置参数表',0,1,'',0,'','','','',0,0,'','');
DELETE FROM 实时库列模式 WHERE 表代码 = 'dzjzrounditem';
INSERT INTO 实时库列模式 VALUES('id','编号','dzjzrounditem',0,5,0,1,0,0,0,0);
INSERT INTO 实时库列模式 VALUES('name','名称','dzjzrounditem',256,10,0,1,10,10,0,0);
INSERT INTO 实时库列模式 VALUES('strapreal','功能压板实际状态','dzjzrounditem',0,0,0,2,20,20,0,0);
INSERT INTO 实时库列模式 VALUES('strapjudge','功能压板状态研判','dzjzrounditem',0,0,0,2,30,30,0,0);
INSERT INTO 实时库列模式 VALUES('realvalue','频率或电压实际定值','dzjzrounditem',0,8,0,2,40,40,0,0);
INSERT INTO 实时库列模式 VALUES('realtime','动作延时实际定值','dzjzrounditem',0,8,0,2,50,50,0,0);
INSERT INTO 实时库列模式 VALUES('valuejudge','定值研判','dzjzrounditem',0,0,0,2,60,60,0,0);
INSERT INTO 实时库列模式 VALUES('pvalue','有功功率','dzjzrounditem',0,8,0,2,70,70,0,0);
INSERT INTO 实时库列模式 VALUES('activejudge','有源馈线研判','dzjzrounditem',0,0,0,2,80,80,0,0);
INSERT INTO 实时库列模式 VALUES('planjudge','减载方案研判','dzjzrounditem',0,0,0,2,90,90,0,0);
INSERT INTO 实时库列模式 VALUES('funcjudge','功能研判','dzjzrounditem',0,0,0,2,100,100,0,0);
INSERT INTO 实时库列模式 VALUES('judgepower','投运切荷量','dzjzrounditem',0,8,0,2,110,110,0,0);
INSERT INTO 实时库列模式 VALUES('breakstate','开关状态','dzjzrounditem',0,0,0,2,120,120,0,0);
INSERT INTO 实时库列模式 VALUES('linkedbreak','关联开关','dzjzrounditem',24,10,0,1,130,130,0,0);
INSERT INTO 实时库列模式 VALUES('planvalue','频率或电压整定值','dzjzrounditem',0,8,0,1,140,140,0,0);
INSERT INTO 实时库列模式 VALUES('plantime','动作延时整定值','dzjzrounditem',0,8,0,1,150,150,0,0);
INSERT INTO 实时库列模式 VALUES('realvalueid','频率或电压定值ID','dzjzrounditem',24,10,0,1,160,160,0,0);
INSERT INTO 实时库列模式 VALUES('realtimeid','动作延时定值ID','dzjzrounditem',24,10,0,1,170,170,0,0);
INSERT INTO 实时库列模式 VALUES('loadtype','负荷类型','dzjzrounditem',0,0,0,1,180,180,0,0);
INSERT INTO 实时库列模式 VALUES('strapplan','投退计划','dzjzrounditem',0,0,0,1,190,190,0,0);
INSERT INTO 实时库列模式 VALUES('strapid','压板ID','dzjzrounditem',256,10,0,1,200,200,0,0);
INSERT INTO 实时库列模式 VALUES('alarmid','告警信号ID','dzjzrounditem',256,10,0,1,210,210,0,0);
INSERT INTO 实时库列模式 VALUES('roundid','所属轮次','dzjzrounditem',0,5,0,1,220,220,0,0);
INSERT INTO 实时库列模式 VALUES('devalarm','装置告警','dzjzrounditem',0,0,0,2,230,230,0,0);
INSERT INTO 实时库列模式 VALUES('areaid','所属分区','dzjzrounditem',0,5,0,1,240,240,0,0);
INSERT INTO 实时库列模式 VALUES('staid','所属厂站','dzjzrounditem',4,10,0,1,250,250,0,0);
INSERT INTO 实时库列模式 VALUES('planpower','计划切荷量','dzjzrounditem',0,8,0,2,260,260,0,0);
INSERT INTO 实时库列模式 VALUES('roundname','轮次名称','dzjzrounditem',256,10,0,1,270,270,0,0);
INSERT INTO 实时库列模式 VALUES('feedername','线路名称','dzjzrounditem',256,10,0,1,280,280,0,0);
INSERT INTO 实时库列模式 VALUES('roundtype','轮类型','dzjzrounditem',0,5,0,1,290,290,0,0);
INSERT INTO 实时库列模式 VALUES('standbypower','实际备用切荷量','dzjzrounditem',0,8,0,2,300,300,0,0);
INSERT INTO 实时库列模式 VALUES('planstandbypwr','计划备用切荷量','dzjzrounditem',0,8,0,2,310,310,0,0);
INSERT INTO 实时库列模式 VALUES('actionid','动作信号ID','dzjzrounditem',24,10,0,1,320,320,0,0);
INSERT INTO 实时库列模式 VALUES('deviceid','所属装置','dzjzrounditem',0,5,0,1,330,330,0,0);
INSERT INTO 实时库列模式 VALUES('devicename','装置名称','dzjzrounditem',256,10,0,1,340,340,0,0);
INSERT INTO 实时库列模式 VALUES('functype','功能类型','dzjzrounditem',0,0,0,1,350,350,0,0);
INSERT INTO 实时库列模式 VALUES('devtype','装置类型','dzjzrounditem',0,0,0,1,360,360,0,0);
INSERT INTO 实时库列模式 VALUES('pname','有功代码','dzjzrounditem',24,10,0,1,370,370,0,0);
INSERT INTO 实时库列模式 VALUES('lastalarm','上次告警时间','dzjzrounditem',0,5,0,2,380,380,0,0);
INSERT INTO 实时库列模式 VALUES('assocexit','关联出口','dzjzrounditem',0,0,0,1,390,390,0,0);
INSERT INTO 实时库列模式 VALUES('exitid','出口矩阵ID','dzjzrounditem',24,10,0,1,400,400,0,0);
INSERT INTO 实时库列模式 VALUES('exitjudge','出口矩阵研判','dzjzrounditem',0,0,0,2,410,410,0,0);

DROP TABLE IF EXISTS xopensdb.低周减载周期巡检任务表;
CREATE TABLE IF NOT EXISTS xopensdb.低周减载周期巡检任务表 (
    编号 INT NOT NULL,
    名称 VARCHAR(256) NULL,
    周期开始时间 INT NULL,
    周期结束时间 INT NULL,
    周期 INT NULL,
    分区条件 VARCHAR(256) NULL,
    厂站条件 VARCHAR(256) NULL,
    轮次条件 VARCHAR(256) NULL,
    CONSTRAINT pk_低周减载周期巡检任务表 PRIMARY KEY (编号)
);


DELETE FROM 实时库表模式 
WHERE
    代码 = 'dzjzptask';
INSERT INTO 实时库表模式 VALUES('dzjzptask','低周减载周期巡检任务表',1066,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','低周减载周期巡检任务表','','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM 实时库列模式 
WHERE
    表代码 = 'dzjzptask';
INSERT INTO 实时库列模式 VALUES('id','编号','dzjzptask',0,5,0,1,0,0,0,0);
INSERT INTO 实时库列模式 VALUES('name','名称','dzjzptask',256,10,0,1,10,10,0,0);
INSERT INTO 实时库列模式 VALUES('starttime','周期开始时间','dzjzptask',0,5,0,1,20,20,0,0);
INSERT INTO 实时库列模式 VALUES('endtime','周期结束时间','dzjzptask',0,5,0,1,30,30,0,0);
INSERT INTO 实时库列模式 VALUES('interval','周期','dzjzptask',0,5,0,1,40,40,0,0);
INSERT INTO 实时库列模式 VALUES('areacond','分区条件','dzjzptask',256,10,0,1,50,50,0,0);
INSERT INTO 实时库列模式 VALUES('stacond','厂站条件','dzjzptask',256,10,0,1,60,60,0,0);
INSERT INTO 实时库列模式 VALUES('roundcond','轮次条件','dzjzptask',256,10,0,1,70,70,0,0);
INSERT INTO 实时库列模式 VALUES('lastinspect','上次巡检时间','dzjzptask',0,5,0,2,80,80,0,0);

DROP TABLE IF EXISTS  xopensdb.低周减载装置定值信息表;
CREATE TABLE IF NOT EXISTS xopensdb.低周减载装置定值信息表 (
    RTU号 SMALLINT NOT NULL,
    厂站代码 CHAR(4) NOT NULL,
    规约号 SMALLINT NOT NULL,
    装置类型 SMALLINT NOT NULL,
    组号 SMALLINT NOT NULL,
    序号 SMALLINT NOT NULL,
    定值 CHAR(20) NOT NULL,
    描述 CHAR(160) NOT NULL,
    单位 CHAR(5) NOT NULL,
    定值类型 CHAR(20) NOT NULL,
    CONSTRAINT pk_低周减载装置定值信息表 PRIMARY KEY (RTU号 , 装置类型 , 组号 , 序号)
);

DELETE FROM 实时库表模式 
WHERE
    代码 = 'dzjzcdz';
INSERT INTO 实时库表模式 VALUES('dzjzcdz','低周减载装置定值信息表',1067,'scadadb','SCADA','scada',1,1,1,1,2000,'rtuno','type','groupno','no','','','低周减载装置定值信息表','','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM 实时库列模式 
WHERE
    表代码 = 'dzjzcdz';
INSERT INTO 实时库列模式 VALUES('rtuno','RTU号','dzjzcdz',0,3,0,1,0,0,0,0);
INSERT INTO 实时库列模式 VALUES('type','装置类型','dzjzcdz',0,3,0,1,10,10,0,0);
INSERT INTO 实时库列模式 VALUES('groupno','组号','dzjzcdz',0,3,0,1,20,20,0,0);
INSERT INTO 实时库列模式 VALUES('no','序号','dzjzcdz',0,3,0,1,30,30,0,0);
INSERT INTO 实时库列模式 VALUES('value','定值','dzjzcdz',20,10,0,1,40,40,0,0);
INSERT INTO 实时库列模式 VALUES('desc','描述','dzjzcdz',160,10,0,1,50,50,0,0);
INSERT INTO 实时库列模式 VALUES('unit','单位','dzjzcdz',5,10,0,1,60,60,0,0);
INSERT INTO 实时库列模式 VALUES('vtype','定值类型','dzjzcdz',20,10,0,1,70,70,0,0);


DELETE FROM DM配置表 
WHERE
    配置类型 = '低周减载区域类型';
INSERT INTO DM配置表 VALUES('低周减载区域类型','区调',1,'');
INSERT INTO DM配置表 VALUES('低周减载区域类型','变电站',2,'');

delete from DM配置表 
where
    配置类型 = '低周减载轮次类型';
insert into DM配置表 values('低周减载轮次类型','基础轮',1,'');
insert into DM配置表 values('低周减载轮次类型','特殊论',2,'');
insert into DM配置表 values('低周减载轮次类型','附加轮',3,'');

delete from DM配置表 
where
    配置类型 = '低周减载轮次功能类型';
insert into DM配置表 values('低周减载轮次功能类型','低频',1,'');
insert into DM配置表 values('低周减载轮次功能类型','低压',2,'');

delete from DM配置表 
where
    配置类型 = '低周减载负荷类型';
insert into DM配置表 values('低周减载负荷类型','高危用户',1,'');
insert into DM配置表 values('低周减载负荷类型','重要用户',2,'');
insert into DM配置表 values('低周减载负荷类型','中比例民生',3,'');
insert into DM配置表 values('低周减载负荷类型','低比例民生',4,'');
insert into DM配置表 values('低周减载负荷类型','纯工业负荷',5,'');

delete from DM配置表 
where
    配置类型 = '低周减载投退计划类型';
insert into DM配置表 values('低周减载投退计划类型','投入',1,'');
insert into DM配置表 values('低周减载投退计划类型','退出',0,'');

delete from DM配置表 
where
    配置类型 = '低周减载装置类型';
insert into DM配置表 values('低周减载装置类型','集中式',1,'');
insert into DM配置表 values('低周减载装置类型','分散式',2,'');

delete from DM配置表 
where
    配置类型 = '低周减载装置功能类型';
insert into DM配置表 values('低周减载装置功能类型','低频',1,'');
insert into DM配置表 values('低周减载装置功能类型','低压',2,'');
insert into DM配置表 values('低周减载装置功能类型','低频&低压',3,'');

delete from 参数表属性表 
where
    表名 = '低周减载区域参数表';
insert into 参数表属性表 values('低周减载区域参数表','SCADA','编号','编号',0,' ',' ',' ',' ',0,0);
delete from 参数域属性表 
where
    所属表名 = '低周减载区域参数表';
insert into 参数域属性表 values('编号','低周减载区域参数表',5,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('名称','低周减载区域参数表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('类型','低周减载区域参数表',10,0,15,'低周减载区域类型','项目','数值','',0,0,'','',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载轮次类型表';
insert into 参数表属性表 values('低周减载轮次类型表','SCADA','编号','编号',0,' ',' ',' ',' ',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载轮次参数表';
insert into 参数表属性表 values('低周减载轮次参数表','SCADA','编号','编号',0,' ',' ',' ',' ',0,0);
delete from 参数域属性表 
where
    所属表名 = '低周减载轮次参数表';
insert into 参数域属性表 values('编号','低周减载轮次参数表',5,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('名称','低周减载轮次参数表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('所属地区','低周减载轮次参数表',10,0,6,'低周减载区域参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('功能类型','低周减载轮次参数表',10,0,15,'低周减载轮次功能类型','项目','数值','',0,0,'','',0,0);
insert into 参数域属性表 values('轮类型','低周减载轮次参数表',10,0,6,'低周减载轮次类型表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('频率或电压整定值','低周减载轮次参数表',8,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('动作延时整定值','低周减载轮次参数表',8,0,0,'','','','',0,0,'','',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载轮次项参数表';
insert into 参数表属性表 values('低周减载轮次项参数表','SCADA','编号','编号',0,' ',' ',' ',' ',0,0);
delete from 参数域属性表 
where
    所属表名 = '低周减载轮次项参数表';
insert into 参数域属性表 values('编号','低周减载轮次项参数表',5,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('名称','低周减载轮次项参数表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('所属分区','低周减载轮次项参数表',10,0,6,'低周减载区域参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('所属轮次','低周减载轮次项参数表',10,0,6,'低周减载轮次参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('关联馈线','低周减载轮次项参数表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('负荷类型','低周减载轮次项参数表',10,0,15,'低周减载负荷类型','项目','数值','',1,0,'','',0,0);
insert into 参数域属性表 values('投退计划','低周减载轮次项参数表',10,0,15,'低周减载投退计划类型','项目','数值','',1,6,'','',0,0);
insert into 参数域属性表 values('关联开关','低周减载轮次项参数表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('有功代码','低周减载轮次项参数表',10,0,0,'','','','',0,0,'','',0,0);


delete from 参数表属性表 
where
    表名 = '低周减载装置参数表';
insert into 参数表属性表 values('低周减载装置参数表','SCADA','编号','编号',0,' ',' ',' ',' ',0,0);
delete from 参数域属性表 
where
    所属表名 = '低周减载装置参数表';
insert into 参数域属性表 values('编号','低周减载装置参数表',5,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('名称','低周减载装置参数表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('装置类型','低周减载装置参数表',10,0,15,'低周减载装置类型','项目','数值','',0,0,'','',0,0);
insert into 参数域属性表 values('功能类型','低周减载装置参数表',10,0,15,'低周减载装置功能类型','项目','数值','',0,0,'','',0,0);
insert into 参数域属性表 values('关联轮次项数','低周减载装置参数表',5,0,0,'','','','',0,0,'','',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载装置参数设定表';
insert into 参数表属性表 values('低周减载装置参数设定表','SCADA','所属装置','轮次编号',0,' ',' ',' ',' ',0,0);
delete from 参数域属性表 
where
    所属表名 = '低周减载装置参数设定表';
insert into 参数域属性表 values('所属装置','低周减载装置参数设定表',10,0,6,'低周减载装置参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('轮次编号','低周减载装置参数设定表',5,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('压板ID','低周减载装置参数设定表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('频率或电压定值ID','低周减载装置参数设定表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('告警信号ID','低周减载装置参数设定表',10,0,0,'','','','',0,0,'','',0,0);
insert into 参数域属性表 values('动作信号ID','低周减载装置参数设定表',10,0,0,'','','','',0,0,'','',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载轮次项视图';
insert into 参数表属性表 values('低周减载轮次项视图','SCADA','轮次项编号','轮次项编号',0,' ',' ',' ',' ',0,0);
delete from 参数域属性表 
where
    所属表名 = '低周减载轮次项视图';
insert into 参数域属性表 values('所属分区','低周减载轮次项视图',10,0,6,'低周减载区域参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('所属轮次','低周减载轮次项视图',10,0,6,'低周减载轮次参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('负荷类型','低周减载轮次项视图',10,0,15,'低周减载负荷类型','项目','数值','',1,0,'','',0,0);
insert into 参数域属性表 values('投退计划','低周减载轮次项视图',10,0,15,'低周减载投退计划类型','项目','数值','',1,6,'','',0,0);
insert into 参数域属性表 values('所属装置','低周减载轮次项视图',10,0,6,'低周减载装置参数表','名称','编号','',0,0,'','',0,0);
insert into 参数域属性表 values('装置类型','低周减载轮次项视图',10,0,15,'低周减载装置类型','项目','数值','',0,0,'','',0,0);
insert into 参数域属性表 values('功能类型','低周减载轮次项视图',10,0,15,'低周减载轮次功能类型','项目','数值','',0,0,'','',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载装置定值信息表';
insert into 参数表属性表 values('低周减载装置定值信息表','SCADA','RTU号','RTU号',0,' ',' ',' ',' ',0,0);

delete from 参数表属性表 
where
    表名 = '低周减载周期巡检任务表';
insert into 参数表属性表 values('低周减载周期巡检任务表','SCADA','编号','编号',0,' ',' ',' ',' ',0,0);


delete from 程序注册表 
where
    代码 = 'dzjzk';
insert into 程序注册表 values('dzjzk', '低压减载轮次应切荷量的K系数',0,1.3,'',0);
insert into 程序注册表 values('dzjzmax', '低压减载年度计划最高负荷',0,0,'',0);

delete from 事项类型表 
where
    类型号 = 2068;
insert into 事项类型表 values(2,2068,'低周减载',' ',0,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ');

-- 历史数据库
use xopenshdb;

DROP TABLE IF EXISTS xopenshdb.低周减载轮次项历史值表;
CREATE TABLE xopenshdb.低周减载轮次项历史值表 (
    时间 INT NOT NULL,
    编号 INT NOT NULL,
    名称 VARCHAR(256),
    功能压板实际状态 TINYINT UNSIGNED NULL,
    有功功率 FLOAT NULL,
    负荷类型 TINYINT UNSIGNED NULL,
    投退计划 TINYINT UNSIGNED NULL,
    所属分区 INT NULL,
    所属厂站 CHAR(4) NULL,
    轮次类型 INT NULL,
    CONSTRAINT pk_低周减载轮次项历史值表 PRIMARY KEY (时间 , 编号)
);

DROP TABLE IF EXISTS xopenshdb.低周减载轮次历史值表;
CREATE TABLE xopenshdb.低周减载轮次历史值表 (
    时间 INT NOT NULL,
    编号 INT NOT NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    应切荷量 FLOAT NULL,
    下发切荷量 FLOAT NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    减载容量研判 TINYINT UNSIGNED NULL,
    计划切荷比例 FLOAT NULL,
    实际切荷比例 FLOAT NULL,
    CONSTRAINT pk_低周减载轮次历史值表 PRIMARY KEY (时间 , 编号)
);

DROP TABLE IF EXISTS xopenshdb.低周减载轮次类型历史值表;
CREATE TABLE xopenshdb.低周减载轮次类型历史值表 (
    时间 INT NOT NULL,
    编号 INT NOT NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    实际投运馈线数 INT NULL,
    计划投运馈线数 INT NULL,
    实际馈线投运率 FLOAT NULL,
    计划馈线投运率 FLOAT NULL,
    应切荷量 FLOAT NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    实际投运完成率 FLOAT NULL,
    计划投运完成率 FLOAT NULL,
    实际投运总负荷占比 FLOAT NULL,
    计划投运总负荷占比 FLOAT NULL,
    实际备用馈线数 INT NULL,
    计划备用馈线数 INT NULL,
    CONSTRAINT pk_低周减载轮次类型历史值表 PRIMARY KEY (时间 , 编号)
); 

DROP TABLE IF EXISTS xopenshdb.低周减载区域历史值表;
CREATE TABLE xopenshdb.低周减载区域历史值表 (
    时间 INT NOT NULL,
    编号 INT NOT NULL,
    投运切荷量 FLOAT NULL,
    计划切荷量 FLOAT NULL,
    实际投运馈线数 INT NULL,
    计划投运馈线数 INT NULL,
    实际馈线投运率 FLOAT NULL,
    计划馈线投运率 FLOAT NULL,
    应切荷量 FLOAT NULL,
    实际备用切荷量 FLOAT NULL,
    计划备用切荷量 FLOAT NULL,
    实际投运完成率 FLOAT NULL,
    计划投运完成率 FLOAT NULL,
    实际投运总负荷占比 FLOAT NULL,
    计划投运总负荷占比 FLOAT NULL,
    实际备用馈线数 INT NULL,
    计划备用馈线数 INT NULL,
    CONSTRAINT pk_低周减载区域历史值表 PRIMARY KEY (时间 , 编号)
);

DROP TABLE IF EXISTS xopenshdb.低周减载装置动作表;
CREATE TABLE xopenshdb.低周减载装置动作表 (
    ID INT NOT NULL,
    时间 INT NOT NULL,
    描述 VARCHAR(256) NULL,
    投运切荷量 FLOAT NULL,
    动作切荷量 FLOAT NULL,
    动作切荷占比 FLOAT NULL,
    动作馈线数 INT NULL,
    CONSTRAINT pk_低周减载装置动作表 PRIMARY KEY (ID)
);

DROP TABLE IF EXISTS xopenshdb.低周减载轮次项动作表;
CREATE TABLE xopenshdb.低周减载轮次项动作表 (
    动作ID INT NOT NULL,
    时间 INT NOT NULL,
    轮次项ID INT NOT NULL,
    线路名称 VARCHAR(256) NULL,
    切荷量 FLOAT NULL,
    所属分区 INT NULL,
    分区名称 VARCHAR(256),
    轮类型 INT NULL,
    轮类型名称 VARCHAR(256),
    动作信号ID VARCHAR(24),
    动作信号名称 VARCHAR(256),
    装置ID INT NULL,
    装置名称 VARCHAR(256),
    装置类型 TINYINT UNSIGNED,
    轮次功能类型 TINYINT UNSIGNED,
    所属厂站 CHAR(4),
    CONSTRAINT pk_低周减载轮次项动作表 PRIMARY KEY (动作ID , 时间 , 轮次项ID)
);

DROP TABLE IF EXISTS xopenshdb.低周减载巡检结果表;
CREATE TABLE IF NOT EXISTS xopenshdb.低周减载巡检结果表 (
    ID INT NOT NULL,
    巡检模式 TINYINT UNSIGNED NULL,
    周期任务编号 INT NULL,
    巡检时间 INT NOT NULL,
    应投未投 INT NULL,
    应退未退 INT NULL,
    装置异常 INT NULL,
    定值不同 INT NULL,
    CONSTRAINT pk_低周减载巡检结果表 PRIMARY KEY (ID)
);

DROP TABLE IF EXISTS xopenshdb.低周减载巡检详情表;
CREATE TABLE IF NOT EXISTS xopenshdb.低周减载巡检详情表 (
    ID INT NOT NULL,
    轮次项编号 INT NOT NULL,
    开关状态 TINYINT UNSIGNED NULL,
    负荷 FLOAT NULL,
    实际定值 VARCHAR(16) NULL,
    计划定值 VARCHAR(16) NULL,
    实际压板状态 TINYINT UNSIGNED NULL,
    计划压板状态 TINYINT UNSIGNED NULL,
    压板状态研判 TINYINT UNSIGNED NULL,
    装置告警 TINYINT UNSIGNED NULL,
    总告警 TINYINT UNSIGNED NULL,
    CONSTRAINT pk_低周减载巡检详情表 PRIMARY KEY (ID , 轮次项编号)
);

-- 存储过程
use xopenshdb;
DELIMITER //
DROP PROCEDURE IF EXISTS dzjz_delete_old_records //
CREATE PROCEDURE dzjz_delete_old_records()
BEGIN
  DELETE FROM xopenshdb.低周减载轮次项历史值表 WHERE 时间 < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.低周减载轮次历史值表 WHERE 时间 < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.低周减载轮次类型历史值表 WHERE 时间 < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.低周减载区域历史值表 WHERE 时间 < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.低周减载装置动作表 WHERE 时间 < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.低周减载轮次项动作表 WHERE 时间 < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
END //

DELIMITER ;

DROP EVENT IF EXISTS dzjz_delete_old_records_event;
CREATE EVENT dzjz_delete_old_records_event
ON SCHEDULE EVERY 1 DAY STARTS (TIMESTAMP(CURRENT_DATE) + INTERVAL 2 HOUR)
DO CALL dzjz_delete_old_records();

-- 以下为测试记录
USE xopensdb;

DELETE FROM 低周减载区域参数表;
INSERT INTO 低周减载区域参数表 VALUES(1,'长兴',1);
INSERT INTO 低周减载区域参数表 VALUES(2,'变电站1',2);
INSERT INTO 低周减载区域参数表 VALUES(3,'变电站2',2);

DELETE FROM 低周减载区域厂站关联表;
INSERT INTO 低周减载区域厂站关联表 VALUES('xy',2);
INSERT INTO 低周减载区域厂站关联表 VALUES('zj',3);

delete from 低周减载轮次参数表;
insert into 低周减载轮次参数表 values(1,'低频减载第一轮',1,1,1,49,300,100);
insert into 低周减载轮次参数表 values(2,'低频减载第二轮',1,1,2,49,300,100);
insert into 低周减载轮次参数表 values(3,'低频减载第三轮',1,1,3,49,300,100);
insert into 低周减载轮次参数表 values(4,'低频减载第四轮',1,1,4,49,300,100);
insert into 低周减载轮次参数表 values(5,'低频减载第五轮',1,1,5,49,300,100);
insert into 低周减载轮次参数表 values(6,'低频减载附加轮',1,1,6,49,300,100);
insert into 低周减载轮次参数表 values(7,'低频减载特一轮',1,1,7,49,300,100);
insert into 低周减载轮次参数表 values(8,'低频减载特二轮',1,1,8,49,300,100);
insert into 低周减载轮次参数表 values(9,'低压减载第一轮',1,2,1,0.8,300,100);
insert into 低周减载轮次参数表 values(10,'低压减载第二轮',1,2,2,0.85,300,100);
insert into 低周减载轮次参数表 values(11,'低压减载特一轮',1,2,7,0.85,300,100);
insert into 低周减载轮次参数表 values(12,'低压减载特二轮',1,2,8,0.85,300,100);


delete from 低周减载轮次项参数表;
insert into 低周减载轮次项参数表 values(1,'线路1',2,1,'Lxy263',5,1,'Pxy_122_2_1YX',"Pxy_121_1_3YC",1,1);
insert into 低周减载轮次项参数表 values(2,'线路2',2,2,'Lxy263',5,1,'Pxy_122_2_2YX',"Pxy_121_1_3YC",2,2);

insert into 低周减载轮次项参数表 values(3,'线路3',2,1,'Lxy263',4,1,'Pxy_121_2_8YX',"Pxy_121_1_3YC",121,3);
insert into 低周减载轮次项参数表 values(4,'线路4',2,1,'Lxy263',4,1,'Pxy_121_2_9YX',"Pxy_121_1_3YC",121,4);
insert into 低周减载轮次项参数表 values(5,'线路5',2,2,'Lxy263',3,1,'Pxy_121_2_10YX',"Pxy_121_1_3YC",121,5);
insert into 低周减载轮次项参数表 values(6,'线路6',2,2,'Lxy263',3,1,'Pxy_121_2_11YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(7,'线路7',2,3,'Lxy263',2,1,'Pxy_121_2_12YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(8,'线路8',2,3,'Lxy263',2,1,'Pxy_121_2_13YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(9,'线路9',2,4,'Lxy263',1,1,'Pxy_121_2_14YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(10,'线路10',2,4,'Lxy263',1,1,'Pxy_121_2_15YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(11,'线路11',2,5,'Lxy263',5,1,'Pxy_121_2_16YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(12,'线路12',2,5,'Lxy263',5,1,'Pxy_121_2_17YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(13,'线路13',2,6,'Lxy263',5,1,'Pxy_121_2_18YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(14,'线路14',2,6,'Lxy263',5,1,'Pxy_121_2_19YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(15,'线路15',2,7,'Lxy263',5,1,'Pxy_121_2_20YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(16,'线路16',2,7,'Lxy263',5,1,'Pxy_121_2_21YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(17,'线路17',2,8,'Lxy263',5,1,'Pxy_121_2_22YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(18,'线路18',2,8,'Lxy263',5,1,'Pxy_121_2_23YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(19,'线路19',2,9,'Lxy263',5,1,'Pxy_121_2_24YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(20,'线路20',2,9,'Lxy263',5,1,'Pxy_121_2_25YX',"Pxy_121_1_3YC",121,1);
insert into 低周减载轮次项参数表 values(21,'线路21',2,10,'Lxy263',5,1,'Pxy_121_2_26YX',"Pxy_121_1_3YC",121,1);


delete from 低周减载装置参数表;
insert into 低周减载装置参数表 values(1,'低周减载装置1',2,3,1,'xy');
insert into 低周减载装置参数表 values(2,'低周减载装置2',2,3,1,'xy');
insert into 低周减载装置参数表 values(121,'低周减载装置121',1,3,1,'xy');

delete from 低周减载装置参数设定表;
insert into 低周减载装置参数设定表 values(1,1,'Pxy085500000000000140Pg','111:17:31','111:17:32','Pxy085500000000000148Pg','Pxy061300000000000175Pg','1:3:1');
insert into 低周减载装置参数设定表 values(2,1,'Pxy085500000000000140Pg','111:17:31','111:17:32','Pxy085500000000000148Pg','Pxy061300000000000175Pg','1:3:1');

insert into 低周减载装置参数设定表 values(121,1,'Pxy_121_2_34YX','1:3:1','1:3:2','Pxy_121_5_1YX','Pxy_121_4_2YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,2,'Pxy_121_2_35YX','1:3:3','1:3:4','Pxy_121_5_1YX','Pxy_121_4_3YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,3,'Pxy_121_2_36YX','1:3:5','1:3:6','Pxy_121_5_1YX','Pxy_121_4_4YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,4,'Pxy_121_2_37YX','1:3:7','1:3:8','Pxy_121_5_1YX','Pxy_121_4_5YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,5,'Pxy_121_2_38YX','1:3:9','1:3:10','Pxy_121_5_1YX','Pxy_121_4_6YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,6,'Pxy_121_2_39YX','1:3:11','1:3:12','Pxy_121_5_1YX','Pxy_121_4_7YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,7,'Pxy_121_2_40YX','1:3:14','1:3:15','Pxy_121_5_1YX','Pxy_121_4_8YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,8,'Pxy_121_2_41YX','1:3:16','1:3:17','Pxy_121_5_1YX','Pxy_121_4_9YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,9,'Pxy_121_2_42YX','1:3:20','1:3:21','Pxy_121_5_1YX','Pxy_121_4_11YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,10,'Pxy_121_2_43YX','1:3:22','1:3:23','Pxy_121_5_1YX','Pxy_121_4_12YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,11,'Pxy_121_2_44YX','1:3:24','1:3:25','Pxy_121_5_1YX','Pxy_121_4_13YX','1:3:1');
insert into 低周减载装置参数设定表 values(121,12,'Pxy_121_2_45YX','1:3:26','1:3:27','Pxy_121_5_1YX','Pxy_121_4_14YX','1:3:1');

delete from 低周减载周期巡检任务表;
INSERT INTO 低周减载周期巡检任务表 VALUES (1, '任务1', 1672531200, 0, 10, '', '', '');

delete from 低周减载装置定值信息表;
INSERT INTO 低周减载装置定值信息表 VALUES (1, 'xy', 93, 111, 17, 32, '300', '低频减载时间','0',1);
INSERT INTO 低周减载装置定值信息表 VALUES (1, 'xy', 93, 111, 17, 31, '49', '低频减载频率','Hz',0);
INSERT INTO 低周减载装置定值信息表 VALUES (2, 'xy', 93, 111, 17, 32, '300', '低频减载时间','0',1);
INSERT INTO 低周减载装置定值信息表 VALUES (2, 'xy', 93, 111, 17, 31, '49', '低频减载频率','Hz',0);
INSERT INTO 低周减载装置定值信息表 VALUES (122, 'xy', 93, 111, 17, 32, '300', '低频减载时间','0',1);
INSERT INTO 低周减载装置定值信息表 VALUES (122, 'xy', 93, 111, 17, 31, '49', '低频减载频率','Hz',0);
INSERT INTO 低周减载装置定值信息表 VALUES (122, 'xy', 93, 111, 17, 34, '300', '低频减载时间','0',1);
INSERT INTO 低周减载装置定值信息表 VALUES (122, 'xy', 93, 111, 17, 33, '0.85', '低压减载电压','Hz',0);

commit;




