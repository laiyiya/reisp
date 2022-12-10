#include "HDRPlus_WhiteBalance.h"


void CHDRPlus_WhiteBalance::Forward(cv::Mat& pInRawImage, TGlobalControl* pControl)
{
	int nWidth = pInRawImage.cols;
	int nHeigh = pInRawImage.rows;

	const float Rgain = (float)pControl->nAWBGain[0] / (float)256.0;// coeffs[0] / coeffs[1];
	const float Ggain0 = (float)pControl->nAWBGain[1] / (float)256.0;// 1.f;
	const float Ggain1 = (float)pControl->nAWBGain[2] / (float)256.0;// 1.f;
	const float Bgain = (float)pControl->nAWBGain[3] / (float)256.0;// coeffs[2] / coeffs[1];
	printf("Rgain Bgain %f %f\n", Rgain, Bgain);

	m_nMin = pControl->nBLC;
	m_nMax = pControl->nWP;
	//调试使用
	m_nMax = 65535;

	unsigned short* tableRgain = new unsigned short[m_nMax + 1];
	unsigned short* tableBgain = new unsigned short[m_nMax + 1];
	for (int k = 0;k < 65536;k++)
	{
		int tmp = k * Rgain;
		tableRgain[k] = CLIP(tmp, m_nMin, m_nMax);
		tmp = k * Bgain;
		tableBgain[k] = CLIP(tmp, m_nMin, m_nMax);
	}

//#pragma omp parallel for
	for (int y = 0;y < nHeigh;y += 2)
	{
		unsigned short* pInRawLine0;
		unsigned short* pInRawLine1;

		switch (pControl->nCFAPattern)
		{
		case RGGB:
			pInRawLine0 = (unsigned short*)(pInRawImage.data + y* pInRawImage.step[0]);
			pInRawLine1 = (unsigned short*)(pInRawImage.data + (y + 1) * pInRawImage.step[0]) + 1;
			break;

		default:
			printf("Please enter correctly RawTYPE\n");
			break;
		}

		for (int x = 0;x < nWidth;x += 2)
		{
			pInRawLine0[0] = tableRgain[pInRawLine0[0]];
			pInRawLine1[0] = tableBgain[pInRawLine1[0]];
			pInRawLine0 += 2;
			pInRawLine1 += 2;
		}
	}

	delete[]tableRgain;
	delete[]tableBgain;

}

