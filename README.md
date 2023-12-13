# 地铁导航线路
简短介绍一下功能 

## 实现方式
先介绍一下读入blabla

主要使用了dijkstra的单源最短路算法 

在考虑换乘时，我们的方法是对于同一站点所在不同线路的情况，将一个站点拆分成多个点，例如出在1号线的a点，处在2号线的a点，并且在他们之间连边以表示换乘所需要的时间。

最后在这样一个包含虚点的图上跑dijkstra算法 同时记录前驱 表示谁更新的自己 最后输出路径时不断寻找前驱 直到找到终点 

## 编译环境 
本项目所有代码使用c++11完成 

输入文件，输出文件，main均使用utf-8编码 如出现乱码 请确保您的使用utf-8打开 并使用utf-8编码输出 
（建议使用vscode打开 并在tasks.json中的args加入类似"-fexec-charset=UTF-8"的指令）

## 使用方式 


## 使用实例 

