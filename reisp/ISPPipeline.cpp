#include <iostream>

#include "../Layer/HDRPlus_Forward.h"
using namespace std;
using namespace cv;



bool Load16BitRawDataFromBinFile(char* pFileName, unsigned short* pRawData, int nWidth, int nHeight, int nBits, bool bHighBit, bool bByteOrder, int nMIPIRAW)//bByteOrder小端正常都是小端模式（小端字节序就是最低有效字节落在低地址上的字节存放方式）bHighBit 高低位有效 nMIPIRAW==0表示不是mipi 其他表示不同mipi
{
	if (nMIPIRAW == 0)//0表示不是mipi其他代表不同mipi


	{
		int x, y, g;
		int mask = (1 << nBits) - 1;
		int shift = (16 - nBits);
		//if (!CreateImage(nWidth, nHeight, nBits))return false;
		FILE* fp = fopen(pFileName, "rb");
		if (fp == NULL)return false;
		for (y = 0; y < nHeight; y++)
		{
			unsigned short* wpLine = pRawData + y* nWidth;
			//从fp中读取数据到wpLine中，每个数据占sizeof(unsigned short)个字节，读nWidth个数据
			//int s = fread(wpLine, sizeof(unsigned short), nWidth, fp);
			if (fread(wpLine, sizeof(unsigned short), nWidth, fp) != nWidth)
			{
				printf("Read RAW16 File from %s Error!!!\n", pFileName);
				fclose(fp);
				return false;
			}
			for (x = 0; x < nWidth; x++)
			{
				g = wpLine[x];
				if (!bByteOrder)
				{
					g = ((g >> 8) & 255) | ((g & 255) << 8);
				}
				if (bHighBit)g = g >> shift;
				if (g > mask)
				{
					printf("Pos [%d,%d] Pixel %d>%d\n", x, y, g, mask);
				}
				g = g & mask;
				wpLine[x] = (unsigned short)g;
			}
		}
		fclose(fp);
		return true;
	}
//	else if (nMIPIRAW == 1)
//	{
//		//RAW 10
//		if (!CreateImage(nWidth, nHeight, 10))return false;
//		SingleUcharImage tmpbuf;
//		tmpbuf.CreateImage(nWidth, nHeight);
//		FILE* fp = fopen(pFileName, "rb");
//		if (fp == NULL)return false;
//		fread(tmpbuf.GetImageData(), 1, nWidth * nHeight / 4 * 5, fp);
//		fclose(fp);
//		int nProcs = omp_get_num_procs();
//#pragma omp parallel for num_threads(nProcs) schedule(dynamic,16) 
//		for (int y = 0; y < nHeight; y++)
//		{
//			unsigned short* pInputImage = GetImageLine(y);
//			unsigned char* pInput = tmpbuf.GetImageData() + y * nWidth * 5 / 4;
//			unsigned char Buffer[5];
//			for (int i = 0; i < nWidth; i += 4)
//			{
//				Buffer[0] = *pInput++;
//				Buffer[1] = *pInput++;
//				Buffer[2] = *pInput++;
//				Buffer[3] = *pInput++;
//				Buffer[4] = *pInput++;
//				int nShift = 6;
//				int g[4];
//				for (int j = 0; j < 4; j++, nShift -= 2)
//				{
//					g[j] = Buffer[j];
//					g[j] <<= 2;
//					g[j] |= (Buffer[4] >> nShift) & 3;
//					*(pInputImage++) = (unsigned short)g[j];
//				}
//			}
//		}
//		return true;
//	}
//	else if (nMIPIRAW == 2)
//	{
//		//QCOM MIPI10
//		int x, y, g;
//		int nStride = (((nWidth * 5) >> 2) + 8) & (~7);
//		FILE* fp = fopen(pFileName, "rb");
//		if (fp == NULL)return false;
//		printf("Width=%d Stride=%d\n", nWidth, nStride);
//		if (!CreateImage(nWidth, nHeight, 10))
//		{
//			fclose(fp);
//			return false;
//		}
//		unsigned char* pBuffer = new unsigned char[nStride];
//		if (pBuffer == NULL)
//		{
//			fclose(fp);
//			return false;
//		}
//		unsigned short* pInputImage = GetImageData();
//		for (y = 0; y < nHeight; y++)
//		{
//			if (fread(pBuffer, 1, nStride, fp) < nStride)
//			{
//				fclose(fp);
//				delete[] pBuffer;
//				return false;
//			}
//			unsigned char* pSrc = pBuffer;
//			for (x = 0; x < nWidth / 4; x++)
//			{
//				g = pSrc[0];
//				g = (g << 2) | (pSrc[4] >> 6);
//				*(pInputImage++) = (unsigned short)g;
//				g = pSrc[1];
//				g = (g << 2) | ((pSrc[4] >> 4) & 3);
//				*(pInputImage++) = (unsigned short)g;
//				g = pSrc[2];
//				g = (g << 2) | ((pSrc[4] >> 2) & 3);
//				*(pInputImage++) = (unsigned short)g;
//				g = pSrc[3];
//				g = (g << 2) | (pSrc[4] & 3);
//				*(pInputImage++) = (unsigned short)g;
//				pSrc += 5;
//			}
//		}
//		fclose(fp);
//		delete[] pBuffer;
//		return true;
//	}
//	else if (nMIPIRAW == 3)
//	{
//		//MTK RAW10
//		int i, buffer[5], g;
//		FILE* fp = fopen(pFileName, "rb");
//		if (fp == NULL)return false;
//		if (!CreateImage(nWidth, nHeight, 10))
//		{
//			fclose(fp);
//			return false;
//		}
//		unsigned short* pInputImage = GetImageData();
//		for (i = 0; i < (nWidth * nHeight) >> 2; i++)
//		{
//			if (fread(buffer, 1, 5, fp) != 5)
//			{
//				printf("Read RAW16 File from %s Error!!!\n", pFileName);
//				fclose(fp);
//				return false;
//			}
//			g = buffer[1] & 3;
//			g = (g << 8) | buffer[0];
//			*(pInputImage++) = (unsigned short)g;
//			g = buffer[2] & 15;
//			g = (g << 6) | ((buffer[1] >> 2) & 63);
//			*(pInputImage++) = (unsigned short)g;
//			g = buffer[3] & 63;
//			g = (g << 4) | ((buffer[2] >> 4) & 15);
//			*(pInputImage++) = (unsigned short)g;
//			g = buffer[4];
//			g = (g << 2) | ((buffer[3] >> 6) & 3);
//			*(pInputImage++) = (unsigned short)g;
//		}
//		fclose(fp);
//		return true;
//	}
	else
	{
		printf("Unknonw MIPI Format=%d!!!\n", nMIPIRAW);
		return false;
	}
}



