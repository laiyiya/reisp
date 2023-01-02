# Reisp
基于OpenCV/C++，以Google HDRPlus为backbone进行拓展，采用OpenMP与Neon技术加速，完整设计ISP pipeline

## OpenMP 
常用指令
* #pragma omp parallel for num_threads(thread_count)多线程并行执行下面的for循环。
* #pragma omp parallel for num_threads(thread_count) private(tmp)多线程并行执行下面的for循环，每一个线程都是拥有tmp私有变量，所有的线程都不会使用到先前的定义。
* #pragma omp parallel for num_threads(thread_count) firstprivate(tmp) 多线程并行执行下面的for循环，每一个线程都是拥有tmp私有变量，只会在每个线程的第一个循环继承，而不会在每个循环中继承。
* #pragma omp parallel for num_threads(thread_count) lastprivate(tmp) 多线程并行执行下面的for循环，lastprivate选项告诉编辑器私有变量会在最后一个循环出去的时候，用私有变量的值替换掉我们共享变量的值；
* #pragma omp parallel for reduction(+:sum) 下面的for循环要分成多个线程运行，但每个线程都要保存一个自己的变量sum在循环结束后，所有线程把自己的sum累加起来作为最后的sum输出。
* #pragma omp parallel for num_threads(thread_count) schedule(dynamic,2)下面的for循环要分成多个线程运行，动态调度：在循环执行过程中，边执行边分配每个线程，平衡每个线程的运行时间。

![pipeline](https://github.com/laiyiya/reisp/blob/main/Pic/isp%20pipeline.jpg)

### DigitalGain 
比较MeanY与目标亮度TargetY估算DigiGain大小。将Raw分4x4=16块并乘上对应不同权重加权平均得到MeanYr，MeanYgr，MeanYgb，MeanYb，引入保留高光部分gain参数highlevelGainThr，考虑CameraGain、MaxToalGain、MaxDigiGain、highlevelGainThr，选择最佳的DigitalGain。
### Getsharpness 
将Raw图四像素RGGB合并成Y图并经过Gamma变换提示亮度。根据不同Gain插值计算出边缘阈值参数EdgeThre，将Y图高斯平滑后使用水平垂直差分算子减去EdgeThre后得到梯度图，梯度图求和并排序，sharpness值最大的raw图即为后续GlobalAlignment的参考帧。
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
在A、U30、LT84、CWF、D50、D65光源下结合24色卡，以最小二乘法标定CCM矩阵；评估当前的RAW图CT色温参数、结合前6种光源标定的CCM插值出当前色温下的CCM矩阵。将图像转至YUV域，引入饱和度调整矩阵S，结合当前Digital Gain对CCM矩阵进行修正，实现饱和度调整。
### ToneMapping
参考《exposure fusion》论文，结合高斯、阿普拉斯金字塔，虚拟曝光融合从而实现ToneMapping。

### ChromaDenoise 
彩噪大部分为低频噪声，因此将RGB转至YUV；根据Gain值的不同，选择不同的双边滤波参数对Y通道去除部分高频噪声；对UV通道进行高斯滤波，滤波后的值与原值按一定比率混合；滤波后会造成饱和度下降，引入饱和度系数权重调整饱和度；YUV转至RGB


# 致谢
参考https://github.com/jhfmat/ISP-pipeline-hdrplus  
google paper: Burst photography for high dynamic range and low-light imaging on mobile cameras
