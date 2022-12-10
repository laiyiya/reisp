
#ifndef __CONFIG_FILTER_H_
#define __CONFIG_FILTER_H_
//#include "SubFunction.h"
//#include "MultiUshortImage.h"
//#include "MultiUcharImage.h"
//#include "SingleUcharImage.h"
//#include "MultiIntImage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Basicdef.h"

//����һ���ṹ�嵱���洢���ò����Ľڵ�
typedef struct NodeConfigParameter {
	char m_pConfigParamName[64];                // ������
	int* m_pConfigParamAddr;                    // ָ��ֵ��ָ��
	int m_nConfigParamMin;                      // ��������Сֵ
	int m_nConfigParamMax;                      // ���������ֵ
	int m_nConfigParamScale;                    // ��������Сstep
	NodeConfigParameter* m_pConfigParamNext;           // ָ����һ��������
	NodeConfigParameter() {
		m_pConfigParamAddr = NULL;
		m_pConfigParamNext = NULL;
		m_nConfigParamMin = 0;
		m_nConfigParamMax = 0x7fffffff; //2��32�η�-1
	}
}NConfigParam;

//һ�������࣬�������ṹ���������ز��������㽫һϵ�в�������������
class CConfigParamList 
{
private:
	int m_nConfigParamListCount;                  // ����ڵ����,����������
	NConfigParam* m_pConfigParamListFrist;        
	NConfigParam* m_pConfigParamListLast;         // ���������ڵ�ָ�룬�ֱ�ָ�������ͷ��β
	//�����ļ��е�һ��linestr���ֱ𽫲�������ֵ����name��val
	bool GetConfigParamListValueFromLineStr(char* linestr, char* name, int& val)
	{
		int i, len;
		len = strlen(linestr);
		for (i = 0;i < len;i++) 
		{
			if (linestr[i] == '=') 
			{
				if (linestr[i + 1] == 'H' || linestr[i + 1] == 'h')
				{
					if (sscanf(linestr + i + 2, "%x\n", &val) != 1)return false;
				}
				else
				{
					if (sscanf(linestr + i + 1, "%x\n", &val) != 1)return false;
				}
				//��linestr����i���ֽڳ��ȵ�name��ַ
				memcpy(name, linestr, i);
				name[i] = '\0';    //����ַ���������
				return true;
			}
		}
		return false;
	}
public:
	//�����������һ���ڵ㣬�����һ������
	bool ConfigParamListAddVariable(const char* pParamName, int* pParamAddr, int nMin = 0, int nMax = 0x7fffffff, int nScale = 1)
	{
		//newһ����㣬������ز���
		NConfigParam* pParamTemp = new NConfigParam;
		//�ж�һ�½���Ƿ�new�ɹ�
		if (pParamTemp == NULL)return false;
		//��������ôд��pParamTemp->m_pConfigParamName = pParamName;
		strcpy(pParamTemp->m_pConfigParamName, pParamName);
		pParamTemp->m_pConfigParamAddr = pParamAddr;
		pParamTemp->m_nConfigParamMin = nMin;
		pParamTemp->m_nConfigParamMax = nMax;
		pParamTemp->m_nConfigParamScale = nScale;
		pParamTemp->m_pConfigParamNext = NULL;

		if (m_pConfigParamListFrist == NULL)   //�������ǿ�
		{
			m_pConfigParamListFrist = m_pConfigParamListLast = pParamTemp;
		}
		else
		{
			m_pConfigParamListLast->m_pConfigParamNext = pParamTemp;
			m_pConfigParamListLast = pParamTemp;
		}
		m_nConfigParamListCount++;
		return true;

	}
	//�������������ڵ㣬����Ӷ������
	bool ConfigParamListAddArray(const char* pParamName, int pParamAddr[],int nCh, int nMin = 0, int nMax = 0x7fffffff, int nScale = 1)
	{
		char ParamName[64];
		for (int i = 0;i < nCh;i++)
		{
			//��pParamName��i��"%s_%d"��ʽ����ParamName
			sprintf(ParamName, "%s_%d", pParamName, i);
			//��Ӳ���
			if (!ConfigParamListAddVariable(pParamName, pParamAddr + i, nMin, nMax, nScale))return false;
		}
		return true;
	}
	bool ConfigParamListAddMatrixRow(const char* pParamName, int* pMatrixRow, int nRow, int nCol, int nMin = 0, int nMax = 0x7fffffff, int nScale = 1)
	{
		char ParamName[64];
		sprintf(ParamName, "%s_%d", pParamName, nRow);
		if (!ConfigParamListAddArray(ParamName, pMatrixRow, nCol, nMin, nMax, nScale))return false;
		return true;
	}
	
