#pragma once

#include "Graph_Matrix.h"
#include <math.h>
namespace Graph
{
	class CProjection : public CMatrix
	{
	public:

		CProjection()
		{
			m_eye = Vector(0.0f,0.0f,1000.0f,1.0f);
			m_lookAt = Vector(0.0f,0.0f,0.0f,1.0f);
		};

		~CProjection(){};

		void SetEyePos(const real* eye)
		{
			m_eye = Vector(eye);
		}

		const Vector& GetEyePos()
		{
			return m_eye;
		}

		void SetLookAt(const real* lookAt)
		{
			m_lookAt = Vector(lookAt);
		}

		const Vector& GetLookAt()
		{
			return m_lookAt;
		}

	public:
		// 投影平面中心为xy中心（x,y方向都居中）
		void InitProMatrix(CdRect &rcPlane, real nearPlane, real farPlane)
		{
			if (IsIdentity())
			{
				real delta = farPlane - nearPlane;
				assert(delta != 0);

				real a = farPlane/delta;
				real b = -nearPlane * a;

				m_matrix[0][0] = 2*nearPlane/rcPlane.Width();
				m_matrix[1][1] = 2*nearPlane/rcPlane.Height();
				m_matrix[2][2] = a;
				m_matrix[3][2] = b;
				m_matrix[2][3] = 1;
			} 
			else
			{
				CProjection Proj;
				Proj.InitProMatrix(rcPlane,nearPlane,farPlane);

				MultMatrix(this,this,&Proj);
			}
			

		}

		void InitProMatrix(real radius,real aspect, real nearPlane, real farPlane)
		{
			assert (IsIdentity());

			real delta = farPlane - nearPlane;
			assert(delta != 0);

			real a = farPlane/delta;
			real b = -nearPlane * a;

			m_matrix[0][0] = 1.0f/tan(radius*aspect/2);
			m_matrix[1][1] = 1.0f/tan(radius/2);
			m_matrix[2][2] = a;
			m_matrix[3][2] = b;
			m_matrix[2][3] = 1;
		}

		// 投影平面中心不为xy中心（x,y方向都不居中）
		void InitProMatrixOffCenter(CdRect &rcPlane, real nearPlane, real farPlane)
		{
			if (IsIdentity())
			{
				InitProMatrix(rcPlane, nearPlane,farPlane);

				m_matrix[2][0] = (rcPlane.left + rcPlane.right)/rcPlane.Width();
				m_matrix[2][1] = (rcPlane.top + rcPlane.bottom)/rcPlane.Height();
			}
			else
			{
				CProjection Proj;
				Proj.InitProMatrixOffCenter(rcPlane,nearPlane,farPlane);

				MultMatrix(this,this,&Proj);
			}

		}

		// 投影vec到平片nearPlane,
		BOOL ProjectPoint(Vector &vec,real nearPlane = 0)
		{
			real innerRes = 0.0f;
			MapVector(vec);
			// 投影计算
			//innerRes = (nearPlane - eye.m_vector[2])/(vec.m_vector[2]-eye.m_vector[2]);
			//vec.m_vector[1] = -(innerRes*(vec.m_vector[1]-eye.m_vector[1]) + eye.m_vector[1]);
			//vec.m_vector[0] = innerRes*(vec.m_vector[0]-eye.m_vector[0]) + eye.m_vector[0];

			innerRes = m_eye.m_vector[2]/(m_eye.m_vector[2]-m_eye.m_vector[2]);
			vec.m_vector[1] = -innerRes*vec.m_vector[1];
			vec.m_vector[0] = innerRes*vec.m_vector[0];
		}

