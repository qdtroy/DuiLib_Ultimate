#pragma once

/// <summary>
/// Structure to define HSL.
/// </summary>
struct HSL
{
#pragma region con/destructor
	HSL()
	{
		hue = saturation = luminance = 0;
	}

	~HSL()
	{

	}

	HSL(int h, int s, int l)
	{
		hue = (h>360)? 360 : ((h<0)?0:h); 
		saturation = (s>255)? 255 : ((s<0)?0:s);
		luminance =  (l>255)? 255 : ((l<0)?0:l);
	}
#pragma endregion
#pragma region Operators
	bool operator ==(HSL &item)
	{
		return (
			this->hue == item.hue 
			&& this->saturation == item.saturation 
			&& this->luminance == item.luminance
			);
	}

	bool operator !=(HSL &item)
	{
		return (
			this->hue != item.hue 
			|| this->saturation != item.saturation 
			|| this->luminance != item.luminance
			);
	}


	const HSL operator =(HSL &item)
	{
		if (*this == item)
		{
			return *this;
		}
		this->hue = item.hue;
		this->saturation = item.saturation;
		this->luminance = item.luminance;

		return *this;
	}
#pragma endregion

#pragma region Fields
	 int hue;				//0~360
	 int saturation;		//0~255
	 int luminance;			//0~255
#pragma endregion
};

class CColorHelper
{
public:
	//R: ( 0 to 255 )    
	//G: ( 0 to 255 )
	//B: ( 0 to 255 )
	static HSL RGBToHSL(int red, int green, int blue);
	static HSL RGBToHSL(const Color &rgb);
	//H: hue ( 0 to 360 )    
	//S: haturation ( 0 to 255 )   
	//L: luminance ( 0 to 255 ) 
	static Gdiplus::Color HSLToRGB(int h, int s, int l);
	static Gdiplus::Color HSLToRGB(const HSL &hsl);

	//hue -180~180
	//saturation -100~100
	//luminance  -100~100

	// hue sat adjust
	static void SetHueSaturation(BitmapData* data, long hValue, long sValue);
	static void SetHueSaturation(BYTE* data,int width,int height,long hValue,long sValue);
	// light adjust
	static void SetBrightness(BitmapData* data, long value); 
	static void SetBrightness(BYTE* data,int width,int height,long value);
	
};