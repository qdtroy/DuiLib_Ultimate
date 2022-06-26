#pragma once

#include "Graph_Matrix.h"
namespace Graph
{

	class CView : public CMatrix
	{
	public:

		CView(){};
		~CView(){};

	public:
		// 初始化观察矩阵(左手坐标系)
		void InitViewMatrix(Vector &eye, Vector &lookAt, Vector &up)
		{						
			//right vector
			Vector Z = lookAt - eye;
			Vector vecTemp;
			vecTemp.Cross(&up,&Z);
			vecTemp.Normalize();
			m_matrix[0][0] = vecTemp.m_vector[0];
			m_matrix[1][0] = vecTemp.m_vector[1];
			m_matrix[2][0] = vecTemp.m_vector[2];
			
			//up vector
			vecTemp.Cross(&Z,&vecTemp);
			vecTemp.Normalize();
			m_matrix[0][1] = vecTemp.m_vector[0];
			m_matrix[1][1] = vecTemp.m_vector[1];
			m_matrix[2][1] = vecTemp.m_vector[2];

			Z.Normalize();
			m_matrix[0][2] = Z.m_vector[0];
			m_matrix[1][2] = Z.m_vector[1];
			m_matrix[2][2] = Z.m_vector[2];

			vecTemp.SetVector(m_matrix[0][0],m_matrix[1][0],m_matrix[2][0],1.0f);
			m_matrix[3][0] = -(eye * vecTemp);
			vecTemp.SetVector(m_matrix[0][1],m_matrix[1][1],m_matrix[2][1],1.0f);
			m_matrix[3][1] = -(eye * vecTemp);
			vecTemp.SetVector(m_matrix[0][2],m_matrix[1][2],m_matrix[2][2],1.0f);
			m_matrix[3][2] = -(eye *vecTemp);
			m_matrix[3][3] = 1.0f;
				
		}
	};
}