		// rcSrc 投影到wRc
		// rcSrc 与wRc均是数据的下标范围而不是bouderRect
		BOOL ProjectRect(CdRect &rcSrc,WarpRect &wRc,CdPoint &ptCenter)
		{
			// 移到xy平面中心
			CdRect rc(rcSrc);
		//	ptCenter.Offset(-rcSrc.left,-rcSrc.top);
			CdSize offSet(ptCenter.x,ptCenter.y);
			rc.OffsetRect(-offSet);

			Vector vec[4];
			vec[0].SetVector(rc.left,rc.top,0,1.0f);
			vec[1].SetVector(rc.right,rc.top,0,1.0f);
			vec[2].SetVector(rc.left,rc.bottom,0,1.0f);
			vec[3].SetVector(rc.right,rc.bottom,0,1.0f);

			CdPoint ptRes[4];
			real innerRes = 0.0f;
			for(int i = 0; i < 4; i++)
			{
				MapVector(vec[i]);
				innerRes = (0 - m_eye.m_vector[2])/(vec[i].m_vector[2]-m_eye.m_vector[2]);
				ptRes[i].y = innerRes*(vec[i].m_vector[1]-m_eye.m_vector[1]) + m_eye.m_vector[1];
				ptRes[i].x = innerRes*(vec[i].m_vector[0]-m_eye.m_vector[0]) + m_eye.m_vector[0];
			}
			wRc.topLeft = ptRes[0];
			wRc.topRight = ptRes[1];
			wRc.bottomLeft = ptRes[2];
			wRc.bottomRight = ptRes[3];
			wRc.Offset(offSet);
			wRc.topLeft = wRc.topLeft.ToPOINT();
			wRc.topRight = wRc.topRight.ToPOINT();
			wRc.bottomLeft = wRc.bottomLeft.ToPOINT();
			wRc.bottomRight = wRc.bottomRight.ToPOINT();

			return TRUE;
		}
		// 用rcSrc构造原点cube,通过旋转得到投影的面,index 为要获得的面的序号
		// rcSrc 与wRc均是数据的下标范围而不是bouderRect
		// cube plane index  
		// front 	0 
		// right 	1 
		// left  	2 
		// back  	3 
		// up    	4 
		// bottom	5
		BOOL CubeProjectRect(CdRect &rcSrc,WarpRect &wRc,int index,real nearPlane = 0)
		{
			assert(rcSrc.TopLeft() == CdPoint(0,0));
		
			// 移到xy平面中心
			real ptTemp1 = rcSrc.Width()/2;
			real ptTemp2 = rcSrc.Height()/2;

			Vector eye(0.0f,0.0f,10*nearPlane,1.0f);

			SetEyePos(eye.m_vector);

			// cube plane index  
			// front 	0 
			// right 	1 
			// left  	2 
			// back  	3 
			// up    	4 
			// bottom	5
			Vector vec[8];
			vec[0].SetVector(-ptTemp1,ptTemp2,nearPlane,1.0f);	//front left top point
			vec[1].SetVector(ptTemp1,ptTemp2,nearPlane,1.0f);		//front right top point
			vec[2].SetVector(ptTemp1,-ptTemp2,nearPlane,1.0f);	//front right bottom point
			vec[3].SetVector(-ptTemp1,-ptTemp2,nearPlane,1.0f);	//front left bottom point
			vec[4].SetVector(-ptTemp1,ptTemp2,-nearPlane,1.0f);	//back left top point
			vec[5].SetVector(ptTemp1,ptTemp2,-nearPlane,1.0f);	//back right top point
			vec[6].SetVector(ptTemp1,-ptTemp2,-nearPlane,1.0f);	//back right bottom point
			vec[7].SetVector(-ptTemp1,-ptTemp2,-nearPlane,1.0f);	//back left bottom point

			CdPoint ptRes[8];
			real innerRes = 0.0f;
			for(int i = 0; i < 8; i++)
			{
				MapVector(vec[i]);
				innerRes = (nearPlane - m_eye.m_vector[2])/(vec[i].m_vector[2]-m_eye.m_vector[2]);
				ptRes[i].y = -(innerRes*(vec[i].m_vector[1]-m_eye.m_vector[1]) + m_eye.m_vector[1]);
				ptRes[i].x = innerRes*(vec[i].m_vector[0]-m_eye.m_vector[0]) + m_eye.m_vector[0];
			}

			switch(index)
			{
			case 0:
				wRc.topLeft = ptRes[0];
				wRc.topRight = ptRes[1];
				wRc.bottomRight= ptRes[2];
				wRc.bottomLeft  = ptRes[3];
				break;
			case 1:
				wRc.topLeft = ptRes[1];
				wRc.topRight = ptRes[5];
				wRc.bottomRight = ptRes[6];
				wRc.bottomLeft = ptRes[2];
				break;
			case 2:
				wRc.topLeft = ptRes[4];
				wRc.topRight = ptRes[0];
				wRc.bottomRight = ptRes[3];
				wRc.bottomLeft = ptRes[7];
				break;
			case 3:
				wRc.topLeft = ptRes[5];
				wRc.topRight = ptRes[4];
				wRc.bottomRight = ptRes[7];
				wRc.bottomLeft = ptRes[6];
				break;
			case 4:
				wRc.topLeft = ptRes[4];
				wRc.topRight = ptRes[5];
				wRc.bottomRight = ptRes[1];
				wRc.bottomLeft = ptRes[0];
				break;
			case 5:
				wRc.topLeft = ptRes[3];
				wRc.topRight = ptRes[2];
				wRc.bottomRight = ptRes[6];
				wRc.bottomLeft = ptRes[7];
				break;
			default:
				break;
			}
			wRc.Offset(CdSize(ptTemp1,ptTemp2));
			wRc.topLeft = wRc.topLeft.ToPOINT();
			wRc.topRight = wRc.topRight.ToPOINT();
			wRc.bottomLeft = wRc.bottomLeft.ToPOINT();
			wRc.bottomRight = wRc.bottomRight.ToPOINT();

			return TRUE;
		}

