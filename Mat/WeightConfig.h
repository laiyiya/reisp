
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

//创建一个结构体当做存储配置参数的节点
typedef struct NodeConfigParameter {
	char m_pConfigParamName[64];                // 参数名
	int* m_pConfigParamAddr;                    // 指向值的指针
	int m_nConfigParamMin;                      // 参数的最小值
	int m_nConfigParamMax;                      // 参数的最大值
	int m_nConfigParamScale;                    // 参数的最小step
	NodeConfigParameter* m_pConfigParamNext;           // 指向下一个结点参数
	NodeConfigParameter() {
		m_pConfigParamAddr = NULL;
		m_pConfigParamNext = NULL;
		m_nConfigParamMin = 0;
		m_nConfigParamMax = 0x7fffffff; //2的32次方-1
	}
}NConfigParam;

//一个链表类，对上述结构体结点进行相关操作，方便将一系列参数构建成链表
class CConfigParamList 
{
private:
	int m_nConfigParamListCount;                  // 链表节点个数,即参数个数
	NConfigParam* m_pConfigParamListFrist;        
	NConfigParam* m_pConfigParamListLast;         // 创建两个节点指针，分别指向链表的头和尾
	//解析文件中的一行linestr，分别将参数名和值传给name和val
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
				//从linestr复制i个字节长度到name地址
				memcpy(name, linestr, i);
				name[i] = '\0';    //添加字符串结束符
				return true;
			}
		}
		return false;
	}