bool SaveToBitmapFile(char* pFileName, cv::Mat & img, int nChannel)
{
	Mat tmp;
	//convertTo,可以改变mat深度，及CV_8UC1，但不可以改变通道数C，支持转换后赋值给自己
	//dst(i)=src(i)xscale+(shift,shift,…)
	img.convertTo(tmp, CV_8UC1, 255.0 / 65535, 1);
	if (nChannel) 
	{
		cvtColor(tmp, tmp, COLOR_GRAY2RGB);
	}
	imwrite(pFileName, tmp);
	return true;
}





bool m_bHighBits = false;           //高低位有效
bool m_bByteOrder = true;           //是否小端模式
int m_nWidth = 4608;                //图像宽
int m_nHeight = 3456;               //图像高
int m_blc = 64;                     //黑电平校正时的黑电平
int m_Saturate = 65535;             //像素值上限
int m_Rotation = 0;
int m_FaceNum = 0;
int m_nMIPIRAW = 0;                 //0：MIPI协议  1：QCOM，MTK等等
int m_nFrameNum = 8;                //一共8帧原始图像
int m_nBits = 16;                   //图像bit位
int OutBit = 8;                     //输出图像bit位
int m_nISO = 2191;                  //ISO
int m_nCFAPattern = 2;              //彩色滤光片阵列模式，0：BGGR 1：GBRG 2表示GRBG 3：RGGB
int m_nMinISO = 100;                //最小ISO
int m_CameraExposure = 10000000;    //曝光时间
float m_fRGain = 1.781603;          //白平衡增益
float m_fGGain = 1.000000;
float m_fBGain = 1.640738;          
float m_f[3][3] = { 1.781250, -0.906250, 0.125000,      //CCM色彩转换矩阵
	0.007812,1.015625,-0.023438,
	0.218750,-1.007812,1.789062 };

