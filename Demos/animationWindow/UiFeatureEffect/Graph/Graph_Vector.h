#pragma once

#include <emmintrin.h>

namespace Graph
{
	class Vector 
	{
	public:
		// w == 1 点，  有平移
		// w == 0 向量，没有平移
		__declspec(align(16)) real m_vector[4];

		// 构造函数

		inline Vector::Vector()
		{
			m_vector[0]=0.0f;
			m_vector[1]=0.0f;
			m_vector[2]=0.0f;
			m_vector[3]=1.0f;
		}
		inline Vector::Vector(const real* v)	
		{
			m_vector[0]=v[0];
			m_vector[1]=v[1];
			m_vector[2]=v[2];
			m_vector[3]=v[3];
		}

		inline Vector::Vector(real _x, real _y, real _z, real _w)
		{
			m_vector[0]=_x;
			m_vector[1]=_y;
			m_vector[2]=_z;
			m_vector[3]=_w;
		}

		// 设置向量
		inline void Vector::SetVector(const real* v)
		{
			m_vector[0]=v[0];
			m_vector[1]=v[1];
			m_vector[2]=v[2];
			m_vector[3]=v[3];
		}

		inline void Vector::SetVector(real _x, real _y, real _z, real _w)
		{
			m_vector[0]=_x;
			m_vector[1]=_y;
			m_vector[2]=_z;
			m_vector[3]=_w;
		}

		// 减法
		inline void Vector::Difference(const Vector* pSrc, const Vector* pDest)
		{
			m_vector[0]=pDest->m_vector[0] - pSrc->m_vector[0];
			m_vector[1]=pDest->m_vector[1] - pSrc->m_vector[1];
			m_vector[2]=pDest->m_vector[2] - pSrc->m_vector[2];
			m_vector[3]=1.0f;			
		}

		// 反向量
		inline void Vector::Inverse()
		{
			m_vector[0]=-m_vector[0];
			m_vector[1]=-m_vector[1];
			m_vector[2]=-m_vector[2];
			m_vector[3]=1.0f;
		}

		// 是否单位向量
		inline bool Vector::IsNormalized()
		{
			return D_Equal(m_vector[0]*m_vector[0]+m_vector[1]*m_vector[1]+m_vector[2]*m_vector[2], 1.0f);
		}

		// 运算符重载
		inline void Vector::operator += (Vector& v)
		{
			m_vector[0]+=v.m_vector[0];
			m_vector[1]+=v.m_vector[1];
			m_vector[2]+=v.m_vector[2];
			m_vector[3]=1.0f;
		}
	
		inline void Vector::operator -= (Vector& v)
		{
			m_vector[0]-=v.m_vector[0];
			m_vector[1]-=v.m_vector[1];
			m_vector[2]-=v.m_vector[2];
			m_vector[3]=1.0f;
		}

		inline void Vector::operator *= (real f)
		{
			m_vector[0]*= f;
			m_vector[1]*= f;
			m_vector[2]*= f;
			m_vector[3]=1.0f;
		}
	
		inline void Vector::operator /= (real f)
		{
			f = 1.0f/f;
			m_vector[0]*= f;
			m_vector[1]*= f;
			m_vector[2]*= f;
			m_vector[3]=1.0f;
		}
	
		inline const Vector Vector::operator = (const Vector& v)
		{
			if(&v != this)
			{
				memcpy(m_vector,v.m_vector,sizeof(m_vector));
			}
		
			return *this;
		}

		inline Vector Vector::operator + (Vector& v) const
		{
			return Vector(m_vector[0]+v.m_vector[0], m_vector[1]+v.m_vector[1], m_vector[2]+v.m_vector[2], 1.0f);
		}
	
		inline Vector Vector::operator - (Vector& v) const
		{
			return Vector(m_vector[0]-v.m_vector[0], m_vector[1]-v.m_vector[1], m_vector[2]-v.m_vector[2], 1.0f);
		}
		
		inline real Vector::operator * (Vector& v) const
		{
			return m_vector[0]*v.m_vector[0] + m_vector[1]*v.m_vector[1] + m_vector[2]*v.m_vector[2];
		}
		
		inline Vector Vector::operator * (real f) const
		{
			return Vector(m_vector[0]*f, m_vector[1]*f, m_vector[2]*f, 1.0f);
		}
		
		inline bool Vector::operator ==(Vector& v)
		{
			return ((((m_vector[0]-v.m_vector[0])<dConst_MinPrecision && (m_vector[0]-v.m_vector[0])>-dConst_MinPrecision) || 
				((m_vector[1]-v.m_vector[1])<dConst_MinPrecision && (m_vector[1]-v.m_vector[1])>-dConst_MinPrecision) || 
				((m_vector[2]-v.m_vector[2])<dConst_MinPrecision && (m_vector[2]-v.m_vector[2])>-dConst_MinPrecision))? false:true);
		}

		// 向量长度的平方
		real Vector::GetLengthSq() 
		{
			return m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1] + m_vector[2]*m_vector[2];
		}

		// 向量长度
		float Vector::GetLength()
		{
			float f;

			_asm
			{
				lea ecx, f;
				mov eax, this;
				mov dword ptr [eax+12], 0;

				movups xmm0, [eax];
				mulps xmm0, xmm0;
				movaps xmm1, xmm0;
				shufps xmm1, xmm1, 4Eh; 
				addps xmm0, xmm1;
				movaps xmm1, xmm0;
				shufps xmm1, xmm1, 11h; 
				addss xmm0, xmm1;

				sqrtss xmm0, xmm0; 
				movss dword ptr [ecx], xmm0; 

				mov dword ptr [eax+12], 3F800000h; // 3F800000h == 1.0f
			}

		}
		// 单位化向量
		void Vector::Normalize()
		{
			_asm
			{
				mov eax, this;
				mov dword ptr[eax+12], 0;

				movups xmm0, [eax];
				movaps xmm2, xmm0;
				mulps xmm0, xmm0;
				movaps xmm1, xmm0;
				shufps xmm1, xmm1, 4Eh;
				addps xmm0, xmm1;
				movaps xmm1, xmm0;
				shufps xmm1, xmm1, 11h;
				addps xmm0, xmm1;

				rsqrtps xmm0, xmm0;
				mulps xmm2, xmm0;
				movups [eax], xmm2;

				mov dword ptr [eax+12], 3F800000h;
			}

		}

		// 通过两向量求叉乘,结果保存在该向量中
		void Vector::Cross(const Vector* pU, const Vector* pV)
		{
			_asm
			{
				mov eax, pU;
				mov edx, pV;

				movups xmm0, [eax]
				movups xmm1, [edx]
				movaps xmm2, xmm0
				movaps xmm3, xmm1

				shufps xmm0, xmm0, 0xc9
				shufps xmm1, xmm1, 0xd2
				mulps xmm0, xmm1

				shufps xmm2, xmm2, 0xd2
				shufps xmm3, xmm3, 0xc9
				mulps xmm2, xmm3

				subps xmm0, xmm2

				mov eax, this
				movups [eax], xmm0

				mov [eax+12], 3F800000h;
			}

		}

		// 求两向量夹角
		real Vector::AngleWith(Vector& v)
		{
			return (real)acosf((*this * v)/(this->GetLength()*v.GetLength()*2.0f));
		}

	};

}