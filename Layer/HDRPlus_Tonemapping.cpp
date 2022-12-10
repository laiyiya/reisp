#include "HDRPlus_Tonemapping.h"
//
//
//�����ع⣬ͼ���ں��㷨��ʵ��local tonemapping�����׼ͼ���ں��㷨������Ȩ�ؼ��㲿�֣������������������Ϣ��
// 
// 

//�������������Ƿ����
bool CHDRPlus_Tonemapping::EstimateDigiGain(cv::Mat& pInImage, TGlobalControl* pControl)
{
	int m, n, W, x, y, x1, y1, Y, C[1];
	int nWidth = pInImage.cols;
	int nHeight = pInImage.rows;

	double fMean[1], fMeanV;
	long long int intMean[1], intMeanV;
	int nWinW = nWidth >> 2;
	int nWinH = nHeight >> 2;
	unsigned int* pHist = new unsigned int[m_nMax + 1];
	if (pHist == NULL)return false;
	memset(pHist, 0, sizeof(int) * (m_nMax + 1));
	fMean[0] = 0;
	intMean[0] = 0;
	C[0] = 0;
	//��ͼ��ֳ�4*4��16��飬ÿ�����m_nBlockWeightMap��ϵ��
	for (y = y1 = 0, m = 0; y < nHeight; y++)
	{
		unsigned short* pLine = (unsigned short*)(pInImage.data + y * pInImage.step[0]);
		W = m_nBlockWeightMap[m][0];
		for (x = x1 = 0, n = 0; x < nWidth; x++)
		{
			unsigned short Y = pLine[0];
			intMean[0] += Y * W;
			C[0] += W;
			pHist[Y] += W;
			x1++;
			if (x1 == nWinW)
			{
				n++;
				W = m_nBlockWeightMap[m][n];
				x1 = 0;
			}
			pLine++;
		}
		y1++;
		if (y1 == nWinH)
		{
			m++;
			y1 = 0;
		}
	}
	//��Ȩ���ƽ���Ҷ�
	fMean[0] = (double)intMean[0];
	fMean[0] /= C[0];
	printf("GRAY Mean:[%f]\n", fMean[0]);


	if (fMean[0] < 0)fMean[0] = 0;
	fMeanV = fMean[0];
	if (fMeanV < 1.0)fMeanV = 1.0;
	unsigned int nPs = 0;
	//����߹ⲿ�ֵ�����
	unsigned int nThre1 = (C[0] * m_nHighLevelPtsPercent[0]) >> 16;//Ratio of histogram between 0 and X
	unsigned int nThre2 = (C[0] * m_nHighLevelPtsPercent[1]) >> 16;//Ratio of histogram between 0 and X
	if (nThre2 < nThre1)nThre2 = nThre1;
	for (y = m_nMax - 1; y > 0; y--)
	{
		nPs += pHist[y];
		if (nPs >= nThre1)
		{
			y++;
			break;
		}
	}
	if (y <= 0)y = 0 + 1;
	int m_nHighLevelGain[2];
	m_nHighLevelGain[0] = (1 << 23) / (y);
	printf("HighLevel0=%d HighGain0=%d\n", y, m_nHighLevelGain[0]);
	if (nThre2 > nThre1)
	{
		for (; y > 0; y--)
		{
			nPs += pHist[y];
			if (nPs >= nThre2)
			{
				y++;
				break;
			}
		}
		if (y <= 0)y = 1;
		m_nHighLevelGain[1] = (1 << 23) / (y);
	}
	else
	{
		m_nHighLevelGain[1] = m_nHighLevelGain[0];
	}
	printf("HighLevel1=%d HighGain1=%d\n", y, m_nHighLevelGain[1]);
	delete[] pHist;
	int m_nMeanY;
	//��������
	if (m_bAutoDigiGainEnable == 1)
	{
		/*if (fMeanV < m_nTargetY)
		{*/
		pControl->nDigiGain = (int)(m_nTargetY * 128.0 / fMeanV);                         //����=Ŀ��Ҷ�/��ǰ��Ȩƽ���Ҷ�
		if (pControl->nDigiGain > m_nMaxDigiGain)pControl->nDigiGain = m_nMaxDigiGain;  //���������������õ����棬����=�������
		if (m_bKeepHighLightEnable == 1)                                                //�߹ⲿ��
		{
			if (pControl->nDigiGain > m_nHighLevelGain[0])pControl->nDigiGain = m_nHighLevelGain[0];
		}
		/*}
		else
		{
			pControl->nDigiGain = 128;
		}
		m_nMeanY = (int)(fMeanV*pControl->nDigiGain) / 128.0;*/
	}
	else
	{
		m_nMeanY = (int)(fMeanV * pControl->nDigiGain) / 128.0;
	}
	//��֤�ܵ�gain���ܴ���m_nMaxTotalGain
	int nTotalGain = (pControl->nCameraGain * pControl->nDigiGain + 2047) / 2048;
	if (nTotalGain > m_nMaxTotalGain)
	{
		pControl->nDigiGain = (m_nMaxTotalGain * 2048) / pControl->nCameraGain;
		//if (pControl->nDigiGain < 128)pControl->nDigiGain = 128;
	}
	m_nMeanY = (int)(fMeanV * pControl->nDigiGain) / 128.0;
	//���ͼ��ļ�Ȩƽ���Ҷȣ�Ŀ��Ҷȣ��������棬������ƽ���Ҷ�
	printf("MeanV=%f Target=%d DigiGain=%d AftGain MeanY=%d\n", fMeanV, m_nTargetY, pControl->nDigiGain, m_nMeanY);
	printf("TotalGain=%.2f\n", (float)(pControl->nCameraGain * pControl->nDigiGain) / 2048.0);
	return true;
}

