#include "StdAfx.h"
#include "ColorHelper.h"

 HSL CColorHelper::RGBToHSL(int red, int green, int blue)
 {
	 Color clr(red,green,blue);
	 return RGBToHSL(clr);
 }

 HSL CColorHelper::RGBToHSL(const Color &rgb)
 {
	 int h=0, s=0, l=0;
	 DWORD colorValue = rgb.GetValue();
	 int max_min, mid_min,hueTemp,maxaddmin;
	 __asm
	 {
#pragma region compute hsl
			 lea		esi, [colorValue]
			 movzx		eax, [esi]			//b
			 movzx		ebx, [esi+1]		//g
			 movzx		ecx, [esi+2]		//r

			 cmp		eax, ebx
			 jg			bbigg
			 mov		edx, ebx	
			 mov		esi, eax
			 jmp		gbigb
bbigg:	
			 mov		edx, eax
			 mov		esi, ebx
gbigb:
			 cmp		edx, ecx
			 jg			endMax				//edx = max
			 mov		edi, esi
			 mov		esi, edx			
			 mov		edx, ecx			
			 jmp		sortDone			//esi = mid
endMax:										//edx = max
			 cmp		esi, ecx
			 jg			endMid				//esi = mid
			 mov		edi, esi			
			 mov		esi, ecx
			 jmp		sortDone
endMid:										//edi = min
			 mov		edi, ecx
sortDone:

			 push		eax
			 push		edx

			 mov		eax, edx
			 sub		eax, edi
			 mov		max_min, eax
			 sub		esi, edi
			 mov		mid_min, esi
			 add		edi, edx
			 mov	    maxaddmin, edi

			 cmp		max_min, 0
			 je			noHueTemp
			 mov		eax, esi
			 imul		eax, 60	
			 cdq		
			 idiv		max_min
			 mov		hueTemp, eax
noHueTemp:
			 pop		edx
			 pop		eax

			 // h
			 cmp		max_min, 0
			 je			hEnd
			 cmp		ecx, edx
			 je			maxRed
			 cmp		ebx, edx
			 je			maxGreen
			 jmp		maxBlue
maxRed:
			 cmp		ebx, eax		
			 jl			rbg
//rgb:
			 mov		ecx, hueTemp
			 mov		h, ecx
			 jmp		hEnd
rbg:
			 mov		ecx, 360
			 sub		ecx, hueTemp
			 mov		h, ecx
			 jmp		hEnd
maxGreen:
			 cmp		eax, ecx
			 jg			gbr
			 //grb:
			 mov		ebx, 120
			 sub		ebx, hueTemp
			 mov		h,   ebx
			 jmp		hEnd
gbr:
			 mov		ebx, hueTemp
			 add		ebx, 120
			 mov		h,   ebx
			 jmp		hEnd
maxBlue:
			 cmp		ecx, ebx
			 jg			brg
//bgr:
			 mov		eax, 240
			 sub		eax, hueTemp
			 mov		h,   eax
			 jmp		hEnd
brg:
			 mov		eax, hueTemp
			 add		eax, 240
			 mov		h,   eax
hEnd:
		
			 //l
			 mov		eax, maxaddmin
			 shr		eax, 1
			 mov		l,	 eax


			 //s
			 xor		eax, eax
			 cmp		max_min, 0
			 je			sEnd
			 mov		eax, max_min
			 imul		eax, 255
			 cdq
			 cmp		l, 128
			 jl			lSmall
//lBig:
			 mov		ecx, 510
			 sub		ecx, maxaddmin
			 idiv		ecx
			 jmp		sEnd
lSmall:
			 idiv		maxaddmin
sEnd:
			 mov		s, eax

#pragma endregion
	 }

	 return HSL(h,s,l);
 }

 Gdiplus::Color CColorHelper::HSLToRGB(int h, int s, int l)
 {
	 BYTE r,g,b;
	 r = g = b = 0;
	 int max_min, mid_min,hueTemp,maxaddmin;
	 __asm
	 {
		
#pragma region HSL2RGB
		 mov		eax, h
		 imul		eax, 255
		 mov		ecx, 360
		 cdq
		 idiv		ecx
		 mov		hueTemp,eax								//tempvaule used for hsl2rgb

		 cmp		s, 0
		 je			sZero
		 mov		ebx, 255
		 mov		ecx, l			
		 cmp		ecx, 128
		 jge		labelbig
		 mov		eax, s
		 add		eax, 255
		 imul		eax, l
		 cdq		
		 idiv		ebx
		 mov		max_min, eax			
		 jmp		endQ
labelbig:
		 mov		eax,l
		 imul		eax,s
		 cdq
		 idiv		ebx
		 mov		edx,l
		 add		edx,s
		 sub		edx,eax	
		 mov		max_min, edx			
endQ:	
		 mov		eax, 510																				
		 imul		eax,l
		 cdq
		 idiv		ebx
		 sub		eax,max_min
		 mov		mid_min, eax			
		 mov		ecx, max_min	
		 sub		ecx, eax
		 mov		maxaddmin,ecx			

		 mov		eax, hueTemp			
		 mov		ebx, eax				
		 mov		ecx, eax				
		 add		eax, 85					
		 sub		ecx, 85					

		 cmp		eax, 0
		 jge		label0
		 add		eax, 255				
		 jmp		label1					
label0:										
		 cmp		eax, 255				
		 jle		label1					
		 sub		eax, 255				
label1:										
											
		 mov		edx, eax				
		 imul		edx, 6					
		 cmp		edx, 255				
		 jge		label2					
		 imul		edx,maxaddmin			
		 shr		edx,8					
		 add		edx,mid_min				
		 mov		eax, edx				
		 jmp		rEnd					
label2:										
		 mov		edx, eax				
		 imul		edx, 2
		 cmp		edx, 255
		 jge		label3
		 mov		eax, max_min
		 jmp		 rEnd
label3:
		 mov		edx, eax
		 imul		edx, 3
		 cmp		edx, 510
		 jge		label4
		 mov		edx, 170
		 sub		edx, eax
		 imul		edx,maxaddmin
		 imul		edx,6
		 shr		edx, 8
		 add		edx, mid_min
		 mov		eax, edx
		 jmp		rEnd
label4:
		 mov		eax,mid_min
rEnd:

		 cmp		ebx, 0
		 jge		label5
		 add		ebx, 255
		 jmp		label6
label5:
		 cmp		ebx, 255
		 jle		label6      
		 sub		ebx, 255
label6:

		 mov		edx, ebx
		 imul		edx, 6
		 cmp		edx, 255
		 jge		label7
		 imul		edx,maxaddmin
		 shr		edx,8
		 add		edx,mid_min
		 mov		ebx, edx
		 jmp		gEnd
label7:
		 mov		edx, ebx
		 imul		edx, 2
		 cmp		edx, 255
		 jge		label8
		 mov		ebx, max_min
		 jmp		gEnd
label8:
		 mov		edx, ebx
		 imul		edx, 3
		 cmp		edx, 510
		 jge		label9
		 mov		edx, 170
		 sub		edx, ebx
		 imul		edx,maxaddmin
		 imul		edx,6
		 shr		edx, 8
		 add		edx, mid_min
		 mov		ebx, edx
		 jmp		gEnd
label9:
		 mov		ebx,mid_min
gEnd:

		 cmp		ecx, 0
		 jge		label10
		 add		ecx, 255
		 jmp		label11
label10:
		 cmp		ecx, 255
		 jle		label11      
		 sub		ecx, 255
label11:

		 mov		edx, ecx
		 imul		edx, 6
		 cmp		edx, 255
		 jge		label12
		 imul		edx,maxaddmin
		 shr		edx,8
		 add		edx,mid_min
		 mov		ecx, edx
		 jmp		bEnd
label12:
		 mov		edx, ecx
		 imul		edx, 2
		 cmp		edx, 255
		 jge		label13
		 mov		ecx, max_min
		 jmp		bEnd
label13:
		 mov		edx, ecx
		 imul		edx, 3
		 cmp		edx, 510
		 jge		label14
		 mov		edx, 170
		 sub		edx, ecx
		 imul		edx,maxaddmin
		 imul		edx,6
		 shr		edx, 8
		 add		edx, mid_min
		 mov		ecx, edx
		 jmp		bEnd
label14:
		mov			ecx,mid_min
bEnd:
		jmp			rgbEnd
sZero:
		 mov		eax,l
		 mov		ebx, eax
		 mov		ecx, eax
rgbEnd:
		 mov		b,cl			//mov		r, eax
		 mov		g,bl			//mov		g, ebx
		 mov		r,al			//mov		b, ecx

#pragma endregion
	 }

	 return Color(r,g,b);
 }

 Gdiplus::Color CColorHelper::HSLToRGB(const HSL &hsl)
 {
	 return HSLToRGB(hsl.hue,hsl.saturation,hsl.luminance);
 }

 void CColorHelper::SetBrightness(BitmapData* data, long value)   
 {   
	 __asm   
	 {     
		 mov     eax, data			
			 mov     edi, [eax + 16]		
		 mov     esi, [eax + 4]		
		 imul    esi, [eax]			
		 mov     edx, value			
			 cld							
PixelLoop:   
		 mov     ecx, 3				
RGBLoop:   
		 movzx   eax, [edi]			
		 mov     ebx, eax			
			 test    edx, edx			
			 js      Label1				
			 neg     eax					
			 add     eax, 255			
Label1:   
		 imul    eax, edx   
			 sar     eax, 8				
			 add     eax, ebx			
			 jns     Label2				
			 xor     eax, eax			
			 jmp     Label3   
Label2:   
		 cmp     eax, 255			
			 jle     Label3				
			 mov     eax, 255			
Label3:   
		 stosb						
			 loop    RGBLoop   
			 inc     edi					
			 dec     esi   
			 jnz     PixelLoop    
	 }   
 }    

 void CColorHelper::SetBrightness(BYTE* data,int width,int height,long value)
 {
	 __asm   
	 {     		
		 mov     edi, data		
		 mov     esi, width		
		 imul    esi, height		
		 mov     edx, value			
			 cld							
PixelLoop:   
		 mov     ecx, 3				
RGBLoop:   
		 movzx   eax, [edi]			
		 mov     ebx, eax			
			 test    edx, edx			
			 js      Label1				
			 neg     eax					
			 add     eax, 255			
Label1:   
		 imul    eax, edx   
			 sar     eax, 8				
			 add     eax, ebx			
			 jns     Label2				
			 xor     eax, eax			
			 jmp     Label3   
Label2:   
		 cmp     eax, 255			
			 jle     Label3				
			 mov     eax, 255			
Label3:   
		 stosb						
			 loop    RGBLoop   
			 inc     edi					
			 dec     esi   
			 jnz     PixelLoop    
	 }   
 }

 void CColorHelper::SetHueSaturation(BitmapData* data, long hValue, long sValue)   
 {   
	 long I, S, delta;   

	 __asm   
	 {   
		 mov     eax, data   
			 mov     edi, [eax + 16] 
		 mov     ecx, [eax + 4]  
		 imul    ecx, [eax]   
		 mov     I, ecx          
			 cld						
PixelLoop:                  
		 dec     I               
			 js      endConvert   
			 movzx   ecx, [edi + 2]  
		 movzx   ebx, [edi + 1]  
		 movzx   esi, [edi]      
		 cmp     ecx, ebx        
			 jge     HSL1   
			 xchg    ecx, ebx   
HSL1:   
		 cmp     ecx, esi   
			 jge     HSL2   
			 xchg    ecx, esi   
HSL2:   
		 cmp     esi, ebx   
			 jle     HSL3   
			 mov     esi, ebx   
HSL3:   
		 mov     eax, ecx        
			 sub     eax, esi        
			 jnz     HSL4          
			 add     edi, 4          
			 jmp     PixelLoop       
HSL4:   
		 mov     delta, eax      
			 add     esi, ecx   
			 mov     ebx, esi        
			 shr     esi, 1          
			 cmp     esi, 128   
			 jl      HSL5   
			 neg     ebx             
			 add     ebx, 510   
HSL5:   
		 imul    eax, 255        
			 cdq                     
			 div     ebx             
			 cmp     hValue, 0   
			 je      Label1   

			 push    esi             
			 push    eax   
			 mov     S, eax   
			 cmp     cl, [edi + 2]   
		 jne     HSL6   
			 movzx   eax, [edi + 1]  
		 movzx   edx, [edi]   
		 xor     ebx, ebx   
			 jmp     HSL8   
HSL6:   
		 cmp     cl, [edi + 1]   
		 jne     HSL7   
			 movzx   eax, [edi]      
		 movzx   edx, [edi + 2]  
		 mov     ebx, 120   
			 jmp     HSL8   
HSL7:   
		 movzx   eax, [edi + 2]  
		 movzx   edx, [edi + 1]  
		 mov     ebx, 240   
HSL8:   
		 sub     eax, edx		
			 imul    eax, 60         
			 cdq						
			 idiv    dword ptr delta 
			 add     eax, ebx   
			 add     eax, hValue     
			 jns     HSL9          
			 add     eax, 360   
HSL9:   
		 cmp     eax, 360   
			 jl      HSL10   
			 sub     eax, 360   
HSL10:   
		 mov     ebx, 60   
			 cdq               
			 div     ebx       
			 test    eax, 1   
			 jz      HSL11   
			 neg     edx       
			 add     edx, 60   
HSL11:   
		 push    eax       
			 mov     eax, edx  
			 imul    eax, 255  
			 add     eax, 30   
			 mov     ebx, 60   
			 cdq   
			 idiv    ebx       
			 mov     edx, eax  
			 mov     ebx, 255  
			 sub     ebx, S   
			 sub     eax, 128  
			 imul    eax, ebx  
			 sar     eax, 8   
			 sub     edx, eax  
			 mov     eax, edx  
			 sub     esi, 128  
			 js      HSL12     
			 neg     eax   
			 add     eax, 255  
HSL12:   
		 imul    eax, esi  
			 js      HSL13   
			 add     eax, 64   
HSL13:   
		 sar     eax, 7   
			 add     edx, eax  
			 jns     HSL14   
			 xor     edx, edx  
			 jmp     HSL15   
HSL14:   
		 cmp     edx, 255  
			 jle     HSL15   
			 mov     edx, 255  
HSL15:   
		 pop     eax       
			 mov     ebx, ecx  
			 sub     ebx, delta
			 test    eax, eax  
			 jz      H60       
			 // H120:               
			 cmp     eax, 1    
			 jne     H180      
			 xchg    ecx, edx  
			 jmp     H60       
H180:                 
		 cmp     eax, 2    
			 jne     H240      
			 xchg    ecx, ebx  
			 xchg    edx, ebx  
			 jmp     H60       
H240:                 
		 cmp     eax, 3    
			 jne     H300      
			 xchg    ecx, ebx  
			 jmp     H60       
H300:                 
		 cmp     eax, 4    
			 jne     H360      
			 xchg    edx, ebx  
			 xchg    ecx, ebx  
			 jmp     H60       
H360:                 
		 xchg    edx, ebx  
H60:                  
		 mov     [edi], bl      
			 mov     [edi + 1], dl  
			 mov     [edi + 2], cl  

			 pop     eax			
			 pop     esi			
			 cmp     dword ptr sValue, 0   
			 jnz     Label1          
			 add     edi, 4          
			 jmp     PixelLoop       
Label1:   
		 mov     ebx, sValue     
			 test    ebx, ebx        
			 js      Label4         
			 add     bl, al   
			 jnc     Label2          
			 mov     ebx, eax        
			 jmp     Label3   
Label2:   
		 mov     ebx, 255   
			 sub     ebx, sValue     
Label3:   
		 mov     eax, 65025      
			 cdq   
			 div     ebx   
			 sub     eax, 255   
			 mov     ebx, eax        
Label4:   
		 mov     ecx, 3   
RGBLoop:                    
		 movzx   eax, [edi]      
		 mov     edx, eax   
			 sub     eax, esi        
			 imul    eax, ebx   
			 sar     eax, 8   
			 add     eax, edx   
			 jns     Label5   
			 xor     eax, eax        
			 jmp     Label6   
Label5:   
		 cmp     eax, 255   
			 jle     Label6   
			 mov     eax, 255        
Label6:   
		 stosb                   
			 loop    RGBLoop         
			 inc     edi             
			 jmp     PixelLoop       
endConvert:   
	 }   
 }  