	//�����������ҽڵ�����ΪpName�Ľڵ㣬�����ظýڵ�
	NConfigParam* FindConfigParamName(char* pName)
	{
		NConfigParam* pTemp = m_pConfigParamListFrist;
		while (pTemp != NULL)
		{
			//�Ƚ��ַ�����str1��str2�Ƿ���ͬ�������ͬ�򷵻�0
			//�����ͬ���ڲ�ͬ���ַ������str1���ַ�����str2���ַ����򷵻�1�����򷵻� - 1
			if(strcmp(pTemp->m_pConfigParamName, pName)==0)return pTemp;
			pTemp = pTemp->m_pConfigParamNext;
		}
		return pTemp;
	}
	
	// ���ļ��аѲ������ؽ������ �����ڵ��øú���ǰ�Ѿ����ڣ�
	// ������Ҫ�ж϶������Ĳ������Ƿ��������У���������ھͱ����������޸Ĳ���ֵ
	bool LoadParamListFromFile(FILE* fp)
	{
		char linestr[256], ParamName[64];
		int ParamValue;
		NConfigParam* pTemp;
		if (fp == NULL)return false;
		for (int i = 0;i < m_nConfigParamListCount;i++)
		{
			fgets(linestr, 256, fp);
			if (!GetConfigParamListValueFromLineStr(linestr, ParamName, ParamValue))return false;
			pTemp = FindConfigParamName(ParamName);
			if (pTemp != NULL)         //�ǿգ�˵���ҵ���ΪParamName�Ľ��
			{
				if (ParamValue > pTemp->m_nConfigParamMax || ParamValue < pTemp->m_nConfigParamMin)
				{
					printf("%s ConfigParam value %d is out of ValueRange [%d,%d]!!!\n", ParamName, ParamValue, pTemp->m_nConfigParamMin, pTemp->m_nConfigParamMax);
					if (ParamValue > pTemp->m_nConfigParamMax)
					{
						ParamValue = pTemp->m_nConfigParamMax;
					}
					if (ParamValue < pTemp->m_nConfigParamMin)
					{
						ParamValue = pTemp->m_nConfigParamMin;
					}
				}
			}
			else
			{
				printf("is not Parameter name == %s!!!\n", ParamName);
				return false;
			}

		}
	}

	// ��������ÿ���ڵ�Ĳ���д���ļ�
	bool SaveParamListToFile(FILE* fp)
	{
		NConfigParam* pTemp = m_pConfigParamListFrist;
		if (fp == NULL)return false;
		for (int i = 0; i < m_nConfigParamListCount; i++)
		{
			int ParamValue = *(pTemp->m_pConfigParamAddr);
			int res = fprintf(fp, "%s=%d;\tValueRange=[%d,%d,%d]\n", pTemp->m_pConfigParamName, ParamValue, pTemp->m_nConfigParamMin, pTemp->m_nConfigParamMax, pTemp->m_nConfigParamScale);
			pTemp = pTemp->m_pConfigParamNext;
		}
		return true;
	}

	//���������������������ʹ��
	void ClearConfigParamListMem()
	{
		while (m_pConfigParamListFrist != NULL) {
			m_pConfigParamListLast = m_pConfigParamListFrist->m_pConfigParamNext;
			delete m_pConfigParamListFrist;
			m_pConfigParamListFrist = m_pConfigParamListLast;
		}
		m_nConfigParamListCount = 0;
	}
	//���캯��
	CConfigParamList()
	{
		m_pConfigParamListFrist = m_pConfigParamListLast = NULL;
		m_nConfigParamListCount = 0;
	}
	//��������
	~CConfigParamList()
	{
		ClearConfigParamListMem();
	}

};