public:
	//向链表中添加一个节点，即添加一个参数
	bool ConfigParamListAddVariable(const char* pParamName, int* pParamAddr, int nMin = 0, int nMax = 0x7fffffff, int nScale = 1)
	{
		//new一个结点，并赋相关参数
		NConfigParam* pParamTemp = new NConfigParam;
		//判断一下结点是否new成功
		if (pParamTemp == NULL)return false;
		//不可以这么写，pParamTemp->m_pConfigParamName = pParamName;
		strcpy(pParamTemp->m_pConfigParamName, pParamName);
		pParamTemp->m_pConfigParamAddr = pParamAddr;
		pParamTemp->m_nConfigParamMin = nMin;
		pParamTemp->m_nConfigParamMax = nMax;
		pParamTemp->m_nConfigParamScale = nScale;
		pParamTemp->m_pConfigParamNext = NULL;

		if (m_pConfigParamListFrist == NULL)   //若链表是空
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
	//向链表中添多个节点，即添加多个参数
	bool ConfigParamListAddArray(const char* pParamName, int pParamAddr[],int nCh, int nMin = 0, int nMax = 0x7fffffff, int nScale = 1)
	{
		char ParamName[64];
		for (int i = 0;i < nCh;i++)
		{
			//将pParamName，i按"%s_%d"格式传给ParamName
			sprintf(ParamName, "%s_%d", pParamName, i);
			//添加参数
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
	
	//遍历链表，查找节点名字为pName的节点，并返回该节点
	NConfigParam* FindConfigParamName(char* pName)
	{
		NConfigParam* pTemp = m_pConfigParamListFrist;
		while (pTemp != NULL)
		{
			//比较字符串自str1和str2是否相同。如果相同则返回0
			//如果不同，在不同的字符处如果str1的字符大于str2的字符，则返回1，否则返回 - 1
			if(strcmp(pTemp->m_pConfigParamName, pName)==0)return pTemp;
			pTemp = pTemp->m_pConfigParamNext;
		}
		return pTemp;
	}
	
	// 从文件中把参数加载进链表里， 链表在调用该函数前已经存在，
	// 所以需要判断读进来的参数名是否在链表中，如果不存在就报错，存在则修改参数值
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
			if (pTemp != NULL)         //非空，说明找到名为ParamName的结点
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

	// 将链表中每个节点的参数写进文件
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

	//清空链表与析构函数搭配使用
	void ClearConfigParamListMem()
	{
		while (m_pConfigParamListFrist != NULL) {
			m_pConfigParamListLast = m_pConfigParamListFrist->m_pConfigParamNext;
			delete m_pConfigParamListFrist;
			m_pConfigParamListFrist = m_pConfigParamListLast;
		}
		m_nConfigParamListCount = 0;
	}
	//构造函数
	CConfigParamList()
	{
		m_pConfigParamListFrist = m_pConfigParamListLast = NULL;
		m_nConfigParamListCount = 0;
	}
	//析构函数
	~CConfigParamList()
	{
		ClearConfigParamListMem();
	}

};



//设置一些虚函数，方便继承时重写，同时里面实例化了一个链表，并有一个变量存储该组链表参数的title
class CSingleConfigTitleFILE
{
protected:
	virtual void CreateConfigTitleName() = 0;//创建标题----------纯虚函数由子类实现
	virtual void InitConfigParamList() {};//创建参数列表----------虚函数有子类实现（关联成员变量m_nConfigParamList列表）
	virtual void UpdateInternalConfig() {};//更新参数列表----------虚函数由子类实现
public:
	char m_pConfigTitleName[256];//标题
	CConfigParamList m_nConfigParamList;//参数列表

	virtual void Initialize()//子类初始化中先调用他完成父类虚化函数的实现
	{
		CreateConfigTitleName();
		InitConfigParamList();
	}

	virtual bool LoadConfigFile(char* pConfigFileName)//子类没有实现有父类实现
	{
		char ConfigTitleName[64];
		//“rt” 只读打开一个文本文件，只允许读数据
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
		fprintf(fp, "\n%s\n", m_pConfigTitleName);//保存标题
		if (!m_nConfigParamList.SaveParamListToFile(fp))//保存参数列表
		{
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
};


//该类相对于二维链表，每个节点是一组参数链表
class CMultiConfigFILE : public CSingleConfigTitleFILE
{
protected:
	//构建一个节点，该节点是带有title的每组参数链表
	typedef struct nodeConfigTitleList
	{
		CSingleConfigTitleFILE* pSingleConfigTitleFILE;   //单项类指针
		nodeConfigTitleList* pTitleListNext;               //单项表指针
		nodeConfigTitleList()
		{
			pSingleConfigTitleFILE = NULL;
			pTitleListNext = NULL;
		}
	}NConfigTitleList;//存放多个单文档

	NConfigTitleList* m_pConfigTitleListFirst;
	NConfigTitleList* m_pConfigTitleListLast;
	int m_nConfigTitleListCount;

	void ClearMultiConfigMem()//清空，相当于没有 count也归0
	{
		while (m_pConfigTitleListFirst != NULL)
		{
			m_pConfigTitleListLast = m_pConfigTitleListFirst->pTitleListNext;
			delete m_pConfigTitleListFirst;
			m_pConfigTitleListFirst = m_pConfigTitleListLast;
		}
		m_nConfigTitleListCount = 0;
	}

	//按顺序，添加每个模块的参数链表
	void AddConfigTitle(CSingleConfigTitleFILE* pSingleConfig, char* pNewConfigName = NULL)
	{
		if (pNewConfigName != NULL)
		{
			//复制一个字符串，将这个字符串拼接在另一个字符串后面
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
	virtual void CreateConfigTitleNameList() = 0;//由子类实现
public:
	virtual void Initialize()//子类初始化中
	{
		CreateConfigTitleName();//总标题创建
		CreateConfigTitleNameList();//分标题创建
		InitConfigParamList();//标题下面的参数创建
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

	// 从文件中加载参数
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
		//判断总标题是否等于pConfigTitle， 字符串比较函数strcmp，判断字符串是否相同
		if (!(strcmp(m_pConfigTitleName, pConfigTitle) == 0))
		{
			printf("Can not match Title name %s!!!\n", pConfigTitle);
			fclose(fp);
			return false;
		}
		if (!m_nConfigParamList.LoadParamListFromFile(fp))//本身这个类保存的标题和参数（成员变量用子类的）
		{
			fclose(fp);
			return false;
		}
		int i = 0;
		while (i < m_nConfigTitleListCount)//从链表中取
		{
			if (fscanf(fp, "\n%s\n", pConfigTitle) <= 0) //文件中读取一行
			{
				fclose(fp);
				return false;
			}
			CSingleConfigTitleFILE* pSingleConfig = NULL;
			NConfigTitleList* pTmpTitleList = m_pConfigTitleListFirst;
			while (pTmpTitleList != NULL)
			{
				if ((strcmp(pTmpTitleList->pSingleConfigTitleFILE->m_pConfigTitleName, pConfigTitle) == 0))//对比文件中读取的和链表中的，有的话就将链表中的项取出来
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
		fprintf(fp, "\n%s\n", m_pConfigTitleName);//本身这个类保存的标题和参数（成员变量用子类的）
		if (!m_nConfigParamList.SaveParamListToFile(fp))
		{
			fclose(fp);
			return false;
		}
		NConfigTitleList* pTmpTitleList = m_pConfigTitleListFirst;//文件标题链表取数据从第一个取
		while (pTmpTitleList != NULL)
		{
			fprintf(fp, "\n%s\n", pTmpTitleList->pSingleConfigTitleFILE->m_pConfigTitleName);
			if (!pTmpTitleList->pSingleConfigTitleFILE->m_nConfigParamList.SaveParamListToFile(fp))
			{
				fclose(fp);
				return false;
			}
			pTmpTitleList = pTmpTitleList->pTitleListNext;//取下一个
		}
		fclose(fp);
		return true;
	}
};
#endif