//��ͼ������gain����
bool  CHDRPlus_Tonemapping::Brighten(cv::Mat& pInDarkImage, float gain, cv::Mat& pOutBrightImage)
{
	int nWidth = pInDarkImage.cols;
	int nHeight = pInDarkImage.rows;
	unsigned short* table = new unsigned short[m_nMax + 1];
	for (int k = 0; k < m_nMax + 1; k++)
	{
		int tmp = k * gain;
		table[k] = CLIP(tmp, m_nMin, m_nMax);
	}
	int tmpWidth = nWidth / 4 * 4;
#pragma omp parallel for 
	for (int y = 0; y < nHeight; y++)
	{
		unsigned short* pInDarkLine = (unsigned short*)(pInDarkImage.data + y*pInDarkImage.step[0]);
		unsigned short* pOutBrightLine = (unsigned short*)(pOutBrightImage.data + y*pOutBrightImage.step[0]);
		int x = 0;
		for (; x < tmpWidth; x += 4)         //ΪʲôtempWidth�е�������
		{
			pOutBrightLine[0] = table[pInDarkLine[0]];
			pOutBrightLine[1] = table[pInDarkLine[1]];
			pOutBrightLine[2] = table[pInDarkLine[2]];
			pOutBrightLine[3] = table[pInDarkLine[3]];
			pOutBrightLine += 4;
			pInDarkLine += 4;
		}
		for (; x < nWidth; x++)
		{
			pOutBrightLine[0] = table[pInDarkLine[0]];
			pOutBrightLine++;
			pInDarkLine++;
		}
	}
	delete[]table;
	return true;

}
//�Ҷ�ͼ����gamma�任
bool CHDRPlus_Tonemapping::GrayGammaCorrect(cv::Mat& pInGrayImage, cv::Mat& pOutGammaImage)
{
	int nWidth = pInGrayImage.cols;
	int nHeight = pInGrayImage.rows;
	//�ȳ�ʼ����ֵ
	pOutGammaImage = cv::Mat(pInGrayImage.size(), CV_16UC1);

	int cutoff = 200;                   // ceil(0.00304 * UINT16_MAX)
	float gamma_toe = 12.92;
	float gamma_pow = 0.416667;         // 1 / 2.4
	float gamma_fac = 680.552897;       // 1.055 * UINT16_MAX ^ (1 - gamma_pow);
	float gamma_con = -3604.425;        // -0.055 * UINT16_MAX

	unsigned short* table = new unsigned short[m_nMax];
	for (int k = 0;k < m_nMax + 1;k++)
	{
		long long int tmp;
		if (k < cutoff)
		{
			tmp = gamma_toe * k;
		}
		else 
		{
			tmp = gamma_fac * pow(k, gamma_pow) + gamma_con;
		}
		table[k] = CLIP(tmp, m_nMin, m_nMax);
	}

#pragma omp parallel for
	for (int y = 0;y < nHeight;y++)
	{
		unsigned short* pInLine = (unsigned short*)(pInGrayImage.data + y * pInGrayImage.step[0]);
		unsigned short* pOutLine = (unsigned short*)(pOutGammaImage.data + y * pOutGammaImage.step[0]);
		for (int x = 0;x < nWidth;x++)
		{
			pOutLine[0] = table[pInLine[0]];
			pOutLine++;
			pInLine++;
		}
	}
	delete[] table;
	return true;
}
//��gamma�任
bool CHDRPlus_Tonemapping::GammaInverse(cv::Mat& pInGrayImage, cv::Mat& pOutInverseImage)
{
	int nWidth = pInGrayImage.cols;
	int nHeight = pInGrayImage.rows;
	pOutInverseImage = cv::Mat(pInGrayImage.size(), CV_16UC1);

	unsigned int cutoff = 2575;                   // ceil(1/0.00304 * UINT16_MAX)
	float gamma_toe = 0.0774f;            // 1 / 12.92
	float gamma_pow = 2.4f;
	float gamma_fac = 57632.49226f;       // 1 / 1.055 ^ gamma_pow * U_INT16_MAX;
	float gamma_con = 0.055f;

	unsigned short* table = new unsigned short[m_nMax];
	for (int k = 0;k < m_nMax + 1;k++)
	{
		long long int tmp;
		if (k < cutoff)
		{
			tmp = gamma_toe * k;
		}
		else
		{
			tmp = (pow((float)k / (float)m_nMax + gamma_con, gamma_pow) * gamma_fac);
		}
		table[k] = CLIP(tmp, m_nMin, m_nMax);
	}

#pragma omp parallel for
	for (int y = 0;y < nHeight;y++)
	{
		unsigned short* pInLine = (unsigned short*)(pInGrayImage.data + y * pInGrayImage.step[0]);
		unsigned short* pOutLine = (unsigned short*)(pOutInverseImage.data + y * pOutInverseImage.step[0]);
		for (int x = 0;x < nWidth;x++)
		{
			pOutLine[0] = table[pInLine[0]];
			pOutLine++;
			pInLine++;
		}
	}
	delete[] table;
	return true;
}

