// saveClipBoard.cpp : Defines the entry point for the console application.
// https://msdn.microsoft.com/en-us/library/ms533837(v=VS.85).aspx

#include "stdafx.h"

#include <tchar.h>
#include <memory>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

//INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid);  // helper function

INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

wchar_t *GetWC(char *c)
{
	const size_t cSize = strlen(c)+1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs (wc, c, cSize);

	return wc;
}

void transForm(char *b, char *p) {
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID   encoderClsid;
	Status  stat;
	Image*   image = new Image(GetWC(b));

	// Get the CLSID of the PNG encoder.
	GetEncoderClsid(L"image/png", &encoderClsid);

	stat = image->Save(GetWC(p), &encoderClsid, NULL);

	if(stat == Ok)
		printf("Png was saved successfully\n");
	else
		printf("Failure: stat = %d\n", stat); 

	delete image;
	GdiplusShutdown(gdiplusToken);
}


BOOL SaveBitmapToFile(HBITMAP hBitmap,LPTSTR lpFileName);
Status HBitmapToBitmap( HBITMAP source, PixelFormat pixel_format, Bitmap** result_out );

int main(int argc, char* argv[])
{
	char name[128], namePng[128], cc;
	UINT nRet;
	HBITMAP   hBitmap;

	if (argc != 2) {
		printf("Usage: %s dirve-name", argv[0]);
		return 0;
	}

	for(int idx=1;;idx++) {
		Sleep(200);
		scanf("%c",&cc);
		if ( 0 == OpenClipboard(NULL)) {
			printf("normal open null\n");
			continue;
		}
		//       �򿪼�����       
		// 	HANDLE   hClip=::GetClipboardData(CF_ENHMETAFILE); 
		// 	HENHMETAFILE   hEnhMetaFile=(HENHMETAFILE)hClip; 
		// 	HENHMETAFILE   hMetaFile=CopyEnhMetaFile(hEnhMetaFile, "f:\\9988.jpg ");//���浽�ļ� 
		// 	DeleteEnhMetaFile(hMetaFile);   
		nRet = EnumClipboardFormats(CF_BITMAP);
		if( nRet )
		{
			hBitmap   =   (HBITMAP)GetClipboardData(CF_BITMAP);
			sprintf(name, "%s:\\%d.bmp",argv[1], idx);
			sprintf(namePng, "%s:\\%d.png",argv[1], idx);
			SaveBitmapToFile(hBitmap, name);
			transForm(name, namePng);
			DeleteFile(name);
		} else {
			printf("not bmp pic\n");
		}
		
		CloseClipboard();
	}

	return 0;
}

BOOL SaveBitmapToFile(HBITMAP hBitmap,LPTSTR lpFileName) //hBitmap Ϊץ�õ���Ļλͼ���
{    	 //lpFileName Ϊλͼ�ļ���
	BOOL bRet = FALSE;
	HDC            hDC;         
	//�豸������
	int            iBits;      
	//��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���
	WORD            wBitCount;   
	//λͼ��ÿ��������ռ�ֽ���
	//�����ɫ���С�� λͼ�������ֽڴ�С ��
	//λͼ�ļ���С �� д���ļ��ֽ���
	DWORD dwPaletteSize=0;
	DWORD dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP Bitmap;    
	
	//λͼ���Խṹ
	BITMAPFILEHEADER  bmfHdr;        
	
	//λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER   bi;            
	
	//λͼ��Ϣͷ�ṹ 
	LPBITMAPINFOHEADER lpbi;          
	
	//ָ��λͼ��Ϣͷ�ṹ
	HANDLE fh = NULL;
	HANDLE hDib = NULL;
	HANDLE hPal = NULL;
	HANDLE hOldPal=NULL;
	//�����ļ��������ڴ�������ɫ����
	
	//����λͼ�ļ�ÿ��������ռ�ֽ���
	if(NULL == (hDC = CreateDC("DISPLAY",NULL,NULL,NULL)))
	{
		DeleteDC(hDC);	
		return bRet;
	}
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);	
	DeleteDC(hDC);
	
	if (iBits <= 1) 
		wBitCount = 1;	
	else if (iBits <= 4) 
		wBitCount = 4;
	else if (iBits <= 8) 
		wBitCount = 8;
	else
		wBitCount = 24; 
	
	//����λͼ��Ϣͷ�ṹ
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;
	
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32)* 4*Bitmap.bmHeight ;
	//Ϊλͼ���ݷ����ڴ�
	hDib  = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	if (NULL == hDib) 
	{
		GlobalFree(hDib);
		return bRet;
	} 
	
	
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	if (NULL == lpbi) 
	{	
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return bRet;
	} 
	
	
	*lpbi = bi;
	// �����ɫ��   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC  = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	else
	{
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return bRet;
	}
	
	// ��ȡ�õ�ɫ�����µ�����ֵ
	GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,\
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,\
		(BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	
	//�ָ���ɫ��   
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	//����λͼ�ļ�    
	fh = CreateFile(lpFileName, GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
	{
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return bRet;
	}
	// ����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+ dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER)+ dwPaletteSize;
	// д��λͼ�ļ�ͷ
	if(0 == (WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL)))
	{
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return bRet;
	}
	// д��λͼ�ļ���������
	if(0 == (WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL)))
	{
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return bRet;
	}
	//���   
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	bRet = TRUE;
	return bRet;
}

Status HBitmapToBitmap( HBITMAP source, PixelFormat pixel_format, Bitmap** result_out )
{
  BITMAP source_info = { 0 };
  if( !::GetObject( source, sizeof( source_info ), &source_info ) )
    return Gdiplus::GenericError;

  Gdiplus::Status s;

  std::auto_ptr< Gdiplus::Bitmap > target( new Gdiplus::Bitmap( source_info.bmWidth, source_info.bmHeight, pixel_format ) );
  if( !target.get() )
    return Gdiplus::OutOfMemory;
  if( ( s = target->GetLastStatus() ) != Gdiplus::Ok )
    return s;

  Gdiplus::BitmapData target_info;
  Gdiplus::Rect rect( 0, 0, source_info.bmWidth, source_info.bmHeight );

  s = target->LockBits( &rect, Gdiplus::ImageLockModeWrite, pixel_format, &target_info );
  if( s != Gdiplus::Ok )
    return s;

  if( target_info.Stride != source_info.bmWidthBytes )
    return Gdiplus::InvalidParameter; // pixel_format is wrong!

  CopyMemory( target_info.Scan0, source_info.bmBits, source_info.bmWidthBytes * source_info.bmHeight );

  s = target->UnlockBits( &target_info );
  if( s != Gdiplus::Ok )
    return s;

  *result_out = target.release();

  return Gdiplus::Ok;
}
