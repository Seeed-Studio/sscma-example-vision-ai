
# 在Grove上训练和部署一个fomo模型 - Vision AI

## 简介

本教程展示了如何在Grove - Vision AI上训练和部署一个读表检测模型。

## 前提条件

### 硬件
- 使用Linux的PC，也支持WSL2。
- Grove - Vision AI
- USB电缆

### 软件
- [EdgeLab](https://edgelab.readthedocs.io/zh_CN/latest/)
- [Edgelab-example-vision-ai](https://github.com/Seeed-Studio/edgelab-example-vision-ai)

```{注意}
请确保你已经安装了EdgeLab的最新版本
```

## 训练模型

### 准备好数据集

使用提供的数据集来训练模型。


### 准备好配置文件


### 训练模型



## 转换模型

**第1步:** 将模型转换为TensorFlow Lite。


现在我们将把生成的TFLite文件转换为UF2文件，这样我们就可以直接把UF2文件闪存到Grove - Vision AI Module和SenseCAP A1101中。

**第2步:** 将TFLite文件转换为UF2文件。

```bash
cd edgelab-example-vision-ai
python tools/uf2conv/ -f GROVEAI -t 1 -c <path to model.tflite> -o model.uf2
```

## 将模型和固件烧录到Grove - Vision AI Module和SenseCAP A1101

以下是如何将先前生成的固件（firmware.uf2）和模型文件（model.uf2）闪存到Grove - Vision AI Module和SenseCAP A1101的说明。


**Step 1.** 通过USB Type-C 将Grove - Vision AI Module/ SenseCAP A1101连接到PC


<div align=center><img width=1000 src="../_static/vision_ai/images/45.png"/></div>

**Step 2.** 双击 `boot` 按键进入 **DFU** 模式

<div align=center><img width=1000 src="../_static/vision_ai/images/46.png"/></div>

**Step 3:** 你会看到一个新的存储设备 **GROVEAI** 是 **Grove - Vision AI Module** **VISIONAI** 是 **SenseCAP A1101**

<div align=center><img width=500 src="../_static/vision_ai/images/62.jpg"/></div>

**Step 4:** 将生成 **firmware.uf2** 复制到对应的存储设备中, 重复进入DFU模式后将 **model.uf2**复制到对应的存储设备中

```note
注意: 你需要分开两次进入 DFU 模式，一次烧录固件，一次烧录模型。
当GROVEAI设备消失时，表面固件或模型已经烧录完成。
```


## 运行模型
**Step 1:** 完成上述的模型还有固件烧录工作后, 打开 [链接](https://files.seeedstudio.com/grove_ai_vision/index.html)
        
**Step 2:** 点击 **Connect** 按钮. 你会看到一个弹出窗口. 选择 **Grove AI - （配对）** 然后点击**Connect**
<div align=center><img width=800 src="../_static/vision_ai/images/13.jpg"></div>
        
<div align=center><img width=400 src="../_static/vision_ai/images/12.png"></div>
        
如果连接成功，你会看到以下界面
        
<div align=center><img width=800 src="../_static/vision_ai/images/14.png"></div>
        