//��˹������
bool CHDRPlus_Tonemapping::GaussPyramidImage(cv::Mat& InputImage, cv::Mat pOutPyarmImage[], cv::Mat pOutEdgeImage[], int& nPyramidLevel, bool SaveEdge)
{
	cv::Mat TempImagedown = InputImage;
	cv::Mat TempImageup;
	pOutPyarmImage[0] = InputImage;

	for (int i = 1;i < nPyramidLevel;i++)
	{
		if (TempImagedown.cols <= 4 || TempImagedown.rows <= 4)
		{
			nPyramidLevel = i;
			printf("out PyramidLevel=%d\n", nPyramidLevel);
			break;
		}

		cv::pyrDown(TempImagedown, pOutPyarmImage[i]);//cv::Size(InputImage.cols / pow(2, i), InputImage.rows / pow(2, i)

		if (SaveEdge)
		{
			cv::pyrUp(pOutPyarmImage[i], TempImageup, cv::Size(TempImagedown.cols, TempImagedown.rows));
			//��Ҫת��ΪCV_32FC1���ͣ���Ϊ���ڸ���
			cv::subtract(TempImagedown, TempImageup, pOutEdgeImage[i - 1], cv::Mat(),CV_32FC1);
		}
		TempImagedown = pOutPyarmImage[i];

	}
	return true;
}

