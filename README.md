# Reisp
基于opencv/c++，以google hdrplus为backbone进行拓展，采用openmp与none技术加速完整设计isp pipeline

# Pipeline说明

![pipeline](https://github.com/laiyiya/reisp/blob/main/Pic/isp%20pipeline.jpg)

### GlobalAlignment
采用harris角点计算raw图的各角点，以角点附近领域为特征描述符，使用暴力匹配模式对特征描述符进行匹配，结合匹配后的角点计算单应矩阵Homography，进而图像对齐。
### BlockMatchFusion
BlockMatchFusion整理过程与Nlm非局部均值滤波类似，Sad使用L1范数计算，同时采用金字塔结构加快匹配速度。
### AWB
结合先验知识的自动白平衡，在A、U30、D50、D65等光源下标定并制作色温曲线，分块统计RAW图的R/G、B/G均值、去除异常点后结合色温曲线计算AWB Gain参数。
### RAWDenoise
采用灰阶图卡进行噪声标定，从RAW域转换至YUVH域，将图像分块进行多次harr小波变换，取低频LL图像结合空域滤波再小波反变换，空域滤波参数结合噪声标定结果调整，重复多次后将YUVH域转至RAW域。
### Demosic
结合Hamilton and Adams原理，考虑梯度方向。将Raw图分别按横向纵向插值计算出(B+R+2G)/4,(B+R-2G)/4,(B-R)/2,deltaG共4通道信息，分别表示了低频与高频信息。由于横纵向插值的G不同求平均后进一步得到高频的水平和垂直的梯度信息。将梯度图分块最终加权平均确定每个点的梯度方向，以此作为修正项调整插值。

### CCM 
在A、U30、LT84、CWF、D50、D65光源下结合24色卡标定CCM矩阵；评估当前的RAW图CT色温参数、结合前6种光源标定的CCM插值出当前色温下的CCM矩阵。将图像转至YUV域，引入饱和度调整矩阵S，结合当前Digital Gain对CCM矩阵进行修正，实现饱和度调整。
### ToneMapping
参考《exposure fusion》论文，结合高斯、阿普拉斯金字塔，虚拟曝光融合从而实现ToneMapping。




# 致谢
参考https://github.com/jhfmat/ISP-pipeline-hdrplus  
google paper: Burst photography for high dynamic range and low-light imaging on mobile cameras