		// 用rcSrc构造原点cube,通过旋转得到投影的面,index 为要获得的面的序号
		// rcSrc 与wRc均是数据的下标范围而不是bouderRect
		// cube plane index  
		// front 	0 
		// right 	1 
		// left  	2 
		// back  	3 
		// up    	4 
		// bottom	5
		BOOL CubeInsideProjectRect(CdRect &rcSrc,WarpRect &wRc,int index,real nearPlane = 0)
		{
			assert(rcSrc.TopLeft() == CdPoint(0,0));

			// 移到xy平面中心
			real ptTemp1 = rcSrc.Width()/2;
			real ptTemp2 = rcSrc.Height()/2;

			Vector eye(0.0f,0.0f,-10*nearPlane,1.0f);

			SetEyePos(eye.m_vector);

			// cube plane index  
			// front 	0 
			// right 	1 
			// left  	2 
			// back  	3 
			// up    	4 
			// bottom	5
			Vector vec[8];
			vec[0].SetVector(-ptTemp1,ptTemp2,-nearPlane,1.0f);	//front left top point
			vec[1].SetVector(ptTemp1,ptTemp2,-nearPlane,1.0f);		//front right top point
			vec[2].SetVector(ptTemp1,-ptTemp2,-nearPlane,1.0f);	//front right bottom point
			vec[3].SetVector(-ptTemp1,-ptTemp2,-nearPlane,1.0f);	//front left bottom point
			vec[4].SetVector(-ptTemp1,ptTemp2,nearPlane,1.0f);	//back left top point
			vec[5].SetVector(ptTemp1,ptTemp2,nearPlane,1.0f);	//back right top point
			vec[6].SetVector(ptTemp1,-ptTemp2,nearPlane,1.0f);	//back right bottom point
			vec[7].SetVector(-ptTemp1,-ptTemp2,nearPlane,1.0f);	//back left bottom point

			CdPoint ptRes[8];
			real innerRes = 0.0f;
			for(int i = 0; i < 8; i++)
			{
				MapVector(vec[i]);
				innerRes = (nearPlane - m_eye.m_vector[2])/(vec[i].m_vector[2]-m_eye.m_vector[2]);
				ptRes[i].y = -(innerRes*(vec[i].m_vector[1]-m_eye.m_vector[1]) + m_eye.m_vector[1]);
				ptRes[i].x = innerRes*(vec[i].m_vector[0]-m_eye.m_vector[0]) + m_eye.m_vector[0];
			}

			switch(index)
			{
			case 0:
				wRc.topLeft = ptRes[1];
				wRc.topRight = ptRes[0];
				wRc.bottomRight= ptRes[3];
				wRc.bottomLeft  = ptRes[2];
				break;
			case 1:
				wRc.topLeft = ptRes[5];
				wRc.topRight = ptRes[1];
				wRc.bottomRight = ptRes[2];
				wRc.bottomLeft = ptRes[6];
				break;
			case 2:
				wRc.topLeft = ptRes[0];
				wRc.topRight = ptRes[4];
				wRc.bottomRight = ptRes[7];
				wRc.bottomLeft = ptRes[3];
				break;
			case 3:
				wRc.topLeft = ptRes[4];
				wRc.topRight = ptRes[5];
				wRc.bottomRight = ptRes[6];
				wRc.bottomLeft = ptRes[7];
				break;
			case 4:
				wRc.topLeft = ptRes[0];
				wRc.topRight = ptRes[1];
				wRc.bottomRight = ptRes[5];
				wRc.bottomLeft = ptRes[4];
				break;
			case 5:
				wRc.topLeft = ptRes[7];
				wRc.topRight = ptRes[6];
				wRc.bottomRight = ptRes[2];
				wRc.bottomLeft = ptRes[3];
				break;
			default:
				break;
			}
			wRc.Offset(CdSize(ptTemp1,ptTemp2));
			wRc.topLeft = wRc.topLeft.ToPOINT();
			wRc.topRight = wRc.topRight.ToPOINT();
			wRc.bottomLeft = wRc.bottomLeft.ToPOINT();
			wRc.bottomRight = wRc.bottomRight.ToPOINT();

			return TRUE;
		}