//����Ȩ��ͼ
bool CHDRPlus_Tonemapping::BuildWeight(cv::Mat& pDarkGammaImage, cv::Mat& pBrightGammaImage, cv::Mat& DarkWeightImage, cv::Mat& BrightWeightImage,int ScaleBit)
{
	int ScaleValue = 1 << ScaleBit;
	int nWidth = pDarkGammaImage.cols;
	int nHeight = pDarkGammaImage.rows;
	int nDim = pDarkGammaImage.dims;

	DarkWeightImage = cv::Mat(pDarkGammaImage.size(), CV_16UC1);
	BrightWeightImage = cv::Mat(pDarkGammaImage.size(), CV_16UC1);

	float* WeightTable = new float[m_nMax + 1];
	for (int k = 0;k < m_nMax + 1;k++)
	{
		float darks = ((float)k / (float)m_nMax - 0.5f);
		WeightTable[k] = exp(-12.5f * (darks * darks));
	}
#pragma omp parallel for
	for (int y = 0;y < nHeight;y++)
	{
		unsigned short* pDarkgammaline = (unsigned short*)(pDarkGammaImage.data + y * pDarkGammaImage.step[0]);
		unsigned short* pBrightgammaline = (unsigned short*)(pBrightGammaImage.data + y * pBrightGammaImage.step[0]);
		unsigned short* DarkWeightline = (unsigned short*)(DarkWeightImage.data + y * DarkWeightImage.step[0]);
		unsigned short* BrightWeightline = (unsigned short*)(BrightWeightImage.data + y * BrightWeightImage.step[0]);
		for (int x = 0;x < nWidth;x++)
		{
			float DarkCurve = WeightTable[pDarkgammaline[0]];
			float BrightCurve = WeightTable[pBrightgammaline[0]];
			float DarkWeight = (DarkCurve / (DarkCurve + BrightCurve));
			float BrightWeight = 1.f - DarkWeight;
			//Ȩ�ع�һ����Ŵ�ScaleValue��,һ��Ҫ�Ŵ���Ϊ��һ����С��1������Ϊ���޷��Ŷ����Σ����Ի�ȫ��0
			DarkWeightline[0] = DarkWeight * ScaleValue;
			BrightWeightline[0] = BrightWeight * ScaleValue;

			pDarkgammaline++;
			pBrightgammaline++;
			DarkWeightline++;
			BrightWeightline++;
		}
	}
	delete[] WeightTable;
	return true;
}
//��ϸ�˹����������gammaת����İ�ͼ����ͼ�ں�
bool CHDRPlus_Tonemapping::CombineDarkAndBrightImage(cv::Mat& pDarkGammaImage, cv::Mat& pBrightGammaImage, cv::Mat& pOutCombineImage)
{
	int ScaleBit = 12;
	int nWidth = pDarkGammaImage.cols;
	int nHeight = pDarkGammaImage.rows;
	int nDim = pDarkGammaImage.dims;
	cv::Mat DarkWeightImage, BrightWeightImage;
	cv::Mat DarkImagePyramid[12], BrightImagePyramid[12], DarkWeightImagePyramid[12], BrightWeightImagePyramid[12];
	cv::Mat DarkImagePyramidEdge[12], BrightImagePyramidEdge[12];
	int nPyramidLevel = 12;

	//����ͼ������ͼ�����и�˹��������������������˹��������Եͼ��
	GaussPyramidImage(pDarkGammaImage, DarkImagePyramid, DarkImagePyramidEdge, nPyramidLevel, true);
	GaussPyramidImage(pBrightGammaImage, BrightImagePyramid, BrightImagePyramidEdge, nPyramidLevel, true);

	//�õ�Ȩ��ͼ
	BuildWeight(pDarkGammaImage, pBrightGammaImage, DarkWeightImage, BrightWeightImage, ScaleBit);
	//�԰�Ȩ��ͼ������Ȩ��ͼ�����и�˹������
	GaussPyramidImage(DarkWeightImage, DarkWeightImagePyramid, NULL, nPyramidLevel, false);
	GaussPyramidImage(BrightWeightImage, BrightWeightImagePyramid, NULL, nPyramidLevel, false);


	DarkImagePyramid[nPyramidLevel - 1] = DarkImagePyramid[nPyramidLevel - 1].mul(DarkWeightImagePyramid[nPyramidLevel - 1]) / 4096;
	BrightImagePyramid[nPyramidLevel - 1] = BrightImagePyramid[nPyramidLevel - 1].mul(BrightWeightImagePyramid[nPyramidLevel - 1]) / 4096;
	DarkImagePyramid[nPyramidLevel-1] = DarkImagePyramid[nPyramidLevel - 1] + BrightImagePyramid[nPyramidLevel - 1];

	//��Ȩ�صĵ�Ƶbaseͼͨ���ϲ��������ںϱ�Ե��Ƶͼ��
	pOutCombineImage = DarkImagePyramid[nPyramidLevel - 1];
	for (int i = nPyramidLevel - 2;i >= 0;i--)
	{
		//ͼ���ϲ������󳤿�2��,����DarkImagePyramidEdge�Ŀ�ߣ���ֹ�л��������1
		cv::pyrUp(pOutCombineImage, pOutCombineImage, cv::Size(DarkImagePyramidEdge[i].cols, DarkImagePyramidEdge[i].rows));
		//���ںϱ�Ե��Ƶͼ��
		//��Ƶedgeͼ����*����Ӧ��Ȩ��ͼ = ��Ȩ�صĸ�Ƶedgeͼ
		//��Ҫͳһ�������ͣ�����ʹ��.mul����
		DarkWeightImagePyramid[i].convertTo(DarkWeightImagePyramid[i], CV_32FC1);
		BrightWeightImagePyramid[i].convertTo(BrightWeightImagePyramid[i], CV_32FC1);

		DarkImagePyramidEdge[i] = (DarkImagePyramidEdge[i].mul(DarkWeightImagePyramid[i])) / 4096;   
		BrightImagePyramidEdge[i] = (BrightImagePyramidEdge[i].mul(BrightWeightImagePyramid[i])) / 4096;
		DarkImagePyramidEdge[i] = DarkImagePyramidEdge[i] + BrightImagePyramidEdge[i];
		//��Ƶ���Ƶ���
		cv::add(pOutCombineImage, DarkImagePyramidEdge[i], pOutCombineImage, cv::Mat(), CV_16UC1);
		//pOutCombineImage = pOutCombineImage + DarkImagePyramidEdge[i];
	}
	//pOutCombineImage.convertTo(pOutCombineImage, CV_16UC1);
	return true;
}

