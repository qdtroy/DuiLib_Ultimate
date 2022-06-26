#pragma once

#include "Graph_Vector.h"
#include "Graph_Def.h"

namespace Graph
{
	class CMatrix
	{
	public:
		__declspec(align(16)) real	m_matrix[4][4];

		CMatrix::CMatrix()
		{
			memset(m_matrix,0,16*sizeof(real));
			m_matrix[0][0] = m_matrix[1][1] = m_matrix[2][2] = 1.0f;
		}

		CMatrix::~CMatrix()
		{

		}

		CMatrix::CMatrix(real _M00,real _M01,real _M02,real _M03,
			real _M10,real _M11,real _M12,real _M13,
			real _M20,real _M21,real _M22,real _M23,
			real _M30,real _M31,real _M32,real _M33)
		{
			m_matrix[0][0] = _M00;
			m_matrix[0][1] = _M01;
			m_matrix[0][2] = _M02;
			m_matrix[0][3] = _M03;

			m_matrix[1][0] = _M10;
			m_matrix[1][1] = _M11;
			m_matrix[1][2] = _M12;
			m_matrix[1][3] = _M13;

			m_matrix[2][0] = _M20;
			m_matrix[2][1] = _M21;
			m_matrix[2][2] = _M22;
			m_matrix[2][3] = _M23;

			m_matrix[3][0] = _M30;
			m_matrix[3][1] = _M31;
			m_matrix[3][2] = _M32;
			m_matrix[3][3] = _M33;
		}

		CMatrix::CMatrix(CMatrix &other)
		{
			memcpy(m_matrix,other.m_matrix,16*sizeof(real));
		}

		void operator = (CMatrix &other)
		{
			if(this != &other)
			{
				memcpy(m_matrix,other.m_matrix,16*sizeof(real));
			}
		}

	public:
		void Reset()
		{
			memset(m_matrix,0,16*sizeof(real));
			m_matrix[0][0] = m_matrix[1][1] = m_matrix[2][2] = 1.0f;
		}

		BOOL IsIdentity()
		{
			if (m_matrix[0][0]  == 1.0f &&  m_matrix[1][1]  == 1.0f && m_matrix[2][2]  == 1.0f)
			{
				return m_matrix[0][1] == 0.0f &&  m_matrix[0][2] == 0.0f &&  m_matrix[0][3] == 0.0f &&  m_matrix[1][0] == 
						0.0f &&  m_matrix[1][2] == 0.0f &&  m_matrix[1][3] == 0.0f &&  m_matrix[2][0] == 0.0f &&  m_matrix[2][1] == 
						0.0f &&  m_matrix[2][3] == 0.0f &&  m_matrix[3][0] == 0.0f &&  m_matrix[3][1] == 0.0f &&  m_matrix[3][2] == 0.0f && m_matrix[3][3]== 0.0f;
			} 
			else
			{
				return FALSE;
			}
		}

		void RotateX(real radiusX,MatrixOrder append = MatrixOrderAppend)
		{
	//		assert (IsIdentity());

			m_matrix[1][1] = m_matrix[2][2] = cos(radiusX);
			m_matrix[1][2] = sin(radiusX);
			m_matrix[2][1] = -m_matrix[1][2];
		}

		void RotateY(real radiusY,MatrixOrder append = MatrixOrderAppend)
		{
	//		assert (IsIdentity());
			m_matrix[0][0] = m_matrix[2][2] = cos(radiusY);
			m_matrix[0][2] = -sin(radiusY);
			m_matrix[2][0] = -m_matrix[0][2];
		}

		void RotateZ(real radiusZ,MatrixOrder append = MatrixOrderAppend)
		{
	//		assert (IsIdentity());
	
			m_matrix[0][0] = m_matrix[1][1] = cos(radiusZ);
			m_matrix[0][1] = sin(radiusZ);
			m_matrix[1][0] = -m_matrix[0][1];
		}

		void Translate(real x, real y, real z,MatrixOrder append = MatrixOrderAppend)
		{
	//		assert (IsIdentity());
			m_matrix[3][0] = x;
			m_matrix[3][1] = y;
			m_matrix[3][2] = z;
		}

		void Scale(real x, real y, real z,Vector& ptCenter,MatrixOrder append = MatrixOrderAppend)
		{
	//		assert (IsIdentity());
	
			m_matrix[0][0] *= x;
			m_matrix[1][1] *= y;
			m_matrix[2][2] *= z;

			m_matrix[0][3] = (1.0f - x)*ptCenter.m_vector[0];
			m_matrix[1][3] = (1.0f - y)*ptCenter.m_vector[1];
			m_matrix[2][3] = (1.0f - z)*ptCenter.m_vector[2];	

		}

		inline void MapVector(Vector &vec)
		{				
			_asm
			{
				mov ecx, vec;
				mov edx, this;

				movss xmm0, [ecx];
				shufps xmm0, xmm0, 0; 

				movss xmm1, [ecx+4];
				mulps xmm0, [edx];
				shufps xmm1, xmm1, 0; 

				movss xmm2, [ecx+8];
				mulps xmm1, [edx+16];
				shufps xmm2, xmm2, 0; 

				movss xmm3, [ecx+12];
				mulps xmm2, [edx+32];
				shufps xmm3, xmm3, 0; 

				addps xmm0, xmm1;
				mulps xmm3, [edx+48];

				addps xmm0, xmm2;
				addps xmm0, xmm3; // xmm0 = result
				movups [ecx], xmm0;
				//mov [ecx+12], 3F800000h;
			}

		}

		inline void MultMatrix(CMatrix* pOut, const CMatrix* pIn1, const CMatrix* pIn2)
		{	
				_asm
				{
					mov edx, pIn2; 
					movups xmm4, [edx]; 
					movups xmm5, [edx+16]; 
					movups xmm6, [edx+32]; 
					movups xmm7, [edx+48]; 

					mov eax, pIn1; 
					mov edx, pOut;

					mov ecx, 4; 

			LOOPIT: 
					movss xmm0, [eax]; xmm0 = pIn1->x
					shufps xmm0, xmm0, 0; 			
					movss xmm1, [eax+4]; xmm1 = pIn1->y				
					mulps xmm0, xmm4;

					movss xmm2, [eax+8]; xmm2 = pIn1->z
					shufps xmm1, xmm1, 0;		
					mulps xmm1, xmm5;

					movss xmm3, [eax+12]; xmm3 = pIn1->w
					shufps xmm2, xmm2, 0; 
					mulps xmm2, xmm6;

					shufps xmm3, xmm3, 0; 
					addps xmm0, xmm1;
					mulps xmm3, xmm7;

					addps xmm0, xmm2;
					add eax, 16; 
					addps xmm0, xmm3;

					movups [edx], xmm0;
					add edx, 16;
					
					loop LOOPIT;
				}
			
		}
	};

}