		// 用rcSrc构造原点cube,通过旋转得到投影的面,index 为要获得的面的序号
		// rcSrc 与wRc均是数据的下标范围而不是bouderRect
		// prism plane index  
		// front 		0 
		// right_back 	1 
		// left_back 	2 
		BOOL HorzPrismProjectRect(CdRect &rcSrc,WarpRect &wRc,int index)
		{
			assert(rcSrc.TopLeft() == CdPoint(0,0));

			// 移到xy平面中心
			real ptTemp1 = rcSrc.Width()/2;
			real ptTemp2 = rcSrc.Height()/2;
			real nearPlane = ptTemp1*tanf(M_PI/6);
			real ptTemp = ptTemp1*sqrt(3.0f) - nearPlane;
			
			Vector eye(0.0f,0.0f,10*nearPlane,1.0f);
			SetEyePos(eye.m_vector);

			Vector vec[6];
			vec[0].SetVector(-ptTemp1,ptTemp2,nearPlane,1.0f);	//front left top point
			vec[1].SetVector(ptTemp1,ptTemp2,nearPlane,1.0f);	//front right top point
			vec[2].SetVector(ptTemp1,-ptTemp2,nearPlane,1.0f);	//front right bottom point
			vec[3].SetVector(-ptTemp1,-ptTemp2,nearPlane,1.0f);	//front left bottom point
			vec[4].SetVector(0,ptTemp2,-ptTemp,1.0f);			//back  top point
			vec[5].SetVector(0,-ptTemp2,-ptTemp,1.0f);			//back  bottom point

			CdPoint ptRes[6];
			real innerRes = 0.0f;
			for(int i = 0; i < 6; i++)
			{
				MapVector(vec[i]);
				innerRes = (nearPlane - m_eye.m_vector[2])/(vec[i].m_vector[2]-m_eye.m_vector[2]);
				ptRes[i].y = -(innerRes*(vec[i].m_vector[1]-m_eye.m_vector[1]) + m_eye.m_vector[1]);
				ptRes[i].x = innerRes*(vec[i].m_vector[0]-m_eye.m_vector[0]) + m_eye.m_vector[0];
			}

			switch(index)
			{
			case 0:
				wRc.topLeft = ptRes[0];
				wRc.topRight = ptRes[1];
				wRc.bottomRight= ptRes[2];
				wRc.bottomLeft  = ptRes[3];
				break;
			case 1:
				wRc.topLeft = ptRes[1];
				wRc.topRight = ptRes[4];
				wRc.bottomRight = ptRes[5];
				wRc.bottomLeft = ptRes[2];
				break;
			case 2:
				wRc.topLeft = ptRes[4];
				wRc.topRight = ptRes[0];
				wRc.bottomRight = ptRes[3];
				wRc.bottomLeft = ptRes[5];
				break;
			default:
				break;
			}

			wRc.Offset(CdSize(ptTemp1,ptTemp2));
			wRc.topLeft = wRc.topLeft.ToPOINT();
			wRc.topRight = wRc.topRight.ToPOINT();
			wRc.bottomLeft = wRc.bottomLeft.ToPOINT();
			wRc.bottomRight = wRc.bottomRight.ToPOINT();

			return TRUE;
		}