void CHDRPlus_Tonemapping::GammaCombinRGB(cv::Mat& pRGBImage, cv::Mat& pGrayImage, cv::Mat& pDarkImage)
{
	int nWidth = pRGBImage.cols;
	int nHeight = pRGBImage.rows;
#pragma omp parallel for
	for (int y = 0; y < nHeight; y++)
	{
		//��ͼ���ںϵõ���pDarkImageͼ��ΪpRGBImageͼɫ��ӳ�������gain���Ӷ�ʵ���˾ֲ�ɫ��ӳ��
		unsigned short* pInRGBLine = (unsigned short*)(pRGBImage.data + y * pRGBImage.step[0]);
		unsigned short* pInGrayLine = (unsigned short*)(pGrayImage.data + y * pGrayImage.step[0]);
		unsigned short* pInDarkLine = (unsigned short*)(pDarkImage.data + y * pDarkImage.step[0]);
		for (int x = 0; x < nWidth; x++)
		{
			unsigned int R, G, B, Gain;
			unsigned short Max = *pInGrayLine++;
			Gain = pInDarkLine[0];
			if (Max == 0)
			{
				Max = 1;
			}
			if (Gain == 0)
			{
				Gain = 1;
			}
			Gain = Gain << 12;
			Gain = Gain / Max;   //gain = pDarkImageͼ��Ӧ����ֵ����/pGrayImage��Ӧ����ֵ
			if (Gain > 4096)    //2��12�η�=4096
			{
				R = pInRGBLine[0] * Gain;
				G = pInRGBLine[1] * Gain;
				B = pInRGBLine[2] * Gain;
				R >>= 12;
				G >>= 12;
				B >>= 12;
				pInRGBLine[0] = CLIP(R, m_nMin, m_nMax);
				pInRGBLine[1] = CLIP(G, m_nMin, m_nMax);
				pInRGBLine[2] = CLIP(B, m_nMin, m_nMax);
			}
			pInRGBLine += 3;
			pInDarkLine++;
		}
	}
}

