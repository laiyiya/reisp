#include "HDRPlus_Sharpen.h"


void CHDRPlus_Sharpen::Forward(cv::Mat& pRGBImage)
{
	float strength = (float)m_nSharpenStrength / (float)16;
	int nWidth = pRGBImage.cols;
	int nHeight = pRGBImage.rows;
	int sizeImage = nWidth * nHeight;
	cv::Mat YUVImage, YImage, UImage, VImage, SmallImage, LargeImage;
	cv::cvtColor(pRGBImage, YUVImage, cv::COLOR_BGR2YUV);
	std::vector<cv::Mat> channels(3);
	cv::split(YUVImage, channels);
	YImage = channels[0];
	UImage = channels[1];
	VImage = channels[2];

	cv::GaussianBlur(YImage, SmallImage, cv::Size(7, 7),0);
	cv::GaussianBlur(SmallImage, LargeImage, cv::Size(7, 7),0);
	cv::Mat tmp;
	cv::subtract(SmallImage, LargeImage, tmp);
	//cv::subtract(SmallImage, LargeImage, tmp,cv::Mat(), CV_32FC1);
	tmp = strength * tmp;
	cv::add(YImage, tmp, YImage,cv::Mat(),CV_16UC1);

	cv::merge(channels, YUVImage);
	cv::cvtColor(YUVImage, pRGBImage, cv::COLOR_YUV2BGR);


	//用下面方式计算的话结果会导致图片青色，推断可能opencv的BGR2YUV，与下文的YUV2BGR不匹配，转换矩阵不同
//#pragma omp parallel for 
//	for (int y = 0; y < nHeight; y++)
//	{
//		unsigned short* pRGBline = (unsigned short*)(pRGBImage.data + y * pRGBImage.step[0]);
//		unsigned short* pYline = (unsigned short*)(YImage.data + y * YImage.step[0]);
//		unsigned short* pUline = (unsigned short*)(UImage.data + y * UImage.step[0]);
//		unsigned short* pVline = (unsigned short*)(VImage.data + y * VImage.step[0]);
//		unsigned short* pSmallline = (unsigned short*)(SmallImage.data + y * SmallImage.step[0]);
//		unsigned short* pLargeline = (unsigned short*)(LargeImage.data + y * LargeImage.step[0]);
//		for (int x = 0; x < nWidth; x++)
//		{
//			long long int YUV[3], RGB[3];
//			YUV[0] = pYline[0] + strength * (pSmallline[0] - pLargeline[0]);
//			//int R = pYline[0] + 1.403f * pVline[0];
//			//int G = pYline[0] - .344f * pUline[0] - 0.714f * pVline[0];
//			//int B = pYline[0] + 1.770f * pUline[0];
//			//pRGBline[2] = CLIP(R, 0, 65535);
//			//pRGBline[1] = CLIP(G, 0, 65535);
//			//pRGBline[0] = CLIP(B, 0, 65535);
//			YUV[1] = pUline[0];
//			YUV[2] = pVline[0];
//			YUV[1] -= 32768;
//			YUV[2] -= 32768;
//			RGB[0] = YUV[0] * 2048 + YUV[1] * (4096 - 467);   //扩大2048倍，2的11次方     R
//			RGB[2] = YUV[0] * 2048 + YUV[2] * (4096 - 1225);                            //G
//			RGB[1] = YUV[0] * 2048 - YUV[1] * 705 - YUV[2] * 1463 - 10000;              //B
//			for (int i = 0; i < 3; i++)
//			{
//				if (RGB[i] < 0)RGB[i] = 0;
//				RGB[i] >>= 11;   //还原，右移11位，2的11次方
//				if (RGB[i] > 65535)RGB[i] = 65535;
//				pRGBline[i] = (unsigned short)RGB[i];
//			}
//
//			pYline++;
//			pUline++;
//			pVline++;
//			pSmallline++;
//			pLargeline++;
//			pRGBline += 3;
//		}
//	}


}