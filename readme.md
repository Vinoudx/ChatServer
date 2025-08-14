##### 表结构
######  User表
|字段名称|字段类型|字段说明|约束|
|:---:|:---:|:---:|:---:|
|id|INT|用户id|PRIMARY KEY, AUTOINCREASEMENT|
|name|VARCHAR(50)|用户名|NOT NULL, UNIQUE|
|password|VARCHAR(50)|用户密码|NOT NULL|
|state|ENUM('online', 'offline')|登录状态|DEFAULT 'offline'|

###### Friend表
|字段名称|字段类型|字段说明|约束|
|:---:|:---:|:---:|:---:|
|userid|INT|用户id|NOT NULL, 联合主键|
|friendid|INT|用户id|NOT NULL, 联合主键|

###### AllGroup表
|字段名称|字段类型|字段说明|约束|
|:---:|:---:|:---:|:---:|
|id|INT|用户id|PRIMARY KEY, AUTOINCREASEMENT|
|groupname|VARCHAR(50)|组名|NOT NULL|
|groupdesc|VARCHAR(200)|组功能描述|DEFAULT ''|

###### GroupUser表
|字段名称|字段类型|字段说明|约束|
|:---:|:---:|:---:|:---:|
|groupid|INT|组id|PRIMARY KEY|
|userid|int|组员id|NOT NULL|
|grouprole|ENUM('creator' 'normal')|组内角色|DEFAULT 'normal'|

###### OfflineMessage表
|字段名称|字段类型|字段说明|约束|
|:---:|:---:|:---:|:---:|
|userid|int|用户id|NOT NULL|
|message|VARCHAR(500)|离线消息(json)|NOT NULL|

##### json字段
###### 登录
{"msgid":2,"id":1,"name":"huangyuming","pwd":"sbhym"}
{"msgid":3,"erron":0,"id":1,"name":"huangyuming"}
{"msgid":3,"erron":1,"errinfo":"already online"}
###### 注册
{"msgid":0,"name":"huangyuming","pwd":"sbhym"}

###### 聊天
{"msgid":4,"id":1,"from":"huangyuming","to":2,"msg":"xxx","time":"xxx"}

###### 加好友
{"msgid":5,"id":1,"friendid":"2"}

###### 创建组
{"msgid":6,"id":1,"gname":"name","gdesc":"desc"}

###### 加入组
{"msgid":7,"id":1,"gid":"2"}

###### 组聊天
{"msgid":8,"id":1,"gid":"2","msg":"xxx","time":"xxx"}


##### 实现细节
1. 继承形式的单例模式，注意shared_ptr用裸指针reset(否则make_shared无法访问到子类构造函数)，在类中声明友元类等问题
2. 为了防止sql注入，需要加上预编译sql语句的实现
3. 服务器将消息推送给用户，需要常连接，需要将已建立连接的用户的连接保存下来。因为多线程服务器不确定用户在哪个线程中被处理，需要对映射关系加锁
4. 需要处理客户端异常退出的情况，否则根据登录逻辑客户将永远无法再次登录
5. 在一对一的通信中，小心连接映射表的多线程操作安全

##### 一些问题
1. 实现业务处理和网络服务的两种方法，面向接口和面向回调
2. 注意registeHandler的实现方式，支持对各种方法的注册
3. 在找不到handler的时候，返回一个默认handler，其中打印日志
4. 与业务处理类似，对于数据库的操作也需要跟业务解耦
5. 什么是DRM，简单来说是把一个数据表中的字段整合为一个类，实现对数据库操作的对象化
6. 注意多线程环境下对用户的登录等信息可能存在竞态条件