bool CHDRPlus_Tonemapping::Forward(cv::Mat& pInRGBData, TGlobalControl* pControl)
{
	m_nMax = pControl->nWP;
	m_nMin = pControl->nBLC;

	cv::Mat GrayImage;
	//cv::Mat DarkImage;
	cv::Mat DarkGammaImage;
	cv::Mat BrightImage(pInRGBData.size(), CV_16UC1);
	cv::Mat BrightGammaImage;
	cv::Mat OutImage;
	cv::cvtColor(pInRGBData, GrayImage, cv::COLOR_BGR2GRAY);
	if (!EstimateDigiGain(GrayImage, pControl))return false;

	float comp = (float)m_nDynamicCompression / (float)128;  //5.8f
	float gain = (float)pControl->nDigiGain / (float)128;    //0.5f
	float comp_const = 1.f + comp / m_nVirtualExposureNum;
	float gain_const = 1.f + gain / m_nVirtualExposureNum;
	float comp_slope = (comp - comp_const) / (float)(m_nVirtualExposureNum - 1);
	float gain_slope = (gain - gain_const) / (float)(m_nVirtualExposureNum - 1);
	//����ֱ��ʹ��=��ֵ������cv::Mat DarkImage��GrayImage��;
	//ʹ�ø�ֵ������Ϳ������캯��ֻ����ԭ������Ϣͷ��������в�����������ԭ������.
	//ʹ�ú���clone()����copyTo�ɸ���һ��ͼ��ľ��󣬶�����в�������Ӱ��ԭ����
	cv::Mat DarkImage = GrayImage.clone();

	for (int pass = 0;pass < m_nVirtualExposureNum;pass++)
	{
		float norm_comp = pass * comp_slope + comp_const;//����
		float norm_gain = pass * gain_slope + gain_const;//��С
		printf("norm_comp=%f norm_gain=%f \n", norm_comp, norm_gain);
		Brighten(DarkImage, norm_comp, BrightImage);
		GrayGammaCorrect(DarkImage, DarkGammaImage);
		GrayGammaCorrect(BrightImage, BrightGammaImage);
		CombineDarkAndBrightImage(DarkGammaImage, BrightGammaImage, OutImage);
		GammaInverse(OutImage, DarkGammaImage);
		Brighten(DarkGammaImage, norm_gain, DarkImage);
	}
	GammaCombinRGB(pInRGBData, GrayImage, DarkImage);
	return true;

	// Merge using Exposure Fusion ͼ���ں�
	//cv::cout << "Merging using Exposure Fusion ... " << endl;
	//cv::Mat exposureFusion;
	//Ptr<MergeMertens> mergeMertens = createMergeMertens();
	//mergeMertens->process(images, exposureFusion);
	//cv::createMergeMertens();
}