		// 用rcSrc构造原点cube,通过旋转得到投影的面,index 为要获得的面的序号
		// rcSrc 与wRc均是数据的下标范围而不是bouderRect
		// prism plane index  
		// front 		0 
		// top_back 	1 
		// bottom_back 	2 
		BOOL VeriPrismProjectRect(CdRect &rcSrc,WarpRect &wRc,int index)
		{
			assert(rcSrc.TopLeft() == CdPoint(0,0));

			// 移到xy平面中心
			real ptTemp1 = rcSrc.Width()/2;
			real ptTemp2 = rcSrc.Height()/2;
			real nearPlane = ptTemp2*tanf(M_PI/6);
			real ptTemp = ptTemp2*sqrt(3.0f) - nearPlane;


			Vector eye(0.0f,0.0f,10*nearPlane,1.0f);
			SetEyePos(eye.m_vector);

			Vector vec[6];
			vec[0].SetVector(-ptTemp1,ptTemp2,nearPlane,1.0f);			//front left top point
			vec[1].SetVector(ptTemp1,ptTemp2,nearPlane,1.0f);			//front right top point
			vec[2].SetVector(ptTemp1,-ptTemp2,nearPlane,1.0f);			//front right bottom point
			vec[3].SetVector(-ptTemp1,-ptTemp2,nearPlane,1.0f);			//front left bottom point
			vec[4].SetVector(-ptTemp1,0,-ptTemp,1.0f);					//back  left point
			vec[5].SetVector(ptTemp1,0,-ptTemp,1.0f);					//back  right point

			CdPoint ptRes[6];
			real innerRes = 0.0f;
			for(int i = 0; i < 6; i++)
			{
				MapVector(vec[i]);
				innerRes = (nearPlane - m_eye.m_vector[2])/(vec[i].m_vector[2]-m_eye.m_vector[2]);
				ptRes[i].y = -(innerRes*(vec[i].m_vector[1]-m_eye.m_vector[1]) + m_eye.m_vector[1]);
				ptRes[i].x = innerRes*(vec[i].m_vector[0]-m_eye.m_vector[0]) + m_eye.m_vector[0];
			}

			switch(index)
			{
			case 0:
				wRc.topLeft = ptRes[0];
				wRc.topRight = ptRes[1];
				wRc.bottomRight= ptRes[2];
				wRc.bottomLeft  = ptRes[3];
				break;
			case 1:
				wRc.topLeft = ptRes[4];
				wRc.topRight = ptRes[5];
				wRc.bottomRight = ptRes[1];
				wRc.bottomLeft = ptRes[0];
				break;
			case 2:
				wRc.topLeft = ptRes[3];
				wRc.topRight = ptRes[2];
				wRc.bottomRight = ptRes[5];
				wRc.bottomLeft = ptRes[4];
				break;
			default:
				break;
			}

			wRc.Offset(CdSize(ptTemp1,ptTemp2));
			wRc.topLeft = wRc.topLeft.ToPOINT();
			wRc.topRight = wRc.topRight.ToPOINT();
			wRc.bottomLeft = wRc.bottomLeft.ToPOINT();
			wRc.bottomRight = wRc.bottomRight.ToPOINT();

			return TRUE;
		}

	private:
		Vector m_eye;
		Vector m_lookAt;
	};
}