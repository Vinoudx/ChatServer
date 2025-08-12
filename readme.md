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


##### 实现细节
1. 继承形式的单例模式，注意shared_ptr用裸指针reset(否则make_shared无法访问到子类构造函数)，在类中声明友元类等问题
2. 为了防止sql注入，需要加上预编译sql语句的实现
3. 服务器将消息推送给用户，需要常连接，需要将已建立连接的用户的连接保存下来。因为多线程服务器不确定用户在哪个线程中被处理，需要对映射关系加锁

##### 一些问题
1. 实现业务处理和网络服务的两种方法，面向接口和面向回调
2. 注意registeHandler的实现方式，支持对各种方法的注册
3. 在找不到handler的时候，返回一个默认handler，其中打印日志
4. 与业务处理类似，对于数据库的操作也需要跟业务解耦
5. 什么是DRM，简单来说是把一个数据表中的字段整合为一个类，实现对数据库操作的对象化
6. 注意多线程环境下对用户的登录等信息可能存在竞态条件

