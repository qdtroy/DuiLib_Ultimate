#pragma once

namespace Graph
{
	typedef float    real;

	const real dConst_MinPrecision=1.0e-3f;
	const real DOUBLE_MAXVALUE=1.0e15f;
	const real DOUBLE_MINVALUE=-1.0e15f;

	const real dConst_Precision_I=0.1f;
	const real dConst_Precision_II=0.01f;
	const real dConst_Precision_III=1.0e-3f;
	const real dConst_Precision_IV=1.0e-4f;
	const real dConst_Precision_V=1.0e-5f;
	const real dConst_Precision_VI=1.0e-6f;

	//rect 
	const real dConst_MathRect_MinHeight=1.0f;
	const real dConst_MathRect_MinWidth=1.0f;
	const real dConst_Rect_MaxLen=40000.0f;
	const real dConst_Rect_MinLen=0.01f;

	// 线段相互关系
	#define INTERSECTLINE_NORMAL 0
	#define INTERSECTLINE_NONE   1
	#define INTERSECTLINE_COVER  2
	#define INTERSECTLINE_PARALL 3

	enum MatrixOrder
	{
		MatrixOrderPrepend = 0,
		MatrixOrderAppend = 1
	};
}