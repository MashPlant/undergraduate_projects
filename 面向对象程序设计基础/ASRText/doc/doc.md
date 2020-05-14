OOP 个人作业文档

Project: ASRText ((III) Project: Speech Input Tool)

Author: 李晨昊 2017011466

## 基本说明

ASRText是一个基于Qt的文本编辑器。它的语音识别部分调用了迅飞的语音识别API。

由于迅飞的API的限制，它只能在linux上运行。(事实上迅飞也有windows下的API，但是我没有可以用于调试的机器)

因为不方便为GUI窗口提供输入，所以testcase文件夹为空。

## 如何编译运行

ASRText的编译基于qmake。
```
cd src
mkdir build && cd build
qmake ..
make
./ASRText # run
```
最后一步运行时可能会报告运行错误，找不到动态链接库，我的处理方法是把动态链接库所在的文件夹加入动态链接库的搜索路径中。(我也不知道有没有什么别的更好的解决方案)

```
cd /etc/ld.so.conf.d/
sudo vi libmsc.conf
# then write the follwing line into libmsc.conf
# [location of ASRText]/src/iFlyASR
```
然后运行即可

## 如何使用&功能

ctrl+B 开始语音识别，ctrl+E结束语音识别。(请保持网络畅通)

其余的功能是一个文本编辑器的真子集。简单的保存，复制粘贴等功能都实现了。

## 架构&分析

* [asrrecognizer.h](../src/asrrecognizer.h)
    * class ASRException 描述语音识别过程中发生的异常
    * class ASRRecognizer 接受一个音频文件，调用迅飞接口，返回识别结果(或者抛出ASRException异常)
* [audiorecorder.h](../src/audiorecorder.h)
    * class AudioRecorder 调用Qt的语音输入模块，实时录音并保存为wav文件
        * struct WavHeader 描述了wav文件的文件头
* [state.h](../src/state.h)
    * class State 描述文本编辑器的状态，并提供状态变化的监听接口
* [mainwindow.h](../src/mainwindow.h)
    * Ui::MainWindow 利用qml生成的界面类
    * class MainWindow 描述界面逻辑。利用Qt自带的QTextEdit实现编辑器。调用ASRRecognizer和AudioRecorder进行语音转文字

