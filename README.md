# Reisp
基于opencv/c++，以google hdrplus为backbone进行拓展，采用openmp与none技术加速完整设计isp pipeline

# Pipeline说明

![pipeline](https://raw.githubusercontent.com/laiyiya/reisp/main/Pic/isp%20pipeline.jpg)

### GlobalAlignment
采用harris角点计算raw图的各角点，以角点附近领域为特征描述符，使用暴力匹配模式对特征描述符进行匹配，结合匹配后的角点计算单应矩阵Homography，进而图像对齐。
### BlockMatchFusion
BlockMatchFusion整理过程与Nlm非局部均值滤波类似，Sad使用L1范数计算，同时采用金字塔结构加快匹配速度。
### AWB
结合先验知识的自动白平衡，在A、U30、D50、D65等光源下标定并制作色温曲线，分块统计RAW图的R/G、B/G均值、去除异常点后结合色温曲线计算AWB Gain参数。
### RAWDenoise
从RAW域转换至YUVH域，将图像分块进行多次harr小波变换，取低频LL图像结合空域滤波再小波反变换，重复多次后将YUVH域转至RAW域
### Demosic
结合Hamilton and Adams原理，考虑梯度方向。
### ToneMapping
参考《exposure fusion》论文，结合高斯、阿普拉斯金字塔，虚拟曝光融合从而实现ToneMapping




# 致谢
参考https://github.com/jhfmat/ISP-pipeline-hdrplus  
google paper: Burst photography for high dynamic range and low-light imaging on mobile cameras
