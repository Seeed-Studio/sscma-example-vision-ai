# Edgelab Vision AI 示例程序

本文档展示了如何将您自己的tflite模型部署到**Seeed Grove Vision AI Module**。

## 构建固件

简述如何为Grove - Vision AI模块构建固件。

**注意：** 以下内容经测试可在Ubuntu 20.04电脑上运行。

- **第一步：** 安装以下程序

```sh
sudo apt install make
sudo apt install python3-numpy
```

- **第二步：** 下载 GNU 开发工具

```sh
cd ~
wget https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases/download/arc-2020.09-release/arc_gnu_2020.09_prebuilt_elf32_le_linux_install.tar.gz
```

- **第三步：** 解压文件

```sh
tar -xvf arc_gnu_2020.09_prebuilt_elf32_le_linux_install.tar.gz
```

- **第四步：**  将**arc_gnu_2020.09_prebuilt_elf32_le_linux_install/bin** 添加到 **PATH**

```sh
export PATH="$HOME/arc_gnu_2020.09_prebuilt_elf32_le_linux_install/bin:$PATH"
```

- **第五步：** 下载本仓库并进入目录

```sh
git clone https://github.com/Seeed-Studio/edgelab-example-vision-ai
cd edgelab-example-vision-ai
```

- **第六步：** 下载相关的第三方，tflite模型和库数据（只需要下载一次）。

```sh
make download
```

- **第七步：** 编译固件

```sh
make HW=grove_vision_ai APP=meter
make flash
```

编译完成后 **tools/image_gen_cstm/output/** 目录下将会生成 **output.img**。


- **第八步：** 生成固件镜像 `firmware.uf2` 文件

```sh
python3 tools/ufconv/uf2conv.py -t 0 -c tools/image_gen_cstm/output/output.img -o firmware.uf2
```

- **第九步：** 将预先训练好的模型 `model.tflite` 转换为 `model.uf2` 文件

```sh
wget https://github.com/Seeed-Studio/Edgelab/releases/download/model_zoo/pfld_meter_int8.tflite model.tflite
python3 tools/ufconv/uf2conv.py -t 1 -c model.tflite -o model.uf2
```
## 烧录固件

简述了如何将固件闪存到Grove - Vision AI模块。

- **第一步：** 通过USB Type-C电缆将Grove - Vision AI模块连接到主机PC上

<div align=center><img width=460 src="https://files.seeedstudio.com/wiki/SenseCAP-A1101/47.png"/></div>

- **第二步：** 双击Grove - Vision AI模块的`BOOT`按钮，进入bootloader模式

<div align=center><img width=220 src="https://files.seeedstudio.com/wiki/SenseCAP-A1101/48.png"/></div>

- **第三步：** 在这之后，你会看到一个新的存储驱动器在你的文件资源管理器上显示为 **GROVEAI**。

<div align=center><img width=250 src="https://files.seeedstudio.com/wiki/SenseCAP-A1101/19.jpg"/></div>

- **第四步：** 一次将 `firmware.uf2` 和 `model.uf2` 文件拖放到GROVEAI驱动器上。

一旦复制完成，**GROVEAI** 驱动器将消失。
