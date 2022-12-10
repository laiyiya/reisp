# Reisp
基于opencv/c++，以google hdrplus为backbone进行拓展，采用openmp与none技术加速，完整复现hdrplus

# Pipeline说明

![pipeline](https://raw.githubusercontent.com/laiyiya/reisp/main/Pic/isp%20pipeline.jpg)

### GlobalAlignment
采用harris角点计算raw图的各角点，以角点附近领域为特征描述符，使用暴力匹配模式对特征描述符进行匹配，结合匹配后的角点计算单应矩阵Homography，进而图像对齐。
### BlockMatchFusion
BlockMatchFusion整理过程与Nlm非局部均值滤波类似，Sad使用L1范数计算，同时采用金字塔结构加快匹配速度
### AWB

### RAWDenoise
小波降噪，采用harr小波变换，采用硬阈值收缩降噪，小波反变换
### Demosic
结合Hamilton and Adams原理，考虑梯度方向。
### ToneMapping
参考《exposure fusion》论文，结合高斯、阿普拉斯金字塔，虚拟曝光融合从而实现ToneMapping




# 致谢
参考https://github.com/jhfmat/ISP-pipeline-hdrplus
google paper: Burst photography for high dynamic range and low-light imaging on mobile cameras
