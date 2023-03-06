
# Train and deploy a motion model on Grove - Vision AI

## Introduction

This tutorial shows how to train and deploy a motion reading detection model on Grove - Vision AI

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

please follow the [link](https://docs.edgeimpulse.com/docs/edge-impulse-studio/data-acquisition) to collect the data.


### Train the model


#### Start training.
```bash
cd EdgeLab
conda activate edgelab
python tools/train.py mmcls configs/accelerometer/3axes_accelerometer_62.5Hz_1s_classify.py
```

## Convert the model

**Step 1:** Convert models to TensorFlow Lite.

```bash
cd EdgeLab
conda activate edgelab
python tools/torch2tflite.py mmcls configs/accelerometer/3axes_accelerometer_62.5Hz_1s_classify.py --weights work_dirs/3axes_accelerometer_62.5Hz_1s_classify/exp1/latest.pth  --tflite_type int8
```

Now we will convert the generated TFLite file to a UF2 file so that we can directly flash the UF2 file into Grove - Vision AI Module and SenseCAP A1101

**Step 2:** Execute the following

```bash
cd edgelab-example-vision-ai
python tools/ufconv/uf2conv.py -f GROVEAI -t 1 -c <path to model.tflite> -o model.uf2
```

## Prepare the firmware

### Build the firmware
```bash
cd edgelab-example-vision-ai
make HW=grove_vision_ai APP=motion
make flash
```
### Convert the firmware to UF2 file

```bash
cd edgelab-example-vision-ai
python3 tools/ufconv/uf2conv.py -t 0 -c tools/image_gen_cstm/output/output.img -o firmware.uf2
```

### Flash the model and model

This explains how you can flash the previously generated firmware (firmware.uf2) and the model file (model.uf2) to Grove - Vision AI Module and SenseCAP A1101.

**Step 1.** Connect Grove - Vision AI Module/ SenseCAP A1101 to PC by using USB Type-C cable 

![45](../_static/vision_ai/images/45.png)

**Step 2.** Double click the boot button to enter **boot mode**

![47](../_static/vision_ai/images/47.png)

**Step 3:** After this you will see a new storage drive shown on your file explorer as **GROVEAI** for **Grove - Vision AI Module** and as **VISIONAI** for **SenseCAP A1101**

![46](../_static/vision_ai/images/46.png)

**Step 4:** Drag and drop the previous **firmware.uf2** at first, and then the **model.uf2** file to **GROVEAI** or **VISIONAI** 

```note
You need to enter DFU mode twice separately, once to flash the firmware and once to flash the model.Once the copying is finished **GROVEAI** drive will disapper. This is how we can check whether the copying is successful or not.
```

## Run the model 