//bool CHDRPlus_WhiteBalance::LoadMapFromFile(char* pFileName)
//{
//	m_nAWBMap = cv::imread(pFileName, 0);
//	return true;
//}
//bool CHDRPlus_WhiteBalance::EstimateCTandAWB(cv::Mat& pImage, TGlobalControl* pControl, cv::Mat& Map, int& nCT, float fAWBGain[])
//{
//	int nInBitMax = (1 << pControl->nBit) - 1;
//	m_nMin = pControl->nBLC;
//	m_nMax = nInBitMax;
//
//	int nWidth = pImage.cols;
//	int nHeigh = pImage.rows;
//	int nx12 = nWidth >> 1;
//	int ny12 = nHeigh >> 1;
//	cv::Mat RawImagechannel[4];
//	//将输入的raw图拆分成4通道
//	for (int k = 0;k < 4;k++)
//	{
//		RawImagechannel[k].create(ny12, nx12, CV_16UC1);
//	}
//	#pragma omp parallel for
//	for (int y = 0; y < nHeigh; y += 2)
//	{
//		unsigned short* pInRawLine0 = (unsigned short*)(pImage.data + y * pImage.step[0]);
//		unsigned short* pInRawLine1 = (unsigned short*)(pImage.data + (y + 1) * pImage.step[0]);
//		unsigned short* pSingleline0 = (unsigned short*)(RawImagechannel[0].data + (y / 2) * RawImagechannel[0].step[0]);
//		unsigned short* pSingleline1 = (unsigned short*)(RawImagechannel[1].data + (y / 2) * RawImagechannel[1].step[0]);
//		unsigned short* pSingleline2 = (unsigned short*)(RawImagechannel[2].data + (y / 2) * RawImagechannel[2].step[0]);
//		unsigned short* pSingleline3 = (unsigned short*)(RawImagechannel[3].data + (y / 2) * RawImagechannel[3].step[0]);
//		for (int x = 0; x < nWidth; x += 2)
//		{
//			pSingleline0[0] = pInRawLine0[0];
//			pSingleline1[0] = pInRawLine0[1];
//			pSingleline2[0] = pInRawLine1[0];
//			pSingleline3[0] = pInRawLine1[1];
//
//			pSingleline0++;
//			pSingleline1++;
//			pSingleline2++;
//			pSingleline3++;
//			pInRawLine0 += 2;
//			pInRawLine1 += 2;
//		}
//	}
//
//	/////////////////////////统计有效块的有效r g g b y均值                            //将图片按m_nBlockSize= 16 分块
//	cv::Mat SBlockUsefulPValImage[4];                                        //每个块中的有效像素值
//	cv::Mat SBlockUsefulPValNumImage[4];                                     //每个块中的有效像素值的个数
//	cv::Mat SBlockUsefulPValYImage;                                          //4个通道对应每个块合并后的Y值
//	for (int k = 0;k < 4;k++)
//	{
//		SBlockUsefulPValImage[k] = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_32FC1);
//		SBlockUsefulPValNumImage[k] = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_32FC1);
//	}
//	SBlockUsefulPValYImage = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_32FC1);
//	int nWinW16 = (nx12 + m_nBlockSize - 1) / m_nBlockSize;                         //每个块对应RawImagechannel图的大小
//	int nWinH16 = (ny12 + m_nBlockSize - 1) / m_nBlockSize;
//	int nMineffectivepoint = nWinW16 * nWinH16 * 0.9;                               //最少有效点个数
//	printf("WinH = %d   WinW = %d\n", nWinH16, nWinW16);
//	int sumnum = nWinW16 * nWinH16;
//	int x0, y0, x1, y1, m0, n0;
//	int setmin = pControl->nBLC + 16;
//	int setmax = (nInBitMax - 16);
//	//x0 y0用于控制遍历全图，x1 y1用于将图像划块，m0 n0控制分块后的图像坐标
//	for (int k = 0;k < 4;k++)
//	{
//		for (y0 = y1 = 0, m0 = 0;y0 < ny12;y0++)
//		{
//			unsigned short *pSingleLine = (unsigned short*)(RawImagechannel[k].data + y0 * RawImagechannel[k].step[0]);
//			float* SBlockUsefulPValline = (float*)(SBlockUsefulPValImage[k].data + m0 * SBlockUsefulPValImage[k].step[0]);
//			float* SBlockUsefulPValNumline = (float*)(SBlockUsefulPValNumImage[k].data + m0 * SBlockUsefulPValNumImage[k].step[0]);
//			for (x0 = x1 = 0, n0 = 0;x0 < nx12;x0++)
//			{
//				unsigned short pixel = pSingleLine[x0];
//				if (pixel<setmax && pixel>setmin)
//				{
//					SBlockUsefulPValline[n0] += (pixel - pControl->nBLC);
//					SBlockUsefulPValNumline[n0]++;
//				}
//				x1++;
//				if (x1 == nWinW16)
//				{
//					n0++;
//					x1 = 0;
//				}
//			}
//			y1++;
//			if (y1 == nWinH16)
//			{
//				m0++;
//				y1 = 0;
//			}
//		}
//	}
//
//	for (int j = 0; j < m_nBlockSize; j++)                      //计算4个通道对应每个块合并后的Y值 和 每个块中的有效像素值取平均 = 每个块中的有效像素值/每个块中的有效像素值的个数
//	{
//		float* SBlockUsefulPValline0 = (float*)(SBlockUsefulPValImage[0].data + j * SBlockUsefulPValImage[0].step[0]);
//		float* SBlockUsefulPValline1 = (float*)(SBlockUsefulPValImage[1].data + j * SBlockUsefulPValImage[1].step[0]);
//		float* SBlockUsefulPValline2 = (float*)(SBlockUsefulPValImage[2].data + j * SBlockUsefulPValImage[2].step[0]);
//		float* SBlockUsefulPValline3 = (float*)(SBlockUsefulPValImage[3].data + j * SBlockUsefulPValImage[3].step[0]);
//		float* SBlockUsefulPValNumline0 = (float*)(SBlockUsefulPValNumImage[0].data + j * SBlockUsefulPValNumImage[0].step[0]);
//		float* SBlockUsefulPValNumline1 = (float*)(SBlockUsefulPValNumImage[1].data + j * SBlockUsefulPValNumImage[1].step[0]);
//		float* SBlockUsefulPValNumline2 = (float*)(SBlockUsefulPValNumImage[2].data + j * SBlockUsefulPValNumImage[2].step[0]);
//		float* SBlockUsefulPValNumline3 = (float*)(SBlockUsefulPValNumImage[3].data + j * SBlockUsefulPValNumImage[3].step[0]);;
//		float* SBlockUsefulPValYline = (float*)(SBlockUsefulPValYImage.data + j * SBlockUsefulPValYImage.step[0]);;
//		for (int i = 0; i < m_nBlockSize; i++)
//		{
//			SBlockUsefulPValYline[i] = (SBlockUsefulPValline0[i] + SBlockUsefulPValline1[i] + SBlockUsefulPValline2[i] + SBlockUsefulPValline3[i]) /
//				(SBlockUsefulPValNumline0[i] + SBlockUsefulPValNumline1[i] + SBlockUsefulPValNumline2[i] + SBlockUsefulPValNumline3[i]);
//			SBlockUsefulPValline0[i] = SBlockUsefulPValline0[i] / SBlockUsefulPValNumline0[i];
//			SBlockUsefulPValline1[i] = SBlockUsefulPValline1[i] / SBlockUsefulPValNumline1[i];
//			SBlockUsefulPValline2[i] = SBlockUsefulPValline2[i] / SBlockUsefulPValNumline2[i];
//			SBlockUsefulPValline3[i] = SBlockUsefulPValline3[i] / SBlockUsefulPValNumline3[i];
//		}
//	}
//
//	//float scale = m_HDRPlus_Calibration_AWB.m_nMapSize2 / float(m_HDRPlus_Calibration_AWB.m_nScaleRatio * 1.0);
//	float sumBG = 0;
//	float sumRG = 0;
//	float sumGG = 1;
//	float sumweightBG = 0;
//	float sumweightRG = 0;
//	int nWPNum = 0;
//	cv::Mat WPointImage = cv::Mat::zeros(Map.cols, Map.rows, CV_8UC1);
//	cv::Mat TargetpointImage = cv::Mat::zeros(Map.cols, Map.rows, CV_8UC1);
//	cv::Mat nWP_RGImage = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_32FC1);
//	cv::Mat nWP_BGImage = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_32FC1);
//	cv::Mat nFlagWP_RGBGImage = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_8UC1);
//	m_nROIAllWPareaImage = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_8UC1);
//	m_nROIUsefulWPareaImage = cv::Mat::zeros(m_nBlockSize, m_nBlockSize, CV_8UC1);
//
//	//计算对应块中r/g和b/g的比值
//	for (int j = 0;j < m_nBlockSize;j++)
//	{
//		float* SBlockUsefulPValline0 = (float*)(SBlockUsefulPValImage[0].data + j * SBlockUsefulPValImage[0].step[0]);
//		float* SBlockUsefulPValline1 = (float*)(SBlockUsefulPValImage[1].data + j * SBlockUsefulPValImage[1].step[0]);
//		float* SBlockUsefulPValline2 = (float*)(SBlockUsefulPValImage[2].data + j * SBlockUsefulPValImage[2].step[0]);
//		float* SBlockUsefulPValline3 = (float*)(SBlockUsefulPValImage[3].data + j * SBlockUsefulPValImage[3].step[0]);
//		float* SBlockUsefulPValNumline0 = (float*)(SBlockUsefulPValNumImage[0].data + j * SBlockUsefulPValNumImage[0].step[0]);
//		float* SBlockUsefulPValNumline1 = (float*)(SBlockUsefulPValNumImage[1].data + j * SBlockUsefulPValNumImage[1].step[0]);
//		float* SBlockUsefulPValNumline2 = (float*)(SBlockUsefulPValNumImage[2].data + j * SBlockUsefulPValNumImage[2].step[0]);
//		float* SBlockUsefulPValNumline3 = (float*)(SBlockUsefulPValNumImage[3].data + j * SBlockUsefulPValNumImage[3].step[0]);
//
//		float* nWP_RGline = (float*)(nWP_RGImage.data + j * nWP_RGImage.step[0]);
//		float* nWP_BGline = (float*)(nWP_BGImage.data + j * nWP_BGImage.step[0]);
//
//		unsigned char* nFlagWP_RGBGline = (unsigned char*)(nFlagWP_RGBGImage.data + j * nFlagWP_RGBGImage.step[0]);
//		unsigned char* nROIAllWParealine = (unsigned char*)(m_nROIAllWPareaImage.data + j * m_nROIAllWPareaImage.step[0]);
//
//		for (int i = 0;i < m_nBlockSize;i++)
//		{
//			int Sumeffectivepoint = SBlockUsefulPValNumline0[0] + SBlockUsefulPValNumline1[0] + SBlockUsefulPValNumline2[0] + SBlockUsefulPValNumline3[0];
//			float yblockb_g = 1;
//			float xblockr_g = 1;
//			switch (pControl->nCFAPattern)
//			{
//				case RGGB:
//					yblockb_g = (float)(SBlockUsefulPValline3[i] * 2.0) / float(SBlockUsefulPValline1[i] + SBlockUsefulPValline2[i]);
//					xblockr_g = (float)(SBlockUsefulPValline0[i] * 2.0) / float(SBlockUsefulPValline1[i] + SBlockUsefulPValline2[i]);
//					break;
//				default:
//					printf("Please enter correctly RawTYPE\n");
//					break;
//			}
//			//将计算出的r/g和b/g缩放到map图大小，以定位出在map图中的坐标
//			int PointX = int(xblockr_g * scale);
//			int PointY = int(m_HDRPlus_Calibration_AWB.m_nMapSize2 - yblockb_g * scale);
//			int mapPixel = Map[PointX][PointY];
//
//
//
//
//
//
//
//
//
//		}
//
//
//
//	}
//
//
//
//}
//
//void CHDRPlus_WhiteBalance::Forward2(cv::Mat& pInRawImage, TGlobalControl* pControl)
//{
//	int nWidth = pInRawImage.cols;
//	int nHeigh = pInRawImage.rows;
//	float Bgain = 1;
//	float Ggain0 = 1;
//	float Ggain1 = 1;
//	float Rgain = 1;
//
//	int nCT = 256;
//	if (!m_bAutoAWBEnable)
//	{
//		Rgain = (float)pControl->nAWBGain[0] / (float)256.0;// coeffs[0] / coeffs[1];
//		Ggain0 = (float)pControl->nAWBGain[1] / (float)256.0;// 1.f;
//		Ggain1 = (float)pControl->nAWBGain[2] / (float)256.0;// 1.f;
//		Bgain = (float)pControl->nAWBGain[3] / (float)256.0;// coeffs[2] / coeffs[1];
//		nCT = int(Bgain * 256.0 / Rgain);
//	}
//	else
//	{
//		float fAWBGain[3];
//		EstimateCTandAWB(pInRawImage, pControl, m_nAWBMap, nCT, fAWBGain);
//		Bgain = fAWBGain[0];
//		Ggain0 = fAWBGain[1];
//		Rgain = fAWBGain[2];
//	}
//	printf("Rgain Bgain %f %f\n", Rgain, Bgain);
//
//	int nInBitMax = (1 << pControl->nBit) - 1;
//	m_nMin = pControl->nBLC;
//	m_nMax = nInBitMax;
//	//调试使用
//	//m_nMax = 65535;
//
//	unsigned short* tableRgain = new unsigned short[m_nMax + 1];
//	unsigned short* tableBgain = new unsigned short[m_nMax + 1];
//	for (int k = 0;k < m_nMax+1 ;k++)
//	{
//		int tmp = k * Rgain;
//		tableRgain[k] = CLIP(tmp, m_nMin, m_nMax);
//		tmp = k * Bgain;
//		tableBgain[k] = CLIP(tmp, m_nMin, m_nMax);
//	}
//
//	#pragma omp parallel for
//	for (int y = 0;y < nHeigh;y += 2)
//	{
//		unsigned short* pInRawLine0;
//		unsigned short* pInRawLine1;
//
//		switch (pControl->nCFAPattern)
//		{
//		case RGGB:
//			pInRawLine0 = (unsigned short*)(pInRawImage.data + y * pInRawImage.step[0]);
//			pInRawLine1 = (unsigned short*)(pInRawImage.data + (y + 1) * pInRawImage.step[0]) + 1;
//			break;
//
//		default:
//			printf("Please enter correctly RawTYPE\n");
//			break;
//		}
//
//		for (int x = 0;x < nWidth;x += 2)
//		{
//			pInRawLine0[0] = tableRgain[pInRawLine0[0]];
//			pInRawLine1[0] = tableBgain[pInRawLine1[0]];
//			pInRawLine0 += 2;
//			pInRawLine1 += 2;
//		}
//	}
//
//	delete[]tableRgain;
//	delete[]tableBgain;
//
//}