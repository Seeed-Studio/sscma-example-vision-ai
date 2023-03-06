
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
### 准备数据集

请参考[这里](https://docs.edgeimpulse.com/docs/edge-impulse-studio/data-acquisition)来准备数据集。

### 训练模型

#### 训练模型

```bash
cd EdgeLab
conda activate edgelab
python tools/train.py mmcls configs/accelerometer/3axes_accelerometer_62.5Hz_1s_classify.py
```


## 转换模型

**第1步:** 将模型转换为TensorFlow Lite。

```bash
cd EdgeLab
conda activate edgelab
python tools/torch2tflite.py mmcls configs/accelerometer/3axes_accelerometer_62.5Hz_1s_classify.py --weights work_dirs/3axes_accelerometer_62.5Hz_1s_classify/exp1/latest.pth  --tflite_type int8
```

现在我们将把生成的TFLite文件转换为UF2文件，这样我们就可以直接把UF2文件闪存到Grove - Vision AI Module和SenseCAP A1101中。

**第2步:** 将TFLite文件转换为UF2文件。

```bash
cd edgelab-example-vision-ai
python tools/ufconv/uf2conv.py -f GROVEAI -t 1 -c <path to model.tflite> -o model.uf2
```

```{note}
注意：路径中的exp1是第一次训练是生成的，如果您多次训练expx会依次累加。
```

## 准备固件

### 编译固件

```bash
cd edgelab-example-vision-ai
make HW=grove_vision_ai APP=fomo
make flash
```
### 生成固件

```bash
cd edgelab-example-vision-ai
python3 tools/ufconv/uf2conv.py -t 0 -c tools/image_gen_cstm/output/output.img -o firmware.uf2
```

## 将模型和固件烧录到Grove - Vision AI Module和SenseCAP A1101

以下是如何将先前生成的固件（firmware.uf2）和模型文件（model.uf2）闪存到Grove - Vision AI Module和SenseCAP A1101的说明。


**Step 1.** 通过USB Type-C 将Grove - Vision AI Module/ SenseCAP A1101连接到PC

![45](../_static/vision_ai/images/45.png)

**Step 2.** 双击 `boot` 按键进入 **DFU** 模式

![46](../_static/vision_ai/images/46.png)

**Step 3:** 你会看到一个新的存储设备 **GROVEAI** 是 **Grove - Vision AI Module** **VISIONAI** 是 **SenseCAP A1101**

![62](../_static/vision_ai/images/62.jpg)

**Step 4:** 将生成 **firmware.uf2** 复制到对应的存储设备中, 重复进入DFU模式后将 **model.uf2**复制到对应的存储设备中

```note
注意: 你需要分开两次进入 DFU 模式，一次烧录固件，一次烧录模型。
当GROVEAI设备消失时，表面固件或模型已经烧录完成。
```


## 运行模型
