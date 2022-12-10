#include "HDRPlus_Demosaicing.h"


bool CHDRPlus_Demosaicing::Forward(cv::Mat& source, cv::Mat& dst)
{

	int Width = source.cols;
	int Height = source.rows;

	const int padx = 2;
	const int pady = 2;
	//将输入图像边缘拓宽2像素
	cv::Mat pInRAWImageExtend2(cv::Size(Width+4, Height+4), CV_16UC1);
	cv::copyMakeBorder(source, pInRAWImageExtend2, padx, padx, pady, pady, cv::BORDER_REFLECT);
	int nWidth = pInRAWImageExtend2.cols;
	int nHeight = pInRAWImageExtend2.rows;

#pragma omp parallel for 
	for (int y = 0; y < Height; y += 2)
	{
		int realplaceY = y + pady;
		//.data表示图像Mat的地址，.step表示多少个字节，对于三维图像step[0]对于通道，即一个通道图像所占的字节数
		//对于二维图像，step[0]表示一行的宽，所占的字节数，step[1]表示一列的高，所占的字节数
		unsigned short* p = (unsigned short*)dst.data;
		unsigned short* pRGBline0 = (unsigned short*)(dst.data + y * dst.step[0]);
		unsigned short* pRGBline1 = (unsigned short*)(dst.data + (y + 1) * dst.step[0]);

		int bitWidthInRAWImageExtend2 = pInRAWImageExtend2.step[0];
		//相当与定义了一个6行的窗口，每次滑动步长为2
		unsigned short* pRAWlineSub2 = (unsigned short*)(pInRAWImageExtend2.data + (realplaceY - 2) * bitWidthInRAWImageExtend2);
		unsigned short* pRAWlineSub1 = (unsigned short*)(pInRAWImageExtend2.data + (realplaceY - 1) * bitWidthInRAWImageExtend2);
		unsigned short* pRAWlineCurrent = (unsigned short*)(pInRAWImageExtend2.data + realplaceY * bitWidthInRAWImageExtend2);
		unsigned short* pRAWlineAdd1 = (unsigned short*)(pInRAWImageExtend2.data + (realplaceY + 1) * bitWidthInRAWImageExtend2);
		unsigned short* pRAWlineAdd2 = (unsigned short*)(pInRAWImageExtend2.data + (realplaceY + 2) * bitWidthInRAWImageExtend2);
		unsigned short* pRAWlineAdd3 = (unsigned short*)(pInRAWImageExtend2.data + (realplaceY + 3) * bitWidthInRAWImageExtend2);
		pRAWlineSub2 += padx;
		pRAWlineSub1 += padx;
		pRAWlineCurrent += padx;
		pRAWlineAdd1 += padx;
		pRAWlineAdd2 += padx;
		pRAWlineAdd3 += padx;
		int RGB0[6] = { 0,0,0,0,0,0 };
		int RGB1[6] = { 0,0,0,0,0,0 };
		for (int x = 0; x < Width; x += 2)
		{
			int x1 = x + 1;
			switch (3)
			{
			//对RGGB4个点进行差值，每个点都得到对于的RGB值
			case RGGB:
				//R--at--R
				//G--at--R
				//B--at--R
				RGB0[0] = pRAWlineCurrent[x];
				RGB0[1] = (pRAWlineCurrent[x] * 4 + (pRAWlineCurrent[x - 1] + pRAWlineCurrent[x + 1] + pRAWlineSub1[x] + pRAWlineAdd1[x]) * 2 - pRAWlineCurrent[x - 2] - pRAWlineCurrent[x + 2] - pRAWlineSub2[x] - pRAWlineAdd2[x]) >> 3;
				RGB0[2] = (pRAWlineCurrent[x] * 12 + (pRAWlineSub1[x - 1] + pRAWlineSub1[x + 1] + pRAWlineAdd1[x + 1] + pRAWlineAdd1[x - 1]) * 4 - (pRAWlineAdd2[x] + pRAWlineSub2[x] + pRAWlineCurrent[x + 2] + pRAWlineCurrent[x - 2]) * 3) >> 4;
				//R--at--G
				//G--at--G
				//B--at--G
				//rgb
				RGB0[3] = (pRAWlineCurrent[x1] * 10 + (pRAWlineCurrent[x1 - 1] + pRAWlineCurrent[x1 + 1]) * 8 + pRAWlineSub2[x1] + pRAWlineAdd2[x1] - (pRAWlineCurrent[x1 - 2] + pRAWlineCurrent[x1 + 2] + pRAWlineSub1[x1 + 1] + pRAWlineAdd1[x1 + 1] + pRAWlineSub1[x1 - 1] + pRAWlineAdd1[x1 - 1]) * 2) >> 4;
				RGB0[4] = pRAWlineCurrent[x1];
				RGB0[5] = (pRAWlineCurrent[x1] * 10 + (pRAWlineSub1[x1] + pRAWlineAdd1[x1]) * 8 + pRAWlineCurrent[x1 + 2] + pRAWlineCurrent[x1 - 2] - (pRAWlineAdd2[x1] + pRAWlineSub2[x1] + pRAWlineSub1[x1 + 1] + pRAWlineAdd1[x1 + 1] + pRAWlineSub1[x1 - 1] + pRAWlineAdd1[x1 - 1]) * 2) >> 4;
				//R--at--G
				//G--at--G
				//B--at--G
				//rgb
				RGB1[0] = (pRAWlineAdd1[x] * 10 + (pRAWlineCurrent[x] + pRAWlineAdd2[x]) * 8 + pRAWlineAdd1[x + 2] + pRAWlineAdd1[x - 2] - (pRAWlineAdd3[x] + pRAWlineSub1[x] + pRAWlineCurrent[x + 1] + pRAWlineAdd2[x + 1] + pRAWlineCurrent[x - 1] + pRAWlineAdd2[x - 1]) * 2) >> 4;
				RGB1[1] = pRAWlineAdd1[x];
				RGB1[2] = (pRAWlineAdd1[x] * 10 + (pRAWlineAdd1[x - 1] + pRAWlineAdd1[x + 1]) * 8 + pRAWlineSub1[x] + pRAWlineAdd3[x] - (pRAWlineAdd1[x - 2] + pRAWlineAdd1[x + 2] + pRAWlineCurrent[x - 1] + pRAWlineCurrent[x + 1] + pRAWlineAdd2[x - 1] + pRAWlineAdd2[x + 1]) * 2) >> 4;
				//R--at--B
				//G--at--B
				//B--at--B
				//rgb
				RGB1[3] = (pRAWlineAdd1[x1] * 12 + (pRAWlineCurrent[x1 - 1] + pRAWlineCurrent[x1 + 1] + pRAWlineAdd2[x1 + 1] + pRAWlineAdd2[x1 - 1]) * 4 - (pRAWlineAdd3[x1] + pRAWlineSub1[x1] + pRAWlineAdd1[x1 + 2] + pRAWlineAdd1[x1 - 2]) * 3) >> 4;
				RGB1[4] = (pRAWlineAdd1[x1] * 4 + (pRAWlineAdd1[x1 - 1] + pRAWlineAdd1[x1 + 1] + pRAWlineCurrent[x1] + pRAWlineAdd2[x1]) * 2 - pRAWlineAdd1[x1 - 2] - pRAWlineAdd1[x1 + 2] - pRAWlineSub1[x1] - pRAWlineAdd3[x1]) >> 3;
				RGB1[5] = pRAWlineAdd1[x1];
				break;
			default:
				printf("Please enter correctly RawTYPE\n");
				break;
			}
			pRGBline0[0] = CLIP(RGB0[0], m_nMin, m_nMax);
			pRGBline0[1] = CLIP(RGB0[1], m_nMin, m_nMax);
			pRGBline0[2] = CLIP(RGB0[2], m_nMin, m_nMax);
			pRGBline0[3] = CLIP(RGB0[3], m_nMin, m_nMax);
			pRGBline0[4] = CLIP(RGB0[4], m_nMin, m_nMax);
			pRGBline0[5] = CLIP(RGB0[5], m_nMin, m_nMax);
			pRGBline1[0] = CLIP(RGB1[0], m_nMin, m_nMax);
			pRGBline1[1] = CLIP(RGB1[1], m_nMin, m_nMax);
			pRGBline1[2] = CLIP(RGB1[2], m_nMin, m_nMax);
			pRGBline1[3] = CLIP(RGB1[3], m_nMin, m_nMax);
			pRGBline1[4] = CLIP(RGB1[4], m_nMin, m_nMax);
			pRGBline1[5] = CLIP(RGB1[5], m_nMin, m_nMax);
			pRGBline0 += 6;
			pRGBline1 += 6;
		}
	}
	return true;
}
