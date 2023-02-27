
# Train and deploy a meter model on Grove - Vision AI

## Introduction

This tutorial shows how to train and deploy a meter reading detection model on Grove - Vision AI

## Prerequisites

### Hardware
- PC with Linux, WSL2 is also supported.
- Grove - Vision AI
- USB cable

### Software
- [EdgeLab](https://edgelab.readthedocs.io/zh_CN/latest/)
- [edgelab-example-vision-ai](https://github.com/Seeed-Studio/edgelab-example-vision-ai)

```{note}
Plesae make sure that you have installed the latest version of EdgeLab
```

## Train the model

### Prepare the dataset

Use the provided dataset to train the model.

```bash
cd EdgeLab
mkdir -p datasets
cd datasets
wget https://files.seeedstudio.com/wiki/Edgelab/coco_mask.zip
unzip coco_mask.zip
```

### Prepare the configuration file

```python
_base_ = '../_base_/default_runtime.py'

num_classes=1
model = dict(type='PFLD',
             backbone=dict(type='PfldMobileNetV2',
                           inchannel=3,
                           layer1=[16, 16, 16, 16, 16],
                           layer2=[32, 32, 32, 32, 32, 32],
                           out_channel=16),
             head=dict(
                 type='PFLDhead',
                 num_point=num_classes,
                 input_channel=16,
             ),
             loss_cfg=dict(type='PFLDLoss'))


# dataset settings
dataset_type = 'MeterData'

data_root = '~/datasets/meter'
height=112
width=112
batch_size=32
workers=4

train_pipeline = [
    dict(type=Resize, height=height, width=width, interpolation=0),
    dict(type='ColorJitter', brightness=0.3, p=0.5),
    # dict(type='GaussNoise'),
    dict(type='MedianBlur', blur_limit=3, p=0.3),
    dict(type='HorizontalFlip'),
    dict(type='VerticalFlip'),
    dict(type='Rotate'),
    dict(type='Affine', translate_percent=[0.05, 0.1], p=0.6)
]

val_pipeline = [dict(type=Resize, height=height, width=width)]



data = dict(
    samples_per_gpu=batch_size,
    workers_per_gpu=workers,
    train=dict(type=dataset_type,
               data_root=data_root,
               index_file=r'train/annotations.txt',
               pipeline=train_pipeline,
               test_mode=False),
    val=dict(type=dataset_type,
             data_root=data_root,
             index_file=r'val/annotations.txt',
             pipeline=val_pipeline,
             test_mode=True),
    test=dict(type=dataset_type,
              data_root=data_root,
              index_file=r'val/annotations.txt',
              pipeline=val_pipeline,
              test_mode=True
              # dataset_info={{_base_.dataset_info}}
              ))


lr=0.0001
epochs=300
evaluation = dict(save_best='loss')
optimizer = dict(type='Adam', lr=lr, betas=(0.9, 0.99), weight_decay=1e-6)
optimizer_config = dict(grad_clip=dict(max_norm=35, norm_type=2))
# learning policy
lr_config = dict(policy='step',
                 warmup='linear',
                 warmup_iters=400,
                 warmup_ratio=0.0001,
                 step=[400, 440, 490])
total_epochs = epochs
find_unused_parameters = True
```

### Train the model

```bash
cd EdgeLab
conda activate edgelab
tools/train.py mmpose configs/pfld/pfld_mv2n_112.py --gpus=1 --cfg-options total_epochs=50
```

## Convert the model

**Step 1:** Convert models to TensorFlow Lite.

```bash
cd EdgeLab
conda activate edgelab
python tools/export.py configs/pfld/pfld_mv2n_112.py --weights work_dirs/pfld_mv2n_112/exp1/latest.pth --data ~/datasets/meter/train/images
```

Now we will convert the generated TFLite file to a UF2 file so that we can directly flash the UF2 file into Grove - Vision AI Module and SenseCAP A1101

**Step 2:** Execute the following

```bash
cd edgelab-example-vision-ai
python tools/uf2conv/ -f GROVEAI -t 1 -c <path to model.tflite> -o model.uf2
```

### Flash the model and model

This explains how you can flash the previously generated firmware (firmware.uf2) and the model file (model.uf2) to Grove - Vision AI Module and SenseCAP A1101.

**Step 1.** Connect Grove - Vision AI Module/ SenseCAP A1101 to PC by using USB Type-C cable 

<div align=center><img width=1000 src="../_static/vision_ai/images/45.png"/></div>

**Step 2.** Double click the boot button to enter **boot mode**

<div align=center><img width=1000 src="../_static/vision_ai/images/46.png"/></div>

**Step 3:** After this you will see a new storage drive shown on your file explorer as **GROVEAI** for **Grove - Vision AI Module** and as **VISIONAI** for **SenseCAP A1101**

<div align=center><img width=500 src="../_static/vision_ai/images/62.jpg"/></div>

**Step 4:** Drag and drop the previous **firmware.uf2** at first, and then the **model.uf2** file to **GROVEAI** or **VISIONAI** 

```note
You need to enter DFU mode twice separately, once to flash the firmware and once to flash the model.Once the copying is finished **GROVEAI** drive will disapper. This is how we can check whether the copying is successful or not.
```

## Run the model 

**Step 1:** After loading the firmware and connecting to PC, visit [this URL](https://files.seeedstudio.com/grove_ai_vision/index.html)
        
**Step 2:** Click **Connect** button. Then you will see a pop up on the browser. Select **Grove AI - Paired** and click **Connect**
<div align=center><img width=800 src="../_static/vision_ai/images/13.jpg"></div>
        
<div align=center><img width=400 src="../_static/vision_ai/images/12.png"></div>
        
Upon successful connection, you will see a live preview from the camera. Here the camera is pointed at an analog meter.
        
<div align=center><img width=800 src="../_static/vision_ai/images/14.png"></div>
        
Now we need to set 3 points which is the center point, start point and the end point. 
        
**Step 3:** Click on **Set Center Point** and click on the center of the meter. you will see a pop up confirm the location and press **OK**
        
<div align=center><img width=800 src="../_static/vision_ai/images/15.png"></div>
        
You will see the center point is already recorded
        
<div align=center><img width=800 src="../_static/vision_ai/images/16.png"></div>
        
**Step 4:** Click on **Set Start Point** and click on the first indicator point. you will see a pop up confirm the location and press **OK**
        
<div align=center><img width=800 src="../_static/vision_ai/images/17.png"></div>
        
You will see the first indicator point is already recorded
        
<div align=center><img width=800 src="../_static/vision_ai/images/18.png"></div>
        
**Step 5:** Click on **Set End Point** and click on the last indicator point. you will see a pop up confirm the location and press **OK**

<div align=center><img width=800 src="../_static/vision_ai/images/19.png"></div>
        
You will see the last indicator point is already recorded
        
<div align=center><img width=800 src="../_static/vision_ai/images/20.png"></div>
        
**Step 6:** Set the measuring range according to the first digit and last digit of the meter. For example, he we set as **From:0 To 0.16**
        
<div align=center><img width=800 src="../_static/vision_ai/images/21.png"></div>
        
**Step 7:** Set the number of decimal places that you want the result to display. Here we set as 2
        
<div align=center><img width=800 src="../_static/vision_ai/images/22.png"></div>
        
Finally you can see the live meter reading results as follows
        
<div align=center><img width=800 src="../_static/vision_ai/images/meter.gif"/></div>