//加载txt配置参数
bool LoadMetaDataTxtFile(char* pFileName)
{
	FILE* fp = fopen(pFileName, "rt");
	if (fp == NULL)return false;
	//加载txt文档的白平衡参数，并打印输出
	if (fscanf(fp, "wb rgb gain(%f,%f,%f)\n", &m_fRGain, &m_fGGain, &m_fBGain) != 3)return false;
	printf("AWBGain=[%f,%f,%f]\n", m_fRGain, m_fGGain, m_fBGain);
	//加载txt文档的CCM矩阵参数，并打印输出
	if (fscanf(fp, "[%f,%f,%f,\n", &m_f[0][0], &m_f[0][1], &m_f[0][2]) != 3)return false;
	if (fscanf(fp, "[%f,%f,%f,\n", &m_f[1][0], &m_f[1][1], &m_f[1][2]) != 3)return false;
	if (fscanf(fp, "[%f,%f,%f,\n", &m_f[2][0], &m_f[2][1], &m_f[2][2]) != 3)return false;
	printf("[%f,%f,%f,\n", m_f[0][0], m_f[0][1], m_f[0][2]);
	printf("[%f,%f,%f,\n", m_f[1][0], m_f[1][1], m_f[1][2]);
	printf("[%f,%f,%f,\n", m_f[2][0], m_f[2][1], m_f[2][2]);
	//加载txt文档的iso参数，并打印输出
	if (fscanf(fp, "ae  sensorSensitivity(iso): %d\n", &m_nISO) != 1)return false;
	printf("ISO=%d\n", m_nISO);
	//加载txt文档的曝光时间参数，并打印输出
	if (fscanf(fp, "exposure(ns): %d\n", &m_CameraExposure) != 1)return false;
	printf("Exposure=%d\n", m_CameraExposure);
	//加载txt文档的黑电平参数，并打印输出
	if (fscanf(fp, "BLC: %d\n", &m_blc) != 1)return false;
	printf("BLC=%d\n", m_blc);
	//加载txt文档的彩色滤光片阵列参数，并打印输出
	if (fscanf(fp, "CFAPattern: %d\n", &m_nCFAPattern) != 1)return false;
	printf("m_nCFAPattern=%d\n", m_nCFAPattern);
	//加载txt文档的像素值上限参数，并打印输出
	if (fscanf(fp, "Saturate: %d\n", &m_Saturate) != 1)return false;
	printf("m_Saturate=%d\n", m_Saturate);
	//加载txt文档的raw图宽高参数，并打印输出
	if (fscanf(fp, "raw_width,raw_height(%d %d)\n", &m_nWidth, &m_nHeight) != 2)return false;
	printf("raw_width,raw_height=%d %d\n", m_nWidth, m_nHeight);

	return true;
}


int main(int argc, char* argv[])
{
	//加载txt参数
	LoadMetaDataTxtFile("C:/Users/mi/Desktop/reisp/data/burst23_0.txt");
	TGlobalControl* pControl = new TGlobalControl;
	pControl->nBLC = m_blc;
	pControl->nWP = m_Saturate;
	pControl->nCameraGain = m_nISO * 16 / m_nMinISO;
	pControl->nFrameNum = m_nFrameNum;
	pControl->nCFAPattern = m_nCFAPattern;
	pControl->nAWBGain[0] = m_fRGain * 256;
	pControl->nAWBGain[1] = m_fGGain * 256;
	pControl->nAWBGain[2] = m_fGGain * 256;
	pControl->nAWBGain[3] = m_fBGain * 256;
	for (int n = 0; n < 3; n++)
	{
		for (int m = 0; m < 3; m++)
		{
			pControl->nCCM[n][m] = m_f[n][m];
		}
	}
	//加载raw图
	char* rawFileName = "C:/Users/mi/Desktop/reisp/data/BlockMatchFusion.raw";
	//char* rawFileName = "C:/Users/mi/Desktop/reisp/data/burst23_0.raw";
	unsigned short* pRawData = (unsigned short*)malloc(m_nWidth * m_nHeight * sizeof(unsigned short));
	if (!Load16BitRawDataFromBinFile(rawFileName, pRawData, m_nWidth, m_nHeight, m_nBits, m_bHighBits, m_bByteOrder, m_nMIPIRAW))return false;


	Mat img(Size(m_nWidth, m_nHeight), CV_16UC1, pRawData);
	Mat OutRGBImage8(Size(m_nWidth, m_nHeight), CV_16UC3);

	SaveToBitmapFile("Outbmp/myrawdata.bmp", img, 3);
	

	//实例化m_HDRPlus_Forward对象,相关模块参数初始化
	CHDRPlus_Forward m_HDRPlus_Forward;
	//加载isp配置参数
	//m_HDRPlus_Forward.LoadMultiConfigFile("C:/Users/mi/Desktop/reisp/data/weight.param");
	//调用Forward方法进行isppipeline
	m_HDRPlus_Forward.Forward(img, OutRGBImage8, pControl);
	

	//Mat img2(Size(width, height), CV_8UC3, Scalar(0));

	//

	//cvtColor(img, img2, COLOR_BayerBG2BGR);
	//cvtColor(img2, img2, COLOR_BGR2RGB);

	//img2.convertTo(img2, CV_8UC3, 255);

	
	//namedWindow("img", WINDOW_NORMAL);
	//imshow("img", img);
	//namedWindow("img2", WINDOW_NORMAL);
	//imshow("img2", img2);
	//waitKey(0);
	//destroyWindow("img");
	//destroyWindow("img2");
	return 0;
}




