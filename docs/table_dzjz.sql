use xopensdb;

-- ����
DROP TABLE IF EXISTS xopensdb.���ܼ������������;
CREATE TABLE xopensdb.���ܼ������������ (
    ��� INT NOT NULL,
    ���� VARCHAR(256) NULL,
    ���� TINYINT UNSIGNED NULL,
    CONSTRAINT pk_���ܼ������������ PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ�������ֵ��;
CREATE TABLE xopensdb.���ܼ�������ֵ�� (
    ��� INT NOT NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    ʵ��Ͷ�������� INT NULL,
    �ƻ�Ͷ�������� INT NULL,
    ʵ������Ͷ���� FLOAT NULL,
    �ƻ�����Ͷ���� FLOAT NULL,
    Ӧ�к��� FLOAT NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    ʵ��Ͷ������� FLOAT NULL,
    �ƻ�Ͷ������� FLOAT NULL,
    ʵ��Ͷ���ܸ���ռ�� FLOAT NULL,
    �ƻ�Ͷ���ܸ���ռ�� FLOAT NULL,
    ʵ�ʱ��������� INT NULL,
    �ƻ����������� INT NULL,
    CONSTRAINT pk_���ܼ�������ֵ�� PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ�������վ������;
CREATE TABLE xopensdb.���ܼ�������վ������ (
    ��վ��� char(4) NOT NULL,
    �������� INT NULL,
    CONSTRAINT pk_���ܼ�������վ������ PRIMARY KEY (��վ���)
);

DELETE FROM ʵʱ���ģʽ WHERE ���� = 'dzjzarea';
INSERT INTO ʵʱ���ģʽ VALUES('dzjzarea','���ܼ��������',1064,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','���ܼ������������','���ܼ�������ֵ��','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM ʵʱ����ģʽ 
WHERE
    ����� = 'dzjzarea';
INSERT INTO ʵʱ����ģʽ VALUES('id','���','dzjzarea',0,5,0,1,0,0,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('name','����','dzjzarea',256,10,0,1,10,10,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('type','����','dzjzarea',0,0,0,1,20,20,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('judgepower','Ͷ���к���','dzjzarea',0,8,0,2,30,30,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planpower','�ƻ��к���','dzjzarea',0,8,0,2,40,40,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realfeedernum','ʵ��Ͷ��������','dzjzarea',0,5,0,2,50,50,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planfeedernum','�ƻ�Ͷ��������','dzjzarea',0,5,0,2,60,60,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realfeederrate','ʵ������Ͷ����','dzjzarea',0,8,0,2,70,70,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planfeederrate','�ƻ�����Ͷ����','dzjzarea',0,8,0,2,80,80,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('requirepower','Ӧ�к���','dzjzarea',0,8,0,2,90,90,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('standbypower','ʵ�ʱ����к���','dzjzarea',0,8,0,2,100,100,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planstandbypwr','�ƻ������к���','dzjzarea',0,8,0,2,110,110,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realcomprate','ʵ��Ͷ�������','dzjzarea',0,8,0,2,120,120,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('plancomprate','�ƻ�Ͷ�������','dzjzarea',0,8,0,2,130,130,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realpowerrate','ʵ��Ͷ���ܸ���ռ��','dzjzarea',0,8,0,2,140,140,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planpowerrate','�ƻ�Ͷ���ܸ���ռ��','dzjzarea',0,8,0,2,150,150,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realstandfn','ʵ�ʱ���������','dzjzarea',0,5,0,2,160,160,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planstandfn','�ƻ�����������','dzjzarea',0,5,0,2,170,170,0,0);


-- �ִ�
DROP TABLE IF EXISTS xopensdb.���ܼ����ִβ�����;
CREATE TABLE xopensdb.���ܼ����ִβ����� (
    ��� INT NOT NULL,
    ���� VARCHAR(256) NULL,
    �������� INT NULL,
    �������� TINYINT UNSIGNED NULL,
    ������ INT NULL,
    Ƶ�ʻ��ѹ����ֵ FLOAT NULL,
    ������ʱ����ֵ FLOAT NULL,
    �·�Ӧ�к��� FLOAT NULL,
    �ƻ��кɱ��� FLOAT NULL,
    CONSTRAINT pk_���ܼ����ִβ����� PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ����ִ�ֵ��;
CREATE TABLE xopensdb.���ܼ����ִ�ֵ�� (
    ��� INT NOT NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    Ӧ�к��� FLOAT NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    ������������ TINYINT UNSIGNED NULL,
    �ϴθ澯ʱ�� INT NULL,
    ʵ���кɱ��� FLOAT NULL,
    CONSTRAINT pk_���ܼ����ִβ����� PRIMARY KEY (���)
);

delete from ʵʱ���ģʽ 
where
    ���� = 'dzjzround';
insert into ʵʱ���ģʽ values('dzjzround','���ܼ����ִα�',1063,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','���ܼ����ִβ�����','���ܼ����ִ�ֵ��','','','',0,1,'',0,'','','','',0,0,'','');
delete from ʵʱ����ģʽ 
where
    ����� = 'dzjzround';
insert into ʵʱ����ģʽ values('id','���','dzjzround',0,5,0,1,0,0,0,0);
insert into ʵʱ����ģʽ values('name','����','dzjzround',256,10,0,1,10,10,0,0);
insert into ʵʱ����ģʽ values('functype','��������','dzjzround',0,0,0,1,20,20,0,0);
insert into ʵʱ����ģʽ values('roundtype','������','dzjzround',0,5,0,1,30,30,0,0);
insert into ʵʱ����ģʽ values('judgepower','Ͷ���к���','dzjzround',0,8,0,2,40,40,0,0);
insert into ʵʱ����ģʽ values('planpower','�ƻ��к���','dzjzround',0,8,0,2,50,50,0,0);
insert into ʵʱ����ģʽ values('requirepower','Ӧ�к���','dzjzround',0,8,0,2,60,60,0,0);
insert into ʵʱ����ģʽ values('issuepower','�·�Ӧ�к���','dzjzround',0,8,0,1,70,70,0,0);
insert into ʵʱ����ģʽ values('standbypower','ʵ�ʱ����к���','dzjzround',0,8,0,2,80,80,0,0);
insert into ʵʱ����ģʽ values('planstandbypwr','�ƻ������к���','dzjzround',0,8,0,2,90,90,0,0);
insert into ʵʱ����ģʽ values('judgerequire','������������','dzjzround',0,0,0,2,100,100,0,0);
insert into ʵʱ����ģʽ values('lastalarm','�ϴθ澯ʱ��','dzjzround',0,5,0,2,110,110,0,0);
insert into ʵʱ����ģʽ values('planpwrrate','�ƻ��кɱ���','dzjzround',0,8,0,1,120,120,0,0);
insert into ʵʱ����ģʽ values('realpwrrate','ʵ���кɱ���','dzjzround',0,8,0,2,130,130,0,0);

-- �ִ�����
DROP TABLE IF EXISTS xopensdb.���ܼ����ִ����ͱ�;
CREATE TABLE xopensdb.���ܼ����ִ����ͱ� (
    ��� INT NOT NULL,
    ���� VARCHAR(256),
    CONSTRAINT pk_���ܼ����ִ����ͱ� PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ����ִ�����ֵ��;
CREATE TABLE xopensdb.���ܼ����ִ�����ֵ�� (
    ��� INT NOT NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    ʵ��Ͷ�������� INT NULL,
    �ƻ�Ͷ�������� INT NULL,
    ʵ������Ͷ���� FLOAT NULL,
    �ƻ�����Ͷ���� FLOAT NULL,
    Ӧ�к��� FLOAT NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    ʵ��Ͷ������� FLOAT NULL,
    �ƻ�Ͷ������� FLOAT NULL,
    ʵ��Ͷ���ܸ���ռ�� FLOAT NULL,
    �ƻ�Ͷ���ܸ���ռ�� FLOAT NULL,
    ʵ�ʱ��������� INT NULL,
    �ƻ����������� INT NULL,
    CONSTRAINT pk_���ܼ����ִ�����ֵ�� PRIMARY KEY (���)
);

delete from ���ܼ����ִ����ͱ�;
insert into ���ܼ����ִ����ͱ� values(1,'��һ��');
insert into ���ܼ����ִ����ͱ� values(2,'�ڶ���');
insert into ���ܼ����ִ����ͱ� values(3,'������');
insert into ���ܼ����ִ����ͱ� values(4,'������');
insert into ���ܼ����ִ����ͱ� values(5,'������');
insert into ���ܼ����ִ����ͱ� values(6,'������');
insert into ���ܼ����ִ����ͱ� values(7,'��һ��');
insert into ���ܼ����ִ����ͱ� values(8,'�ض���');


delete from ʵʱ���ģʽ 
where
    ���� = 'dzjzroundsta';
insert into ʵʱ���ģʽ values('dzjzroundsta','���ܼ����ִ�����ͳ�Ʊ�',1065,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','���ܼ����ִ����ͱ�','���ܼ����ִ�����ֵ��','','','',0,1,'',0,'','','','',0,0,'','');
delete from ʵʱ����ģʽ 
where
    ����� = 'dzjzroundsta';
insert into ʵʱ����ģʽ values('id','���','dzjzroundsta',0,5,0,1,0,0,0,0);
insert into ʵʱ����ģʽ values('name','����','dzjzroundsta',256,10,0,1,30,30,0,0);
insert into ʵʱ����ģʽ values('judgepower','Ͷ���к���','dzjzroundsta',0,8,0,2,40,40,0,0);
insert into ʵʱ����ģʽ values('planpower','�ƻ��к���','dzjzroundsta',0,8,0,2,50,50,0,0);
insert into ʵʱ����ģʽ values('realfeedernum','ʵ��Ͷ��������','dzjzroundsta',0,5,0,2,60,60,0,0);
insert into ʵʱ����ģʽ values('planfeedernum','�ƻ�Ͷ��������','dzjzroundsta',0,5,0,2,70,70,0,0);
insert into ʵʱ����ģʽ values('realfeederrate','ʵ������Ͷ����','dzjzroundsta',0,8,0,2,80,80,0,0);
insert into ʵʱ����ģʽ values('planfeederrate','�ƻ�����Ͷ����','dzjzroundsta',0,8,0,2,90,90,0,0);
insert into ʵʱ����ģʽ values('requirepower','Ӧ�к���','dzjzroundsta',0,8,0,2,100,100,0,0);
insert into ʵʱ����ģʽ values('standbypower','ʵ�ʱ����к���','dzjzroundsta',0,8,0,2,110,110,0,0);
insert into ʵʱ����ģʽ values('planstandbypwr','�ƻ������к���','dzjzroundsta',0,8,0,2,120,120,0,0);
insert into ʵʱ����ģʽ values('realcomprate','ʵ��Ͷ�������','dzjzroundsta',0,8,0,2,130,130,0,0);
insert into ʵʱ����ģʽ values('plancomprate','�ƻ�Ͷ�������','dzjzroundsta',0,8,0,2,140,140,0,0);
insert into ʵʱ����ģʽ values('realpowerrate','ʵ��Ͷ���ܸ���ռ��','dzjzroundsta',0,8,0,2,150,150,0,0);
insert into ʵʱ����ģʽ values('planpowerrate','�ƻ�Ͷ���ܸ���ռ��','dzjzroundsta',0,8,0,2,160,160,0,0);
insert into ʵʱ����ģʽ values('realstandfn','ʵ�ʱ���������','dzjzroundsta',0,5,0,2,170,170,0,0);
insert into ʵʱ����ģʽ values('planstandfn','�ƻ�����������','dzjzroundsta',0,5,0,2,180,180,0,0);


DROP TABLE IF EXISTS xopensdb.���ܼ����ִ��������;
CREATE TABLE xopensdb.���ܼ����ִ�������� (
    ��� INT NOT NULL,
    ���� VARCHAR(256) NULL,
    �������� INT NULL,
    �����ִ� INT NULL,
    �������� VARCHAR(22) NULL,
    �������� TINYINT UNSIGNED NULL,
    Ͷ�˼ƻ� TINYINT UNSIGNED NULL,
    �������� VARCHAR(24) NULL,
    �й����� VARCHAR(24) NULL,
    ����װ�� INT NULL,
    �������� TINYINT UNSIGNED NULL,
    CONSTRAINT pk_���ܼ����ִ�������� PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ����ִ���ֵ��;
CREATE TABLE xopensdb.���ܼ����ִ���ֵ�� (
    ��� INT NOT NULL,
    ����״̬ TINYINT UNSIGNED NULL,
    ����ѹ��ʵ��״̬ TINYINT UNSIGNED NULL,
    ����ѹ��״̬���� TINYINT UNSIGNED NULL,
    Ƶ�ʻ��ѹʵ�ʶ�ֵ FLOAT NULL,
    ������ʱʵ�ʶ�ֵ FLOAT NULL,
    ��ֵ���� TINYINT UNSIGNED NULL,
    �й����� FLOAT NULL,
    ��Դ�������� TINYINT UNSIGNED NULL,
    ���ط������� TINYINT UNSIGNED NULL,
    �������� TINYINT UNSIGNED NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    װ�ø澯 TINYINT UNSIGNED NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    �ϴθ澯ʱ�� INT NULL,
    ���ھ������� TINYINT UNSIGNED NULL,
    CONSTRAINT pk_���ܼ����ִ���ֵ�� PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ���װ�ò�����;
CREATE TABLE xopensdb.���ܼ���װ�ò����� (
    ��� INT NOT NULL,
    ���� VARCHAR(256) NULL,
    װ������ TINYINT UNSIGNED NULL,
    �������� TINYINT UNSIGNED NULL,
    �����ִ����� INT NULL DEFAULT 0,
    ������վ CHAR(4) NULL,
    CONSTRAINT pk_���ܼ���װ�ò����� PRIMARY KEY (���)
);

DROP TABLE IF EXISTS xopensdb.���ܼ���װ�ò���ֵ��;
CREATE TABLE xopensdb.���ܼ���װ�ò����� (
    ��� INT NOT NULL,
    �澯״̬ TINYINT UNSIGNED NULL,
    ��ѹ�����н��� TINYINT UNSIGNED NULL,
    ��ѹ������ʧ��ԭ�� TINYINT UNSIGNED NULL,
    Ӳѹ�����н��� TINYINT UNSIGNED NULL,
    Ӳѹ������ʧ��ԭ�� TINYINT UNSIGNED NULL,
    ��ֵ���н��� TINYINT UNSIGNED NULL,
    ��ֵ����ʧ��ԭ�� TINYINT UNSIGNED NULL,
    CONSTRAINT pk_���ܼ���װ�ò���ֵ�� PRIMARY KEY (���)
);

DELETE FROM ʵʱ���ģʽ WHERE ���� = 'dzjzdev';
INSERT INTO ʵʱ���ģʽ VALUES('dzjzdev','���ܼ���װ�ò�����',1068,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','���ܼ���װ�ò�����','���ܼ���װ�ò���ֵ��','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM ʵʱ����ģʽ WHERE ����� = 'dzjzdev';
INSERT INTO ʵʱ����ģʽ VALUES('id','���','dzjzdev',0,5,0,1,0,0,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('name','����','dzjzdev',256,10,0,1,10,10,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('ofsta','������վ','dzjzdev',4,10,0,1,20,20,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('type','װ������','dzjzdev',0,0,0,1,30,30,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('alarm','�澯״̬','dzjzdev',0,0,0,2,40,40,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('sfret','��ѹ�����н���','dzjzdev',0,0,0,2,50,50,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('sfreason','��ѹ������ʧ��ԭ��','dzjzdev',0,0,0,2,60,60,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('diret','Ӳѹ�����н���','dzjzdev',0,0,0,2,70,70,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('direason','Ӳѹ������ʧ��ԭ��','dzjzdev',0,0,0,2,80,80,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('sgret','��ֵ���н���','dzjzdev',0,0,0,2,90,90,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('sgreason','��ֵ����ʧ��ԭ��','dzjzdev',0,0,0,2,100,100,0,0);


DROP TABLE IF EXISTS xopensdb.���ܼ���װ�ò����趨��;
CREATE TABLE xopensdb.���ܼ���װ�ò����趨�� (
    ����װ�� INT NOT NULL,
    �ִα�� INT NOT NULL,
    ѹ��ID VARCHAR(256) NULL,
    Ƶ�ʻ��ѹ��ֵID VARCHAR(24) NULL,
    ������ʱ��ֵID VARCHAR(24) NULL,
    �澯�ź�ID VARCHAR(256) NULL,
    �����ź�ID VARCHAR(24) NULL,
    ���ھ���ID VARCHAR(24) NULL,
    CONSTRAINT pk_���ܼ���װ�ò����趨�� PRIMARY KEY (����װ�� , �ִα��)
);

DROP VIEW IF EXISTS xopensdb.���ܼ����ִ�����ͼ;
CREATE VIEW xopensdb.���ܼ����ִ�����ͼ (��� , ���� , �������� , �������� , �����ִ� , �ִ����� , ������վ , ��վ���� , �������� , ��·���� , �������� , Ͷ�˼ƻ� , �������� , �������� , ����װ�� , װ������ , װ������ , �������� , װ�ù����ִ����� , ѹ��ID , Ƶ�ʻ��ѹ��ֵID , ������ʱ��ֵID , �澯�ź�ID , �����ź�ID , Ƶ�ʻ��ѹ����ֵ , ������ʱ����ֵ , �·�Ӧ�к��� , ������ , ���������� , �й�����, ��������, ���ھ���ID) AS
    SELECT 
        a.���,
        a.����,
        a.��������,
        i.����,
        a.�����ִ�,
        h.����,
        b.��վ����,
        g.����,
        a.��������,
        c.����,
        a.��������,
        a.Ͷ�˼ƻ�,
        a.��������,
        b.����,
        a.����װ��,
        e.����,
        e.װ������,
        h.��������,
        e.�����ִ�����,
        f.ѹ��ID,
        f.Ƶ�ʻ��ѹ��ֵID,
        f.������ʱ��ֵID,
        f.�澯�ź�ID,
        f.�����ź�ID,
        h.Ƶ�ʻ��ѹ����ֵ,
        h.������ʱ����ֵ,
        h.�·�Ӧ�к���,
        h.������,
        j.����,
        a.�й�����,
        a.��������,
        f.���ھ���ID
    FROM
        ���ܼ����ִ�������� a
            LEFT JOIN ���ܼ���װ�ò����� e ON e.��� = a.����װ��
            LEFT JOIN ���ܼ���װ�ò����趨�� f ON f.����װ�� = a.����װ�� AND f.�ִα��=a.�����ִ�
            LEFT JOIN ң�Ų����� b ON a.�������� = b.����
            LEFT JOIN ��·������ c ON a.�������� = c.���
            LEFT JOIN ��վ������ g ON g.��� = b.��վ����
            LEFT JOIN ���ܼ����ִβ����� h ON h.��� = a.�����ִ�
            LEFT JOIN ���ܼ������������ i ON i.��� = a.��������
            LEFT JOIN ���ܼ����ִ����ͱ� j ON j.��� = h.������;

DELETE FROM ʵʱ���ģʽ WHERE ���� = 'dzjzrounditem';
INSERT INTO ʵʱ���ģʽ VALUES('dzjzrounditem','���ܼ����ִ����',1062,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','���ܼ����ִ�����ͼ','���ܼ����ִ���ֵ��','���ܼ����ִ��������','���ܼ����ִβ�����','���ܼ���װ�ò�����',0,1,'',0,'','','','',0,0,'','');
DELETE FROM ʵʱ����ģʽ WHERE ����� = 'dzjzrounditem';
INSERT INTO ʵʱ����ģʽ VALUES('id','���','dzjzrounditem',0,5,0,1,0,0,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('name','����','dzjzrounditem',256,10,0,1,10,10,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('strapreal','����ѹ��ʵ��״̬','dzjzrounditem',0,0,0,2,20,20,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('strapjudge','����ѹ��״̬����','dzjzrounditem',0,0,0,2,30,30,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realvalue','Ƶ�ʻ��ѹʵ�ʶ�ֵ','dzjzrounditem',0,8,0,2,40,40,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realtime','������ʱʵ�ʶ�ֵ','dzjzrounditem',0,8,0,2,50,50,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('valuejudge','��ֵ����','dzjzrounditem',0,0,0,2,60,60,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('pvalue','�й�����','dzjzrounditem',0,8,0,2,70,70,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('activejudge','��Դ��������','dzjzrounditem',0,0,0,2,80,80,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planjudge','���ط�������','dzjzrounditem',0,0,0,2,90,90,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('funcjudge','��������','dzjzrounditem',0,0,0,2,100,100,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('judgepower','Ͷ���к���','dzjzrounditem',0,8,0,2,110,110,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('breakstate','����״̬','dzjzrounditem',0,0,0,2,120,120,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('linkedbreak','��������','dzjzrounditem',24,10,0,1,130,130,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planvalue','Ƶ�ʻ��ѹ����ֵ','dzjzrounditem',0,8,0,1,140,140,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('plantime','������ʱ����ֵ','dzjzrounditem',0,8,0,1,150,150,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realvalueid','Ƶ�ʻ��ѹ��ֵID','dzjzrounditem',24,10,0,1,160,160,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('realtimeid','������ʱ��ֵID','dzjzrounditem',24,10,0,1,170,170,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('loadtype','��������','dzjzrounditem',0,0,0,1,180,180,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('strapplan','Ͷ�˼ƻ�','dzjzrounditem',0,0,0,1,190,190,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('strapid','ѹ��ID','dzjzrounditem',256,10,0,1,200,200,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('alarmid','�澯�ź�ID','dzjzrounditem',256,10,0,1,210,210,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('roundid','�����ִ�','dzjzrounditem',0,5,0,1,220,220,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('devalarm','װ�ø澯','dzjzrounditem',0,0,0,2,230,230,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('areaid','��������','dzjzrounditem',0,5,0,1,240,240,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('staid','������վ','dzjzrounditem',4,10,0,1,250,250,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planpower','�ƻ��к���','dzjzrounditem',0,8,0,2,260,260,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('roundname','�ִ�����','dzjzrounditem',256,10,0,1,270,270,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('feedername','��·����','dzjzrounditem',256,10,0,1,280,280,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('roundtype','������','dzjzrounditem',0,5,0,1,290,290,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('standbypower','ʵ�ʱ����к���','dzjzrounditem',0,8,0,2,300,300,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('planstandbypwr','�ƻ������к���','dzjzrounditem',0,8,0,2,310,310,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('actionid','�����ź�ID','dzjzrounditem',24,10,0,1,320,320,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('deviceid','����װ��','dzjzrounditem',0,5,0,1,330,330,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('devicename','װ������','dzjzrounditem',256,10,0,1,340,340,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('functype','��������','dzjzrounditem',0,0,0,1,350,350,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('devtype','װ������','dzjzrounditem',0,0,0,1,360,360,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('pname','�й�����','dzjzrounditem',24,10,0,1,370,370,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('lastalarm','�ϴθ澯ʱ��','dzjzrounditem',0,5,0,2,380,380,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('assocexit','��������','dzjzrounditem',0,0,0,1,390,390,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('exitid','���ھ���ID','dzjzrounditem',24,10,0,1,400,400,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('exitjudge','���ھ�������','dzjzrounditem',0,0,0,2,410,410,0,0);

DROP TABLE IF EXISTS xopensdb.���ܼ�������Ѳ�������;
CREATE TABLE IF NOT EXISTS xopensdb.���ܼ�������Ѳ������� (
    ��� INT NOT NULL,
    ���� VARCHAR(256) NULL,
    ���ڿ�ʼʱ�� INT NULL,
    ���ڽ���ʱ�� INT NULL,
    ���� INT NULL,
    �������� VARCHAR(256) NULL,
    ��վ���� VARCHAR(256) NULL,
    �ִ����� VARCHAR(256) NULL,
    CONSTRAINT pk_���ܼ�������Ѳ������� PRIMARY KEY (���)
);


DELETE FROM ʵʱ���ģʽ 
WHERE
    ���� = 'dzjzptask';
INSERT INTO ʵʱ���ģʽ VALUES('dzjzptask','���ܼ�������Ѳ�������',1066,'scadadb','SCADA','scada',1,1,1,1,2000,'id','','','','','','���ܼ�������Ѳ�������','','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM ʵʱ����ģʽ 
WHERE
    ����� = 'dzjzptask';
INSERT INTO ʵʱ����ģʽ VALUES('id','���','dzjzptask',0,5,0,1,0,0,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('name','����','dzjzptask',256,10,0,1,10,10,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('starttime','���ڿ�ʼʱ��','dzjzptask',0,5,0,1,20,20,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('endtime','���ڽ���ʱ��','dzjzptask',0,5,0,1,30,30,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('interval','����','dzjzptask',0,5,0,1,40,40,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('areacond','��������','dzjzptask',256,10,0,1,50,50,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('stacond','��վ����','dzjzptask',256,10,0,1,60,60,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('roundcond','�ִ�����','dzjzptask',256,10,0,1,70,70,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('lastinspect','�ϴ�Ѳ��ʱ��','dzjzptask',0,5,0,2,80,80,0,0);

DROP TABLE IF EXISTS  xopensdb.���ܼ���װ�ö�ֵ��Ϣ��;
CREATE TABLE IF NOT EXISTS xopensdb.���ܼ���װ�ö�ֵ��Ϣ�� (
    RTU�� SMALLINT NOT NULL,
    ��վ���� CHAR(4) NOT NULL,
    ��Լ�� SMALLINT NOT NULL,
    װ������ SMALLINT NOT NULL,
    ��� SMALLINT NOT NULL,
    ��� SMALLINT NOT NULL,
    ��ֵ CHAR(20) NOT NULL,
    ���� CHAR(160) NOT NULL,
    ��λ CHAR(5) NOT NULL,
    ��ֵ���� CHAR(20) NOT NULL,
    CONSTRAINT pk_���ܼ���װ�ö�ֵ��Ϣ�� PRIMARY KEY (RTU�� , װ������ , ��� , ���)
);

DELETE FROM ʵʱ���ģʽ 
WHERE
    ���� = 'dzjzcdz';
INSERT INTO ʵʱ���ģʽ VALUES('dzjzcdz','���ܼ���װ�ö�ֵ��Ϣ��',1067,'scadadb','SCADA','scada',1,1,1,1,2000,'rtuno','type','groupno','no','','','���ܼ���װ�ö�ֵ��Ϣ��','','','','',0,1,'',0,'','','','',0,0,'','');
DELETE FROM ʵʱ����ģʽ 
WHERE
    ����� = 'dzjzcdz';
INSERT INTO ʵʱ����ģʽ VALUES('rtuno','RTU��','dzjzcdz',0,3,0,1,0,0,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('type','װ������','dzjzcdz',0,3,0,1,10,10,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('groupno','���','dzjzcdz',0,3,0,1,20,20,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('no','���','dzjzcdz',0,3,0,1,30,30,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('value','��ֵ','dzjzcdz',20,10,0,1,40,40,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('desc','����','dzjzcdz',160,10,0,1,50,50,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('unit','��λ','dzjzcdz',5,10,0,1,60,60,0,0);
INSERT INTO ʵʱ����ģʽ VALUES('vtype','��ֵ����','dzjzcdz',20,10,0,1,70,70,0,0);


DELETE FROM DM���ñ� 
WHERE
    �������� = '���ܼ�����������';
INSERT INTO DM���ñ� VALUES('���ܼ�����������','����',1,'');
INSERT INTO DM���ñ� VALUES('���ܼ�����������','���վ',2,'');

delete from DM���ñ� 
where
    �������� = '���ܼ����ִ�����';
insert into DM���ñ� values('���ܼ����ִ�����','������',1,'');
insert into DM���ñ� values('���ܼ����ִ�����','������',2,'');
insert into DM���ñ� values('���ܼ����ִ�����','������',3,'');

delete from DM���ñ� 
where
    �������� = '���ܼ����ִι�������';
insert into DM���ñ� values('���ܼ����ִι�������','��Ƶ',1,'');
insert into DM���ñ� values('���ܼ����ִι�������','��ѹ',2,'');

delete from DM���ñ� 
where
    �������� = '���ܼ��ظ�������';
insert into DM���ñ� values('���ܼ��ظ�������','��Σ�û�',1,'');
insert into DM���ñ� values('���ܼ��ظ�������','��Ҫ�û�',2,'');
insert into DM���ñ� values('���ܼ��ظ�������','�б�������',3,'');
insert into DM���ñ� values('���ܼ��ظ�������','�ͱ�������',4,'');
insert into DM���ñ� values('���ܼ��ظ�������','����ҵ����',5,'');

delete from DM���ñ� 
where
    �������� = '���ܼ���Ͷ�˼ƻ�����';
insert into DM���ñ� values('���ܼ���Ͷ�˼ƻ�����','Ͷ��',1,'');
insert into DM���ñ� values('���ܼ���Ͷ�˼ƻ�����','�˳�',0,'');

delete from DM���ñ� 
where
    �������� = '���ܼ���װ������';
insert into DM���ñ� values('���ܼ���װ������','����ʽ',1,'');
insert into DM���ñ� values('���ܼ���װ������','��ɢʽ',2,'');

delete from DM���ñ� 
where
    �������� = '���ܼ���װ�ù�������';
insert into DM���ñ� values('���ܼ���װ�ù�������','��Ƶ',1,'');
insert into DM���ñ� values('���ܼ���װ�ù�������','��ѹ',2,'');
insert into DM���ñ� values('���ܼ���װ�ù�������','��Ƶ&��ѹ',3,'');

delete from ���������Ա� 
where
    ���� = '���ܼ������������';
insert into ���������Ա� values('���ܼ������������','SCADA','���','���',0,' ',' ',' ',' ',0,0);
delete from ���������Ա� 
where
    �������� = '���ܼ������������';
insert into ���������Ա� values('���','���ܼ������������',5,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('����','���ܼ������������',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('����','���ܼ������������',10,0,15,'���ܼ�����������','��Ŀ','��ֵ','',0,0,'','',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ����ִ����ͱ�';
insert into ���������Ա� values('���ܼ����ִ����ͱ�','SCADA','���','���',0,' ',' ',' ',' ',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ����ִβ�����';
insert into ���������Ա� values('���ܼ����ִβ�����','SCADA','���','���',0,' ',' ',' ',' ',0,0);
delete from ���������Ա� 
where
    �������� = '���ܼ����ִβ�����';
insert into ���������Ա� values('���','���ܼ����ִβ�����',5,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('����','���ܼ����ִβ�����',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִβ�����',10,0,6,'���ܼ������������','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִβ�����',10,0,15,'���ܼ����ִι�������','��Ŀ','��ֵ','',0,0,'','',0,0);
insert into ���������Ա� values('������','���ܼ����ִβ�����',10,0,6,'���ܼ����ִ����ͱ�','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('Ƶ�ʻ��ѹ����ֵ','���ܼ����ִβ�����',8,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('������ʱ����ֵ','���ܼ����ִβ�����',8,0,0,'','','','',0,0,'','',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ����ִ��������';
insert into ���������Ա� values('���ܼ����ִ��������','SCADA','���','���',0,' ',' ',' ',' ',0,0);
delete from ���������Ա� 
where
    �������� = '���ܼ����ִ��������';
insert into ���������Ա� values('���','���ܼ����ִ��������',5,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('����','���ܼ����ִ��������',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִ��������',10,0,6,'���ܼ������������','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('�����ִ�','���ܼ����ִ��������',10,0,6,'���ܼ����ִβ�����','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִ��������',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִ��������',10,0,15,'���ܼ��ظ�������','��Ŀ','��ֵ','',1,0,'','',0,0);
insert into ���������Ա� values('Ͷ�˼ƻ�','���ܼ����ִ��������',10,0,15,'���ܼ���Ͷ�˼ƻ�����','��Ŀ','��ֵ','',1,6,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִ��������',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('�й�����','���ܼ����ִ��������',10,0,0,'','','','',0,0,'','',0,0);


delete from ���������Ա� 
where
    ���� = '���ܼ���װ�ò�����';
insert into ���������Ա� values('���ܼ���װ�ò�����','SCADA','���','���',0,' ',' ',' ',' ',0,0);
delete from ���������Ա� 
where
    �������� = '���ܼ���װ�ò�����';
insert into ���������Ա� values('���','���ܼ���װ�ò�����',5,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('����','���ܼ���װ�ò�����',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('װ������','���ܼ���װ�ò�����',10,0,15,'���ܼ���װ������','��Ŀ','��ֵ','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ���װ�ò�����',10,0,15,'���ܼ���װ�ù�������','��Ŀ','��ֵ','',0,0,'','',0,0);
insert into ���������Ա� values('�����ִ�����','���ܼ���װ�ò�����',5,0,0,'','','','',0,0,'','',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ���װ�ò����趨��';
insert into ���������Ա� values('���ܼ���װ�ò����趨��','SCADA','����װ��','�ִα��',0,' ',' ',' ',' ',0,0);
delete from ���������Ա� 
where
    �������� = '���ܼ���װ�ò����趨��';
insert into ���������Ա� values('����װ��','���ܼ���װ�ò����趨��',10,0,6,'���ܼ���װ�ò�����','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('�ִα��','���ܼ���װ�ò����趨��',5,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('ѹ��ID','���ܼ���װ�ò����趨��',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('Ƶ�ʻ��ѹ��ֵID','���ܼ���װ�ò����趨��',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('�澯�ź�ID','���ܼ���װ�ò����趨��',10,0,0,'','','','',0,0,'','',0,0);
insert into ���������Ա� values('�����ź�ID','���ܼ���װ�ò����趨��',10,0,0,'','','','',0,0,'','',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ����ִ�����ͼ';
insert into ���������Ա� values('���ܼ����ִ�����ͼ','SCADA','�ִ�����','�ִ�����',0,' ',' ',' ',' ',0,0);
delete from ���������Ա� 
where
    �������� = '���ܼ����ִ�����ͼ';
insert into ���������Ա� values('��������','���ܼ����ִ�����ͼ',10,0,6,'���ܼ������������','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('�����ִ�','���ܼ����ִ�����ͼ',10,0,6,'���ܼ����ִβ�����','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִ�����ͼ',10,0,15,'���ܼ��ظ�������','��Ŀ','��ֵ','',1,0,'','',0,0);
insert into ���������Ա� values('Ͷ�˼ƻ�','���ܼ����ִ�����ͼ',10,0,15,'���ܼ���Ͷ�˼ƻ�����','��Ŀ','��ֵ','',1,6,'','',0,0);
insert into ���������Ա� values('����װ��','���ܼ����ִ�����ͼ',10,0,6,'���ܼ���װ�ò�����','����','���','',0,0,'','',0,0);
insert into ���������Ա� values('װ������','���ܼ����ִ�����ͼ',10,0,15,'���ܼ���װ������','��Ŀ','��ֵ','',0,0,'','',0,0);
insert into ���������Ա� values('��������','���ܼ����ִ�����ͼ',10,0,15,'���ܼ����ִι�������','��Ŀ','��ֵ','',0,0,'','',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ���װ�ö�ֵ��Ϣ��';
insert into ���������Ա� values('���ܼ���װ�ö�ֵ��Ϣ��','SCADA','RTU��','RTU��',0,' ',' ',' ',' ',0,0);

delete from ���������Ա� 
where
    ���� = '���ܼ�������Ѳ�������';
insert into ���������Ա� values('���ܼ�������Ѳ�������','SCADA','���','���',0,' ',' ',' ',' ',0,0);


delete from ����ע��� 
where
    ���� = 'dzjzk';
insert into ����ע��� values('dzjzk', '��ѹ�����ִ�Ӧ�к�����Kϵ��',0,1.3,'',0);
insert into ����ע��� values('dzjzmax', '��ѹ������ȼƻ���߸���',0,0,'',0);

delete from �������ͱ� 
where
    ���ͺ� = 2068;
insert into �������ͱ� values(2,2068,'���ܼ���',' ',0,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ');

-- ��ʷ���ݿ�
use xopenshdb;

DROP TABLE IF EXISTS xopenshdb.���ܼ����ִ�����ʷֵ��;
CREATE TABLE xopenshdb.���ܼ����ִ�����ʷֵ�� (
    ʱ�� INT NOT NULL,
    ��� INT NOT NULL,
    ���� VARCHAR(256),
    ����ѹ��ʵ��״̬ TINYINT UNSIGNED NULL,
    �й����� FLOAT NULL,
    �������� TINYINT UNSIGNED NULL,
    Ͷ�˼ƻ� TINYINT UNSIGNED NULL,
    �������� INT NULL,
    ������վ CHAR(4) NULL,
    �ִ����� INT NULL,
    CONSTRAINT pk_���ܼ����ִ�����ʷֵ�� PRIMARY KEY (ʱ�� , ���)
);

DROP TABLE IF EXISTS xopenshdb.���ܼ����ִ���ʷֵ��;
CREATE TABLE xopenshdb.���ܼ����ִ���ʷֵ�� (
    ʱ�� INT NOT NULL,
    ��� INT NOT NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    Ӧ�к��� FLOAT NULL,
    �·��к��� FLOAT NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    ������������ TINYINT UNSIGNED NULL,
    �ƻ��кɱ��� FLOAT NULL,
    ʵ���кɱ��� FLOAT NULL,
    CONSTRAINT pk_���ܼ����ִ���ʷֵ�� PRIMARY KEY (ʱ�� , ���)
);

DROP TABLE IF EXISTS xopenshdb.���ܼ����ִ�������ʷֵ��;
CREATE TABLE xopenshdb.���ܼ����ִ�������ʷֵ�� (
    ʱ�� INT NOT NULL,
    ��� INT NOT NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    ʵ��Ͷ�������� INT NULL,
    �ƻ�Ͷ�������� INT NULL,
    ʵ������Ͷ���� FLOAT NULL,
    �ƻ�����Ͷ���� FLOAT NULL,
    Ӧ�к��� FLOAT NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    ʵ��Ͷ������� FLOAT NULL,
    �ƻ�Ͷ������� FLOAT NULL,
    ʵ��Ͷ���ܸ���ռ�� FLOAT NULL,
    �ƻ�Ͷ���ܸ���ռ�� FLOAT NULL,
    ʵ�ʱ��������� INT NULL,
    �ƻ����������� INT NULL,
    CONSTRAINT pk_���ܼ����ִ�������ʷֵ�� PRIMARY KEY (ʱ�� , ���)
); 

DROP TABLE IF EXISTS xopenshdb.���ܼ���������ʷֵ��;
CREATE TABLE xopenshdb.���ܼ���������ʷֵ�� (
    ʱ�� INT NOT NULL,
    ��� INT NOT NULL,
    Ͷ���к��� FLOAT NULL,
    �ƻ��к��� FLOAT NULL,
    ʵ��Ͷ�������� INT NULL,
    �ƻ�Ͷ�������� INT NULL,
    ʵ������Ͷ���� FLOAT NULL,
    �ƻ�����Ͷ���� FLOAT NULL,
    Ӧ�к��� FLOAT NULL,
    ʵ�ʱ����к��� FLOAT NULL,
    �ƻ������к��� FLOAT NULL,
    ʵ��Ͷ������� FLOAT NULL,
    �ƻ�Ͷ������� FLOAT NULL,
    ʵ��Ͷ���ܸ���ռ�� FLOAT NULL,
    �ƻ�Ͷ���ܸ���ռ�� FLOAT NULL,
    ʵ�ʱ��������� INT NULL,
    �ƻ����������� INT NULL,
    CONSTRAINT pk_���ܼ���������ʷֵ�� PRIMARY KEY (ʱ�� , ���)
);

DROP TABLE IF EXISTS xopenshdb.���ܼ���װ�ö�����;
CREATE TABLE xopenshdb.���ܼ���װ�ö����� (
    ID INT NOT NULL,
    ʱ�� INT NOT NULL,
    ���� VARCHAR(256) NULL,
    Ͷ���к��� FLOAT NULL,
    �����к��� FLOAT NULL,
    �����к�ռ�� FLOAT NULL,
    ���������� INT NULL,
    CONSTRAINT pk_���ܼ���װ�ö����� PRIMARY KEY (ID)
);

DROP TABLE IF EXISTS xopenshdb.���ܼ����ִ������;
CREATE TABLE xopenshdb.���ܼ����ִ������ (
    ����ID INT NOT NULL,
    ʱ�� INT NOT NULL,
    �ִ���ID INT NOT NULL,
    ��·���� VARCHAR(256) NULL,
    �к��� FLOAT NULL,
    �������� INT NULL,
    �������� VARCHAR(256),
    ������ INT NULL,
    ���������� VARCHAR(256),
    �����ź�ID VARCHAR(24),
    �����ź����� VARCHAR(256),
    װ��ID INT NULL,
    װ������ VARCHAR(256),
    װ������ TINYINT UNSIGNED,
    �ִι������� TINYINT UNSIGNED,
    ������վ CHAR(4),
    CONSTRAINT pk_���ܼ����ִ������ PRIMARY KEY (����ID , ʱ�� , �ִ���ID)
);

DROP TABLE IF EXISTS xopenshdb.���ܼ���Ѳ������;
CREATE TABLE IF NOT EXISTS xopenshdb.���ܼ���Ѳ������ (
    ID INT NOT NULL,
    Ѳ��ģʽ TINYINT UNSIGNED NULL,
    ���������� INT NULL,
    Ѳ��ʱ�� INT NOT NULL,
    ӦͶδͶ INT NULL,
    Ӧ��δ�� INT NULL,
    װ���쳣 INT NULL,
    ��ֵ��ͬ INT NULL,
    CONSTRAINT pk_���ܼ���Ѳ������ PRIMARY KEY (ID)
);

DROP TABLE IF EXISTS xopenshdb.���ܼ���Ѳ�������;
CREATE TABLE IF NOT EXISTS xopenshdb.���ܼ���Ѳ������� (
    ID INT NOT NULL,
    �ִ����� INT NOT NULL,
    ����״̬ TINYINT UNSIGNED NULL,
    ���� FLOAT NULL,
    ʵ�ʶ�ֵ VARCHAR(16) NULL,
    �ƻ���ֵ VARCHAR(16) NULL,
    ʵ��ѹ��״̬ TINYINT UNSIGNED NULL,
    �ƻ�ѹ��״̬ TINYINT UNSIGNED NULL,
    ѹ��״̬���� TINYINT UNSIGNED NULL,
    װ�ø澯 TINYINT UNSIGNED NULL,
    �ܸ澯 TINYINT UNSIGNED NULL,
    CONSTRAINT pk_���ܼ���Ѳ������� PRIMARY KEY (ID , �ִ�����)
);

-- �洢����
use xopenshdb;
DELIMITER //
DROP PROCEDURE IF EXISTS dzjz_delete_old_records //
CREATE PROCEDURE dzjz_delete_old_records()
BEGIN
  DELETE FROM xopenshdb.���ܼ����ִ�����ʷֵ�� WHERE ʱ�� < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.���ܼ����ִ���ʷֵ�� WHERE ʱ�� < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.���ܼ����ִ�������ʷֵ�� WHERE ʱ�� < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.���ܼ���������ʷֵ�� WHERE ʱ�� < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.���ܼ���װ�ö����� WHERE ʱ�� < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
  DELETE FROM xopenshdb.���ܼ����ִ������ WHERE ʱ�� < UNIX_TIMESTAMP(DATE_SUB(NOW(), INTERVAL 2 YEAR));
END //

DELIMITER ;

DROP EVENT IF EXISTS dzjz_delete_old_records_event;
CREATE EVENT dzjz_delete_old_records_event
ON SCHEDULE EVERY 1 DAY STARTS (TIMESTAMP(CURRENT_DATE) + INTERVAL 2 HOUR)
DO CALL dzjz_delete_old_records();

-- ����Ϊ���Լ�¼
USE xopensdb;

DELETE FROM ���ܼ������������;
INSERT INTO ���ܼ������������ VALUES(1,'����',1);
INSERT INTO ���ܼ������������ VALUES(2,'���վ1',2);
INSERT INTO ���ܼ������������ VALUES(3,'���վ2',2);

DELETE FROM ���ܼ�������վ������;
INSERT INTO ���ܼ�������վ������ VALUES('xy',2);
INSERT INTO ���ܼ�������վ������ VALUES('zj',3);

delete from ���ܼ����ִβ�����;
insert into ���ܼ����ִβ����� values(1,'��Ƶ���ص�һ��',1,1,1,49,300,100);
insert into ���ܼ����ִβ����� values(2,'��Ƶ���صڶ���',1,1,2,49,300,100);
insert into ���ܼ����ִβ����� values(3,'��Ƶ���ص�����',1,1,3,49,300,100);
insert into ���ܼ����ִβ����� values(4,'��Ƶ���ص�����',1,1,4,49,300,100);
insert into ���ܼ����ִβ����� values(5,'��Ƶ���ص�����',1,1,5,49,300,100);
insert into ���ܼ����ִβ����� values(6,'��Ƶ���ظ�����',1,1,6,49,300,100);
insert into ���ܼ����ִβ����� values(7,'��Ƶ������һ��',1,1,7,49,300,100);
insert into ���ܼ����ִβ����� values(8,'��Ƶ�����ض���',1,1,8,49,300,100);
insert into ���ܼ����ִβ����� values(9,'��ѹ���ص�һ��',1,2,1,0.8,300,100);
insert into ���ܼ����ִβ����� values(10,'��ѹ���صڶ���',1,2,2,0.85,300,100);
insert into ���ܼ����ִβ����� values(11,'��ѹ������һ��',1,2,7,0.85,300,100);
insert into ���ܼ����ִβ����� values(12,'��ѹ�����ض���',1,2,8,0.85,300,100);


delete from ���ܼ����ִ��������;
insert into ���ܼ����ִ�������� values(1,'��·1',2,1,'Lxy263',5,1,'Pxy_122_2_1YX',"Pxy_121_1_3YC",1,1);
insert into ���ܼ����ִ�������� values(2,'��·2',2,2,'Lxy263',5,1,'Pxy_122_2_2YX',"Pxy_121_1_3YC",2,2);

insert into ���ܼ����ִ�������� values(3,'��·3',2,1,'Lxy263',4,1,'Pxy_121_2_8YX',"Pxy_121_1_3YC",121,3);
insert into ���ܼ����ִ�������� values(4,'��·4',2,1,'Lxy263',4,1,'Pxy_121_2_9YX',"Pxy_121_1_3YC",121,4);
insert into ���ܼ����ִ�������� values(5,'��·5',2,2,'Lxy263',3,1,'Pxy_121_2_10YX',"Pxy_121_1_3YC",121,5);
insert into ���ܼ����ִ�������� values(6,'��·6',2,2,'Lxy263',3,1,'Pxy_121_2_11YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(7,'��·7',2,3,'Lxy263',2,1,'Pxy_121_2_12YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(8,'��·8',2,3,'Lxy263',2,1,'Pxy_121_2_13YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(9,'��·9',2,4,'Lxy263',1,1,'Pxy_121_2_14YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(10,'��·10',2,4,'Lxy263',1,1,'Pxy_121_2_15YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(11,'��·11',2,5,'Lxy263',5,1,'Pxy_121_2_16YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(12,'��·12',2,5,'Lxy263',5,1,'Pxy_121_2_17YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(13,'��·13',2,6,'Lxy263',5,1,'Pxy_121_2_18YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(14,'��·14',2,6,'Lxy263',5,1,'Pxy_121_2_19YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(15,'��·15',2,7,'Lxy263',5,1,'Pxy_121_2_20YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(16,'��·16',2,7,'Lxy263',5,1,'Pxy_121_2_21YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(17,'��·17',2,8,'Lxy263',5,1,'Pxy_121_2_22YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(18,'��·18',2,8,'Lxy263',5,1,'Pxy_121_2_23YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(19,'��·19',2,9,'Lxy263',5,1,'Pxy_121_2_24YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(20,'��·20',2,9,'Lxy263',5,1,'Pxy_121_2_25YX',"Pxy_121_1_3YC",121,1);
insert into ���ܼ����ִ�������� values(21,'��·21',2,10,'Lxy263',5,1,'Pxy_121_2_26YX',"Pxy_121_1_3YC",121,1);


delete from ���ܼ���װ�ò�����;
insert into ���ܼ���װ�ò����� values(1,'���ܼ���װ��1',2,3,1,'xy');
insert into ���ܼ���װ�ò����� values(2,'���ܼ���װ��2',2,3,1,'xy');
insert into ���ܼ���װ�ò����� values(121,'���ܼ���װ��121',1,3,1,'xy');

delete from ���ܼ���װ�ò����趨��;
insert into ���ܼ���װ�ò����趨�� values(1,1,'Pxy085500000000000140Pg','111:17:31','111:17:32','Pxy085500000000000148Pg','Pxy061300000000000175Pg','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(2,1,'Pxy085500000000000140Pg','111:17:31','111:17:32','Pxy085500000000000148Pg','Pxy061300000000000175Pg','1:3:1');

insert into ���ܼ���װ�ò����趨�� values(121,1,'Pxy_121_2_34YX','1:3:1','1:3:2','Pxy_121_5_1YX','Pxy_121_4_2YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,2,'Pxy_121_2_35YX','1:3:3','1:3:4','Pxy_121_5_1YX','Pxy_121_4_3YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,3,'Pxy_121_2_36YX','1:3:5','1:3:6','Pxy_121_5_1YX','Pxy_121_4_4YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,4,'Pxy_121_2_37YX','1:3:7','1:3:8','Pxy_121_5_1YX','Pxy_121_4_5YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,5,'Pxy_121_2_38YX','1:3:9','1:3:10','Pxy_121_5_1YX','Pxy_121_4_6YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,6,'Pxy_121_2_39YX','1:3:11','1:3:12','Pxy_121_5_1YX','Pxy_121_4_7YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,7,'Pxy_121_2_40YX','1:3:14','1:3:15','Pxy_121_5_1YX','Pxy_121_4_8YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,8,'Pxy_121_2_41YX','1:3:16','1:3:17','Pxy_121_5_1YX','Pxy_121_4_9YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,9,'Pxy_121_2_42YX','1:3:20','1:3:21','Pxy_121_5_1YX','Pxy_121_4_11YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,10,'Pxy_121_2_43YX','1:3:22','1:3:23','Pxy_121_5_1YX','Pxy_121_4_12YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,11,'Pxy_121_2_44YX','1:3:24','1:3:25','Pxy_121_5_1YX','Pxy_121_4_13YX','1:3:1');
insert into ���ܼ���װ�ò����趨�� values(121,12,'Pxy_121_2_45YX','1:3:26','1:3:27','Pxy_121_5_1YX','Pxy_121_4_14YX','1:3:1');

delete from ���ܼ�������Ѳ�������;
INSERT INTO ���ܼ�������Ѳ������� VALUES (1, '����1', 1672531200, 0, 10, '', '', '');

delete from ���ܼ���װ�ö�ֵ��Ϣ��;
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (1, 'xy', 93, 111, 17, 32, '300', '��Ƶ����ʱ��','0',1);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (1, 'xy', 93, 111, 17, 31, '49', '��Ƶ����Ƶ��','Hz',0);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (2, 'xy', 93, 111, 17, 32, '300', '��Ƶ����ʱ��','0',1);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (2, 'xy', 93, 111, 17, 31, '49', '��Ƶ����Ƶ��','Hz',0);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (122, 'xy', 93, 111, 17, 32, '300', '��Ƶ����ʱ��','0',1);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (122, 'xy', 93, 111, 17, 31, '49', '��Ƶ����Ƶ��','Hz',0);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (122, 'xy', 93, 111, 17, 34, '300', '��Ƶ����ʱ��','0',1);
INSERT INTO ���ܼ���װ�ö�ֵ��Ϣ�� VALUES (122, 'xy', 93, 111, 17, 33, '0.85', '��ѹ���ص�ѹ','Hz',0);

commit;




