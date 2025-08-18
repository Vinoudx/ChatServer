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
7. 负载均衡的作用：将用户的请求均衡到多台server上，通过心跳保持和server的连接，便于水平拓展server
8. 集群服务器之间使用redis进行通信，而不是两两建立tcp连接通信，使用发布订阅的redis消息队列
9. redis上下文相当于一个redis客户端，其中subscribe客户端是阻塞状态的所以还需要一个public客户端

##### 二些问题
###### 数据安全（可尝试实现）
json明文传输，特别是密码需要加密传输
1. 对称加密 AES
2. 非对称加密 RSA
3. 结合，使用对称加密方式加密数据，使用非对称加密加密对称加密的密钥。对称加密密钥由客户端生成，客户端使用服务器公钥加密，服务器用自己的私钥解密

###### 历史数据存储
1. 本地存储
2. 云端存储，短期内数据放数据库，长期数据dump到磁盘文件

###### 客户端消息如何按序显示（重点）
用户上线时通知所有好友，用户的send_seq和recv_seq就是0开始的，好友收到通知后send_seq和recv_seq也是0开始的
用户下线时通知所有好友，好友把这个信息删掉
1. 对于一对一通信：对每个通信的两端（即两个用户）都各自维护send_seq和recv_seq，给消息加上seq，实现按序显示
2. 对于群组通信：只保证同一个发送方的按序，不同发送方就按时间戳排
3. 为什么不用时间戳？信息在排序周期间隔外乱序到达；集群服务器和客户端时间互相不统一不权威
4. 若等待乱序消息超时，方法：向服务器请求发送方重发相应序号的消息，多次超时后判定接受失败，显示后续消息

###### redis不稳定，挂了怎么办
1. 挂了正常，没有服务不会挂
2. 通过设置一些上限保证服务不挂

###### 服务器如何感知客户端存在
使用心跳机制

###### 如何保证消息可靠传输
1. 业务层实现消息确认机制
2. 为什么tcp不能实现可靠传输：我们靠send函数返回值判断是否发送成功，
首先send函数调用成功只表示数据拷贝到内核发送缓冲区不保证到达，
比如网络条件非常差甚至意外断开连接，tcp超时重传有次数上限，则会发送失败

###### 在原基础上添加了一对一通信的消息按序显示，好友上线与下线的通知，显示好友列表和群组列表