//����һЩ�麯��������̳�ʱ��д��ͬʱ����ʵ������һ����������һ�������洢�������������title
class CSingleConfigTitleFILE
{
protected:
	virtual void CreateConfigTitleName() = 0;//��������----------���麯��������ʵ��
	virtual void InitConfigParamList() {};//���������б�----------�麯��������ʵ�֣�������Ա����m_nConfigParamList�б�
	virtual void UpdateInternalConfig() {};//���²����б�----------�麯��������ʵ��
public:
	char m_pConfigTitleName[256];//����
	CConfigParamList m_nConfigParamList;//�����б�

	virtual void Initialize()//�����ʼ�����ȵ�������ɸ����黯������ʵ��
	{
		CreateConfigTitleName();
		InitConfigParamList();
	}

	virtual bool LoadConfigFile(char* pConfigFileName)//����û��ʵ���и���ʵ��
	{
		char ConfigTitleName[64];
		//��rt�� ֻ����һ���ı��ļ���ֻ���������
		FILE* fp = fopen(pConfigFileName, "rt");
		if (fp == NULL)return false;
		if (fscanf(fp, "\n%s\n", ConfigTitleName) <= 0)
		{
			fclose(fp);
			return false;
		}
		if (!(strcmp(m_pConfigTitleName, ConfigTitleName) == 0))
		{
			printf("Can not match Title name %s!!!\n", ConfigTitleName);
			fclose(fp);
			return false;
		}
		if (!m_nConfigParamList.LoadParamListFromFile(fp))
		{
			fclose(fp);
			return false;
		}
		UpdateInternalConfig();
		fclose(fp);
		return true;
	}
	virtual bool SaveSingleConfigFile(char* pFileName)
	{
		FILE* fp = fopen(pFileName, "wt");
		if (fp == NULL)return false;
		fprintf(fp, "\n%s\n", m_pConfigTitleName);//�������
		if (!m_nConfigParamList.SaveParamListToFile(fp))//��������б�
		{
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
};


//��������ڶ�ά����ÿ���ڵ���һ���������
class CMultiConfigFILE : public CSingleConfigTitleFILE
{
protected:
	//����һ���ڵ㣬�ýڵ��Ǵ���title��ÿ���������
	typedef struct nodeConfigTitleList
	{
		CSingleConfigTitleFILE* pSingleConfigTitleFILE;   //������ָ��
		nodeConfigTitleList* pTitleListNext;               //�����ָ��
		nodeConfigTitleList()
		{
			pSingleConfigTitleFILE = NULL;
			pTitleListNext = NULL;
		}
	}NConfigTitleList;//��Ŷ�����ĵ�

	NConfigTitleList* m_pConfigTitleListFirst;
	NConfigTitleList* m_pConfigTitleListLast;
	int m_nConfigTitleListCount;

	void ClearMultiConfigMem()//��գ��൱��û�� countҲ��0
	{
		while (m_pConfigTitleListFirst != NULL)
		{
			m_pConfigTitleListLast = m_pConfigTitleListFirst->pTitleListNext;
			delete m_pConfigTitleListFirst;
			m_pConfigTitleListFirst = m_pConfigTitleListLast;
		}
		m_nConfigTitleListCount = 0;
	}

	//��˳�����ÿ��ģ��Ĳ�������
	void AddConfigTitle(CSingleConfigTitleFILE* pSingleConfig, char* pNewConfigName = NULL)
	{
		if (pNewConfigName != NULL)
		{
			//����һ���ַ�����������ַ���ƴ������һ���ַ�������
			strcat(pSingleConfig->m_pConfigTitleName, pNewConfigName);
		}
		NConfigTitleList* pTmpTitleList = new NConfigTitleList;
		pTmpTitleList->pSingleConfigTitleFILE = pSingleConfig;
		if (m_pConfigTitleListFirst == NULL)
		{
			m_pConfigTitleListFirst = pTmpTitleList;
		}
		else
		{
			m_pConfigTitleListLast->pTitleListNext = pTmpTitleList;
		}
		m_pConfigTitleListLast = pTmpTitleList;
		m_nConfigTitleListCount++;
	}
	virtual void CreateConfigTitleNameList() = 0;//������ʵ��
public:
	virtual void Initialize()//�����ʼ����
	{
		CreateConfigTitleName();//�ܱ��ⴴ��
		CreateConfigTitleNameList();//�ֱ��ⴴ��
		InitConfigParamList();//��������Ĳ�������
	}


	CMultiConfigFILE()
	{
		m_pConfigTitleListFirst = m_pConfigTitleListLast = NULL;
		m_nConfigTitleListCount = 0;
	}
	~CMultiConfigFILE()
	{
		ClearMultiConfigMem();
	}

	// ���ļ��м��ز���
	virtual bool LoadMultiConfigFile(char* pFileName)
	{
		char pConfigTitle[64];
		FILE* fp = fopen(pFileName, "rt");
		if (fp == NULL)return false;
		if (fscanf(fp, "\n%s\n", pConfigTitle) <= 0)
		{
			fclose(fp);
			return false;
		}
		//�ж��ܱ����Ƿ����pConfigTitle�� �ַ����ȽϺ���strcmp���ж��ַ����Ƿ���ͬ
		if (!(strcmp(m_pConfigTitleName, pConfigTitle) == 0))
		{
			printf("Can not match Title name %s!!!\n", pConfigTitle);
			fclose(fp);
			return false;
		}
		if (!m_nConfigParamList.LoadParamListFromFile(fp))//��������ౣ��ı���Ͳ�������Ա����������ģ�
		{
			fclose(fp);
			return false;
		}
		int i = 0;
		while (i < m_nConfigTitleListCount)//��������ȡ
		{
			if (fscanf(fp, "\n%s\n", pConfigTitle) <= 0) //�ļ��ж�ȡһ��
			{
				fclose(fp);
				return false;
			}
			CSingleConfigTitleFILE* pSingleConfig = NULL;
			NConfigTitleList* pTmpTitleList = m_pConfigTitleListFirst;
			while (pTmpTitleList != NULL)
			{
				if ((strcmp(pTmpTitleList->pSingleConfigTitleFILE->m_pConfigTitleName, pConfigTitle) == 0))//�Ա��ļ��ж�ȡ�ĺ������еģ��еĻ��ͽ������е���ȡ����
				{
					pSingleConfig = pTmpTitleList->pSingleConfigTitleFILE;
					break;
				}
				pTmpTitleList = pTmpTitleList->pTitleListNext;
			}
			if (pSingleConfig == NULL)
			{
				printf("Can not find Title name %s!!!\n", pConfigTitle);
				return false;
			}
			if (!pSingleConfig->m_nConfigParamList.LoadParamListFromFile(fp))
			{
				fclose(fp);
				return false;
			}
			UpdateInternalConfig();
			i++;
		}
		fclose(fp);
		return true;
	}

	virtual bool SaveMultiConfigFile(char* pFileName)
	{
		FILE* fp = fopen(pFileName, "wt");
		if (fp == NULL)return false;
		fprintf(fp, "\n%s\n", m_pConfigTitleName);//��������ౣ��ı���Ͳ�������Ա����������ģ�
		if (!m_nConfigParamList.SaveParamListToFile(fp))
		{
			fclose(fp);
			return false;
		}
		NConfigTitleList* pTmpTitleList = m_pConfigTitleListFirst;//�ļ���������ȡ���ݴӵ�һ��ȡ
		while (pTmpTitleList != NULL)
		{
			fprintf(fp, "\n%s\n", pTmpTitleList->pSingleConfigTitleFILE->m_pConfigTitleName);
			if (!pTmpTitleList->pSingleConfigTitleFILE->m_nConfigParamList.SaveParamListToFile(fp))
			{
				fclose(fp);
				return false;
			}
			pTmpTitleList = pTmpTitleList->pTitleListNext;//ȡ��һ��
		}
		fclose(fp);
		return true;
	}
};
#endif