void CColorHelper::SetHueSaturation(BYTE* data,int width,int height,long hValue,long sValue)
{
	long I, S, delta;   

	__asm   
	{   
		mov     edi, [data]
		mov     ecx, width 
		imul    ecx, height   
		mov     I, ecx          
			cld						
PixelLoop:                  
		dec     I               
			js      endConvert   
			movzx   ecx, [edi + 2]  
		movzx   ebx, [edi + 1]  
		movzx   esi, [edi]      
		cmp     ecx, ebx        
			jge     HSL1   
			xchg    ecx, ebx   
HSL1:   
		cmp     ecx, esi   
			jge     HSL2   
			xchg    ecx, esi   
HSL2:   
		cmp     esi, ebx   
			jle     HSL3   
			mov     esi, ebx   
HSL3:   
		mov     eax, ecx        
			sub     eax, esi        
			jnz     HSL4          
			add     edi, 4          
			jmp     PixelLoop       
HSL4:   
		mov     delta, eax      
			add     esi, ecx   
			mov     ebx, esi        
			shr     esi, 1          
			cmp     esi, 128   
			jl      HSL5   
			neg     ebx             
			add     ebx, 510   
HSL5:   
		imul    eax, 255        
			cdq                     
			div     ebx             
			cmp     hValue, 0   
			je      Label1   

			push    esi             
			push    eax   
			mov     S, eax   
			cmp     cl, [edi + 2]   
		jne     HSL6   
			movzx   eax, [edi + 1]  
		movzx   edx, [edi]   
		xor     ebx, ebx   
			jmp     HSL8   
HSL6:   
		cmp     cl, [edi + 1]   
		jne     HSL7   
			movzx   eax, [edi]      
		movzx   edx, [edi + 2]  
		mov     ebx, 120   
			jmp     HSL8   
HSL7:   
		movzx   eax, [edi + 2]  
		movzx   edx, [edi + 1]  
		mov     ebx, 240   
HSL8:   
		sub     eax, edx		
			imul    eax, 60         
			cdq						
			idiv    dword ptr delta 
			add     eax, ebx   
			add     eax, hValue     
			jns     HSL9          
			add     eax, 360   
HSL9:   
		cmp     eax, 360   
			jl      HSL10   
			sub     eax, 360   
HSL10:   
		mov     ebx, 60   
			cdq               
			div     ebx       
			test    eax, 1   
			jz      HSL11   
			neg     edx       
			add     edx, 60   
HSL11:   
		push    eax       
			mov     eax, edx  
			imul    eax, 255  
			add     eax, 30   
			mov     ebx, 60   
			cdq   
			idiv    ebx       
			mov     edx, eax  
			mov     ebx, 255  
			sub     ebx, S   
			sub     eax, 128  
			imul    eax, ebx  
			sar     eax, 8   
			sub     edx, eax  
			mov     eax, edx  
			sub     esi, 128  
			js      HSL12     
			neg     eax   
			add     eax, 255  
HSL12:   
		imul    eax, esi  
			js      HSL13   
			add     eax, 64   
HSL13:   
		sar     eax, 7   
			add     edx, eax  
			jns     HSL14   
			xor     edx, edx  
			jmp     HSL15   
HSL14:   
		cmp     edx, 255  
			jle     HSL15   
			mov     edx, 255  
HSL15:   
		pop     eax       
			mov     ebx, ecx  
			sub     ebx, delta
			test    eax, eax  
			jz      H60       
			// H120:               
			cmp     eax, 1    
			jne     H180      
			xchg    ecx, edx  
			jmp     H60       
H180:                 
		cmp     eax, 2    
			jne     H240      
			xchg    ecx, ebx  
			xchg    edx, ebx  
			jmp     H60       
H240:                 
		cmp     eax, 3    
			jne     H300      
			xchg    ecx, ebx  
			jmp     H60       
H300:                 
		cmp     eax, 4    
			jne     H360      
			xchg    edx, ebx  
			xchg    ecx, ebx  
			jmp     H60       
H360:                 
		xchg    edx, ebx  
H60:                  
		mov     [edi], bl      
			mov     [edi + 1], dl  
			mov     [edi + 2], cl  

			pop     eax			
			pop     esi			
			cmp     dword ptr sValue, 0   
			jnz     Label1          
			add     edi, 4          
			jmp     PixelLoop       
Label1:   
		mov     ebx, sValue     
			test    ebx, ebx        
			js      Label4         
			add     bl, al   
			jnc     Label2          
			mov     ebx, eax        
			jmp     Label3   
Label2:   
		mov     ebx, 255   
			sub     ebx, sValue     
Label3:   
		mov     eax, 65025      
			cdq   
			div     ebx   
			sub     eax, 255   
			mov     ebx, eax        
Label4:   
		mov     ecx, 3   
RGBLoop:                    
		movzx   eax, [edi]      
		mov     edx, eax   
			sub     eax, esi        
			imul    eax, ebx   
			sar     eax, 8   
			add     eax, edx   
			jns     Label5   
			xor     eax, eax        
			jmp     Label6   
Label5:   
		cmp     eax, 255   
			jle     Label6   
			mov     eax, 255        
Label6:   
		stosb                   
			loop    RGBLoop         
			inc     edi             
			jmp     PixelLoop       
endConvert:   
	}   
}
