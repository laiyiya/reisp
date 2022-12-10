#include "HDRPlus_Forward.h"

void CHDRPlus_Forward::Forward(cv::Mat& RawData, cv::Mat& OutRGBData, TGlobalControl* pControl)
{
	cv::Mat OutRawData(cv::Size(RawData.cols, RawData.rows), CV_16UC1);
	//cv::Mat OutRGBData;
	//������
	if (m_nDPCorrectionEnable)
	{

		m_HDRPlus_DPCorrection.Forward(RawData, OutRawData, pControl);
		if (m_HDRPlus_DPCorrection.m_bDumpFileEnable)
		{
			cv::Mat tmp = OutRawData;
			tmp.convertTo(tmp, CV_8UC1, 255.0 / 65535, 0);
			//cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_GRAY2RGB);
			cv::imwrite("Outbmp/DPCorrection.bmp", tmp);
		}
	}
	//�ڵ�ƽУ��
	if (m_nBlackLevelCorrectEnable)
	{

		m_HDRPlus_BalckLevelCorrect.Forward(OutRawData, pControl);
		if (m_HDRPlus_BalckLevelCorrect.m_bDumpFileEnable)
		{
			cv::Mat tmp = OutRawData;
			tmp.convertTo(tmp, CV_8UC1, 255.0 / 65535, 0);
			//cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_GRAY2RGB);
			cv::imwrite("Outbmp/BalckLevelCorrect.bmp", tmp);
		}
	}
	//��ƽ��
	if (m_nWhiteBalanceEnable)
	{
		
		m_HDRPlusWhiteBalance.Forward(OutRawData, pControl);
		if (m_HDRPlusWhiteBalance.m_bDumpFileEnable)
		{
			cv::Mat tmp = OutRawData;
			tmp.convertTo(tmp, CV_8UC1, 255.0 / 65535, 0);
			//cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_GRAY2RGB);
			cv::imwrite("Outbmp/WhiteBalance.bmp", tmp);
		}
	}
	//ȥ������
	if (m_nDemosaicingEnable)
	{
			m_HDRPlus_Demosaicing.Forward(OutRawData, OutRGBData);
			if (m_HDRPlus_Demosaicing.m_bDumpFileEnable)
			{
				cv::Mat tmpOutRGB = OutRGBData;
				tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
				cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
				cv::imwrite("Outbmp/Demosaicing.bmp", tmpOutRGB);
			}
	}
	//CCMɫ��У��
	if (m_nColorCorrectEnable)
	{
		m_HDRPlus_ColorCorrect.Forward(OutRGBData, pControl);
		if (m_HDRPlus_ColorCorrect.m_bDumpFileEnable)
		{
			cv::Mat tmpOutRGB = OutRGBData;
			tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
			cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
			cv::imwrite("Outbmp/ColorCorrect.bmp", tmpOutRGB);
		}
	}

	//LocalTonemappingɫ��ӳ��
	if (m_nTonemappingEnable)
	{
		m_HDRPlus_Tonemapping.Forward(OutRGBData, pControl);
		if (m_HDRPlus_Tonemapping.m_bDumpFileEnable)
		{
			cv::Mat tmpOutRGB = OutRGBData.clone();
			tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
			cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
			cv::imwrite("Outbmp/Tonemapping.bmp", tmpOutRGB);
		}
	}

	//Gamma
	if (m_nGammaCorrectEnable)
	{
		m_HDRPlus_GammaCorrect.Forward(OutRGBData, pControl);
		if (m_HDRPlus_GammaCorrect.m_bDumpFileEnable)
		{
			cv::Mat tmpOutRGB = OutRGBData.clone();
			tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
			cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
			cv::imwrite("Outbmp/GammaCorrect.bmp", tmpOutRGB);
		}
	}
	//������
	if (m_nChromaDenoiseEnable)
	{
		m_HDRPlus_ChromaDenoise.Forward(OutRGBData, pControl);
		if (m_HDRPlus_ChromaDenoise.m_bDumpFileEnable)
		{
			cv::Mat tmpOutRGB = OutRGBData.clone();
			tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
			cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
			cv::imwrite("Outbmp/ChromaDenoise.bmp", tmpOutRGB);
		}
	}
	//�����Աȶ�
	if (m_nContrastEnable)
	{
		m_HDRPlus_Contrast.Forward(OutRGBData, pControl);
		if (m_HDRPlus_Contrast.m_bDumpFileEnable)
		{
			cv::Mat tmpOutRGB = OutRGBData.clone();
			tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
			cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
			cv::imwrite("Outbmp/Contrast.bmp", tmpOutRGB);
		}
	}
	//����ǿ������
	if (m_nSharpenEnable)
	{
		m_HDRPlus_Sharpen.Forward(OutRGBData);
		if (m_HDRPlus_Sharpen.m_bDumpFileEnable)
		{
			cv::Mat tmpOutRGB = OutRGBData.clone();
			tmpOutRGB.convertTo(tmpOutRGB, CV_8UC3, 255.0 / 65535, 0);
			cv::cvtColor(tmpOutRGB, tmpOutRGB, cv::COLOR_BGR2RGB);
			cv::imwrite("Outbmp/Sharpen.bmp", tmpOutRGB);
		}
	}
	if (m_bDumpFileEnable)
	{
		OutRGBData.convertTo(OutRGBData, CV_8UC3, 255.0 / 65535, 0);
		cv::cvtColor(OutRGBData, OutRGBData, cv::COLOR_BGR2RGB);
		cv::imwrite("Outbmp/Normalize.bmp", OutRGBData);
	}
}
