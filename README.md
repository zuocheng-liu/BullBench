# BullBench

## 什么是 BullBench ？ (what)

*   一个可以对web服务进行压力测试的工具
*   最大特点，BullBench 可以读取 nginx access log 中请求，并将其发送给web服务。
*   也可以读取自定义文件，使用正则匹配和替换，定制请求，发送给web服务。
*   可以同时模拟15000并发度请求（具体数值受限于系统配置）
*   和bullbench类似的软件有 webbench、tcpcopy、jmeter等，与它们相比，bullbench有自己独特的地方，比webbench功能多，比tcpcopy操作简单，比jmeter编写测试用例的学习成本更低。

## 代码托管地址 : (where)

<https://github.com/zuocheng-liu/BullBench>

## 为何编写这个软件？ （why）

主要原因是，我曾经在查找一个系统内存泄漏问题时，遇到了一些阻碍：

*   线下压测试时，要尽可能模拟线上请求
*   理想方案是使用tcpcopy，复制线上流量，但是目前没有现成的tcpcopy环境

为了克服这些阻碍，就写一个简单的工具，先是读取nginx access log， 提取请求uri，然后模拟1000个客户端发送给Web后台。

这个工具经过再完善和变得更通用之后，bullbench诞生了。

## 什么时候用呢？ 以及软件的局限有哪些。（when）

使用场景：

*   压力测试时
*   追踪bug，复现线上问题

局限：

*   如果没有请求日志，无法使用bullbench进行压力测试
*   只能模拟发送HTTP GET请求
*   不能够处理 HTTP 3XX 重定向服务
*   其他

## 作者联系方式： 刘作程，career@zuocheng.net (who)

## 如何编译？ （how ）

进入源代码目录，执行make

## 如何使用？

执行 ./bin/bullbench -h 有详细说明和使用实例

```
        BoolBench 1.0
    
        -f <filename> 包含请求的文件，可以是nginx access log，也可以是自定义的数据文件
        -t <num>      请求文件的类型, 1 是 nginx access log, 0 其他 其他, 默认是 1
        -u <url>      请求的url前缀, 不支持 https, 比如 http://www.bullsoft.org
        -H <host>     HTTP请求头字段Host, 默认是 NULL
        -c <num>      并发请求的线程数, 默认是 1000
        -r <regex>    正则表达式,用于提取请求文件中特定的内容. 必须和参数'-t 0' 搭配。
        -o <string>   正则变量拼接后的字符串, 支持 $0-$9
        -h            显示帮助信息
    实例1:  ./bullbench -f /var/log/nginx/access.log -u http://127.0.0.1:8080
    实例2:  ./bullbench -f /var/log/nginx/access.log -u http://127.0.0.1:8080 -H www.bullsoft.org
    实例3:  ./bullbench -f /var/log/nginx/access.log -u http://127.0.0.1:8080 -t 0 -r "[a-z]*([0-9]+)([a-z]*)" -o "/display?a=\$1&b=\$2"
```    

## 一些参数（how much？ how many？）

并发线程数设置多少，依据系统情况而定。
