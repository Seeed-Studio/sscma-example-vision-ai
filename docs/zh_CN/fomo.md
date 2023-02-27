
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
你可以使用你熟悉的数据集获取方式， 以Roboflow提供的[mask-detection](https://universe.roboflow.com/compvis-final-project/mask-detection-m3skq)为例，下载数据集并解压到`EdgeLab/datasets`目录下。

```bash
cd EdgeLab
mkdir -p datasets
cd datasets
wget https://files.seeedstudio.com/wiki/Edgelab/coco_mask.zip
unzip coco_mask.zip

```

### 准备配置文件

```python
_base_ = '../_base_/default_runtime.py'

num_classes=2
model = dict(
    type='Fomo',
    backbone=dict(type='MobileNetV2', widen_factor=0.35, out_indices=(2, )),
    head=dict(
        type='Fomo_Head',
        input_channels=16,
        num_classes=num_classes,
        middle_channels=[96, 32],
        act_cfg='ReLU6',
        loss_cls=dict(type='BCEWithLogitsLoss',
                      reduction='none',
                      pos_weight=40),
        loss_bg=dict(type='BCEWithLogitsLoss', reduction='none'),
        cls_weight=40,
    ),
)

# dataset settings
dataset_type = 'FomoDatasets'
data_root = './work_dirs/datasets/coco_mask'
height=96
width=96
batch_size=16
workers=4


train_pipeline = [
    dict(type='RandomResizedCrop', height=height, width=width, scale=(0.90, 1.1),
         p=1),
    dict(type='Rotate', limit=20),
    dict(type='RandomBrightnessContrast',
         brightness_limit=0.2,
         contrast_limit=0.2,
         p=0.5),
    dict(type='HorizontalFlip', p=0.5),
]
test_pipeline = [dict(type='Resize', height=height, width=width, p=1)]

data = dict(samples_per_gpu=batch_size,
            workers_per_gpu=workers,
            train_dataloader=dict(collate=True),
            val_dataloader=dict(collate=True),
            train=dict(type=dataset_type,
                       data_root=data_root,
                       ann_file='train/_annotations.coco.json',
                       img_prefix='train',
                       pipeline=train_pipeline),
            val=dict(type=dataset_type,
                     data_root=data_root,
                     test_mode=True,
                     ann_file='valid/_annotations.coco.json',
                     img_prefix='valid',
                     pipeline=test_pipeline),
            test=dict(type=dataset_type,
                      data_root=data_root,
                      test_mode=True,
                      ann_file='valid/_annotations.coco.json',
                      img_prefix='valid',
                      pipeline=test_pipeline))

# optimizer
lr=0.001
epochs=300
optimizer = dict(type='Adam', lr=lr, weight_decay=0.0005)

optimizer_config = dict(grad_clip=dict(max_norm=35, norm_type=2))
# learning policy
lr_config = dict(policy='step',
                 warmup='linear',
                 warmup_iters=30,
                 warmup_ratio=0.000001,
                 step=[100, 200, 250])
# runtime settings
evaluation = dict(interval=1, metric=['mAP'], fomo=True)
find_unused_parameters = True
```
保存为`EdgeLab/configs/fomo/fomo_mobilenetv2_mask.py`。

### 训练模型

#### 下载预训练模型

```bash
cd EdgeLab
mkdir -p work_dirs/pretrain/ && cd work_dirs/pretrain
wget  https://github.com/Seeed-Studio/EdgeLab/releases/download/model_zoo/fomo_mv2n_96.pth 
```

#### 训练模型

```bash
cd EdgeLab
conda activate edgelab
python tools/train.py mmdet configs/fomo/fomo_mobnetv2_x8_custom.py --cfg-options total_epochs=50 load_from=./work_dirs/pretrain/fomo_mv2n_96.pth 
```


## 转换模型

**第1步:** 将模型转换为TensorFlow Lite。

```bash
cd EdgeLab
conda activate edgelab
python tools/torch2tflite.py mmdet  configs/fomo/fomo_mobnetv2_x8_custom.py --weights work_dirs/fomo_mobnetv2_x8_custom/exp1/latest.pth --tflite_type int8 
```

现在我们将把生成的TFLite文件转换为UF2文件，这样我们就可以直接把UF2文件闪存到Grove - Vision AI Module和SenseCAP A1101中。

**第2步:** 将TFLite文件转换为UF2文件。

```bash
cd edgelab-example-vision-ai
python tools/uf2conv/ -f GROVEAI -t 1 -c <path to model.tflite> -o model.uf2
```
```{note}
注意：路径中的exp1是第一次训练是生成的，如果您多次训练expx会依次累加。
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
**Step 1:** 完成上述的模型还有固件烧录工作后, 打开 [链接](https://files.seeedstudio.com/grove_ai_vision/index.html)
        
**Step 2:** 点击 **Connect** 按钮. 你会看到一个弹出窗口. 选择 **Grove AI - （配对）** 然后点击**Connect**
![13](../_static/vision_ai/images/13.jpg)   
        
![12](../_static/vision_ai/images/12.png)
        
如果连接成功，你会看到以下界面
        
![14](../_static/vision_ai/images/14.png)
        
