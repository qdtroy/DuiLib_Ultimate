#pragma once

class CCPUID
{
private:
	DWORD m_eax;
	DWORD m_ebx;
	DWORD m_ecx;
	DWORD m_edx;

	char m_cVID[13];	//制造商信息
	char m_cBrand[49];	//商标
public:
	void Executecpuid(DWORD veax)
	{
		__asm
		{
			mov esi, this
			mov eax, veax

			cpuid
			
			mov [esi],eax
			mov [esi+4],ebx
			mov [esi+8],ecx
			mov [esi+12],edx
		}
	}

	//获得CPU的制造商信息(Vender ID)
	const char* GetVID()
	{
	
		memset(m_cVID, 0, 13); 
		Executecpuid(0);

		memcpy(m_cVID,   &m_ebx, 4); 
		memcpy(m_cVID+4, &m_edx, 4); 
		memcpy(m_cVID+8, &m_ecx, 4); 

		return m_cVID; 
	}

	//获得CPU商标信息(Brand)
	const char* GetBrand()
	{
		const DWORD BRANDID = 0x80000002; // 从0x80000002开始，到0x80000004结束

		memset(m_cBrand, 0, 49);
		for(DWORD i = 0; i < 3; i++)
		{
			Executecpuid(BRANDID + i);
			memcpy(m_cBrand + i*16, &m_eax, 16); 
		}
		
		return m_cBrand;
	}

	//判断是否支持hyper-threading
	int IsHyperThreading() 
	{
		Executecpuid(1);
		return (m_edx&(1 << 28)) != 0 ? 1 : 0; 
	}

	// 判断是否支持speed step
	int IsEST() 
	{
		Executecpuid(1);
		return (m_ecx&(1 << 7)) != 0 ? 1 : 0; 
	}

	//判断是否支持MMX
	int IsMMX()
	{
		Executecpuid(1);
		return (m_edx & (1 << 23)) != 0 ? 1 : 0; 
	}

	//判断是否支持SSE
	int IsSSE()  
	{ 
		Executecpuid(1);
		return (m_edx & (1 << 25)) != 0 ? 1 : 0;  
	
	}

	//判断是否支持SSE2   
	int IsSSE2()   
	{
		Executecpuid(1);  
		return (m_edx & (1 << 26)) != 0 ? 1 : 0;  
	}

	//判断是否支持SSE3    
	int IsSSE3()   
	{
		Executecpuid(1);
		return (m_ecx) != 0 ? 1 : 0;
	}

	//判断是否支持SSE4.1  
	int IsSSE4_1()    
	{ 
		Executecpuid(1);
		return (m_ecx&(1 << 19)) != 0 ? 1 : 0; 
	}

	//判断是否支持SSE4.2 
	int IsSSE4_2()  
	{ 
		Executecpuid(1); 
		return (m_ecx&(1 << 20)) != 0 ? 1 : 0;  
	}

};
