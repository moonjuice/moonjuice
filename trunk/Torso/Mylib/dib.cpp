#include "dib.h"
#include "stdafx.h"

CDib::CDib ( int Width, int Height, int BitsPerPixel )
	: DibBits ( NULL ), DibInfo ( NULL )
{
	InitDibInfo ( BitsPerPixel, Width, Height );
	DibBits = (void*) new char[GetBodySize()];
}

CDib::CDib ( const char* bmpfile )
	: DibBits ( NULL ), DibInfo ( NULL )
{
	CFile fp ( bmpfile, CFile::modeRead | CFile::typeBinary );
	DoRead ( fp );
	fp.Close();
}

CDib::CDib ( CFile& file )
	: DibBits ( NULL ), DibInfo ( NULL )
{
	DoRead ( file );
}

CDib::CDib ( CBitmap& ddb, CPalette* palette )
	: DibBits ( NULL ), DibInfo ( NULL )
{
	BITMAP ddbinfo;
	ddb.GetBitmap ( &ddbinfo );	/** [1] **/
	InitDibInfo ( ddbinfo.bmPlanes*ddbinfo.bmBitsPixel, ddbinfo.bmWidth, ddbinfo.bmHeight );
	DibBits = (void*) new char[GetBodySize()];	/** [2] **/
	if ( palette ) CopyPalette ( *palette );	/** [3] **/

	// copy DDB to DIB //
	// generate DC of desktop window //
	CWnd DesktopWnd;	/** [4] **/
	DesktopWnd.Attach ( ::GetDesktopWindow() );
	CWindowDC dtpDC ( &DesktopWnd );
	// use GetDIBits to convert DDB to DIB //
	::GetDIBits( dtpDC.m_hDC,		// handle of device context 
				 HBITMAP ( ddb ),	// handle of bitmap 
				 0,		// first scan line to set in destination bitmap  
				 ddbinfo.bmHeight,	// number of scan lines to copy 
				 LPVOID ( DibBits ),	// address of array for bitmap bits 
				 DibInfo,	// address of structure with bitmap data 
				 DIB_RGB_COLORS // RGB or palette index 
				 );
	DesktopWnd.Detach();
}


CDib::~CDib ( )
{
	delete[] DibInfo;
	delete[] DibBits;
}

CPalette* CDib::GetPalette()
{
	int i;
	CPalette *palette;
	LOGPALETTE *log;

	if ( !GetPaletteCount() ) return NULL;

	log = (LOGPALETTE*) new char [ sizeof(WORD)*2 + sizeof(PALETTEENTRY)*GetPaletteCount() ];
	log->palVersion = 0x300;
	log->palNumEntries = WORD(GetPaletteCount());

	for ( i = 0 ; i < log->palNumEntries ; i ++ )
	{
		log->palPalEntry[i].peRed = DibInfo->bmiColors[i].rgbRed;
		log->palPalEntry[i].peGreen = DibInfo->bmiColors[i].rgbGreen;
		log->palPalEntry[i].peBlue = DibInfo->bmiColors[i].rgbBlue;
		log->palPalEntry[i].peFlags = NULL;
	}

	palette = new CPalette;
	palette->CreatePalette( log );
	delete[] log;
	return palette;
}

BOOL CDib::CopyTo ( CBitmap& ddb )
{
	BOOL ret;
	if ( !DibInfo ) return FALSE;
	// copy DIB to DDB //
	// generate DC of desktop window //
	CWnd DesktopWnd;
	DesktopWnd.Attach ( ::GetDesktopWindow() );
	CWindowDC dtpDC ( &DesktopWnd );
	// use SetDIBits to convert DIB to DDB //

	ret = ::SetDIBits( dtpDC.m_hDC,	// handle of device context
						HBITMAP (ddb),	// handle of bitmap
						0,	// starting scan line
						DibInfo->bmiHeader.biHeight,	// number of scan lines
						DibBits,	// array of bitmap bits
						DibInfo,	// address of structure with bitmap data
						DIB_RGB_COLORS	// type of color indices to use
				 );
	DesktopWnd.Detach();
    return ret;
}

BOOL CDib::SaveFile ( char* bmpfile )
{
	if ( !DibInfo ) return FALSE;

	CFile fp ( bmpfile, CFile::modeCreate | CFile::typeBinary );
	DoWrite ( fp );
	fp.Close();	
			
	return TRUE;
}

int CDib::SetToDC ( CDC& dc, CRect& src, CPoint& dst )
{
	if ( !DibInfo ) return FALSE;

	return ::SetDIBitsToDevice(
				dc.m_hDC,	// handle of device context
				dst.x,	// x-coordinate of upper-left corner of dest. rect. 
				dst.y,	// y-coordinate of upper-left corner of dest. rect. 
				src.Width(),	// source rectangle width 
				src.Height(),	// source rectangle height 
				src.left,	// x-coordinate of lower-left corner of source rect. 
				src.top,	// y-coordinate of lower-left corner of source rect. 
				0,	// first scan line in array 
				DibInfo->bmiHeader.biHeight,	// number of scan lines 
				DibBits,	// address of array with DIB bits 
				DibInfo,	// address of structure with bitmap info. 
				DIB_RGB_COLORS	// RGB or palette indices 
			);	
}

int CDib::StretchToDC ( CDC& dc, CRect& src, CRect& dst, DWORD rop )
{
	if ( !DibInfo ) return FALSE;

	return ::StretchDIBits (
				dc.m_hDC,	// handle of device context
				dst.left,	// x-coordinate of upper-left corner of dest. rect. 
				dst.top,	// y-coordinate of upper-left corner of dest. rect. 
				dst.Width(),	// width of destination rectangle 
				dst.Height(),	// height of destination rectangle 
				src.left,	// x-coordinate of lower-left corner of source rect. 
				src.top,	// y-coordinate of lower-left corner of source rect. 
				src.Width(),	// source rectangle width 
				src.Height(),	// source rectangle height 
				DibBits,	// address of array with DIB bits 
				DibInfo,	// address of structure with bitmap info. 
				DIB_RGB_COLORS,	// RGB or palette indices 
				rop		// raster operation code
			);	
}

int CDib::GetPaletteCount () const
{
	int PaletteSize = 0;
	if (!DibInfo) return 0;
	switch ( DibInfo->bmiHeader.biBitCount )
	{
		case 1:
			PaletteSize = 2;
			break;
		case 4:
			PaletteSize = 16;
			break;
		case 8:
			PaletteSize = 256;
			break;
	}
	return PaletteSize;
}


BOOL CDib::DoRead ( CFile& file )
{
	BITMAPFILEHEADER bmfileheader;
	BITMAPINFOHEADER bmheader;
	long size, headpos;
	int PaletteSize = 0;
	int ret, cbHeaderSize;

	/** [9] 讀取檔案指標的位置 **/
	headpos = file.GetPosition();
	/** [10] 讀取結構 BITMAPFILEHEADER **/
	ret = file.Read ( &bmfileheader, sizeof(BITMAPFILEHEADER) );
	if ( ( ret != sizeof(BITMAPFILEHEADER) ) || ( bmfileheader.bfType != 0x4d42) ) 
		return FALSE;
	/** [11] 讀取 BITMAPINFOHEADER **/		
	ret = file.Read ( &bmheader, sizeof(BITMAPINFOHEADER) );
	if ( ret != sizeof(BITMAPINFOHEADER) )
		return FALSE;
	/** [12] 計算 RGBQUAD 的大小 **/
	switch ( bmheader.biBitCount )
	{
		case 1:
			PaletteSize = 2;
			break;
		case 4:
			PaletteSize = 16;
			break;
		case 8:
			PaletteSize = 256;
			break;
	}
	/** [13] 分配一塊記憶體給BITMAPINFO 使用 **/
	cbHeaderSize = sizeof(BITMAPINFOHEADER) + PaletteSize*sizeof ( RGBQUAD );
	DibInfo = (BITMAPINFO*) new char [ cbHeaderSize ];
	DibInfo->bmiHeader = bmheader;

	if ( PaletteSize )
	{	/** [14] **/
		ret = file.Read ( &(DibInfo->bmiColors[0]), PaletteSize*sizeof ( RGBQUAD ) );
		if ( ret != int( PaletteSize*sizeof ( RGBQUAD ) ) )
		{
			delete[] DibInfo;
			DibInfo = NULL;
			return FALSE;
		}
	}
	size = BytesPerLine() * DibInfo->bmiHeader.biHeight;
	DibBits = (void*) new char[GetBodySize()];

	/** [15] 把檔案指標移到 DIB 影像陣列 **/
	file.Seek ( headpos + bmfileheader.bfOffBits, CFile::begin );

	ret = file.Read ( DibBits, size );
	if ( ret != int ( size ) )
	{
		delete[] DibInfo;
		delete[] DibBits;
		DibInfo = NULL;
		DibBits = NULL;
	}

	return TRUE;
}

BOOL CDib::DoWrite ( CFile& file )
{
	long size;
	int PaletteSize = GetPaletteCount();
	BITMAPFILEHEADER bmfileheader;

	if ( !DibInfo )	 return FALSE;

	size = BytesPerLine() * DibInfo->bmiHeader.biHeight;
	
	/** [16] 填寫 *.BMP 檔頭資料 **/
	bmfileheader.bfType = 0x4d42;
	bmfileheader.bfReserved1 = bmfileheader.bfReserved2 = 0;
	bmfileheader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+PaletteSize*sizeof( RGBQUAD );
	bmfileheader.bfSize = bmfileheader.bfOffBits + size;
	/** [17] **/
	file.Write ( &bmfileheader, sizeof(BITMAPFILEHEADER) );
	file.Write ( DibInfo, sizeof(BITMAPINFOHEADER)+PaletteSize*sizeof( RGBQUAD ) );
	file.Write ( DibBits, size );

	return TRUE;
}

void CDib::InitDibInfo( int BitsPerPixel, int w, int h )
{
	int i;
	int PaletteSize = 0, cbHeaderSize;

	switch ( BitsPerPixel )	/** [5] **/
	{
		case 1:
			PaletteSize = 2;
			break;
		case 4:
			PaletteSize = 16;
			break;
		case 8:
			PaletteSize = 256;
			break;
		case 15:
		case 16:
			BitsPerPixel = 16;
			break;
	}

	cbHeaderSize = sizeof(BITMAPINFOHEADER) + PaletteSize * sizeof ( RGBQUAD );
	if ( DibInfo ) delete[] DibInfo;
	DibInfo = (BITMAPINFO*) new char [ cbHeaderSize ]; /** [6] **/
	/** [7] 填寫 BITMAPINFO 表格 **/
	DibInfo->bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
	DibInfo->bmiHeader.biWidth = w;
	DibInfo->bmiHeader.biHeight = h;
	DibInfo->bmiHeader.biPlanes = 1;
	DibInfo->bmiHeader.biBitCount = short(BitsPerPixel);
	DibInfo->bmiHeader.biCompression = BI_RGB;
	DibInfo->bmiHeader.biSizeImage = BytesPerLine() * DibInfo->bmiHeader.biHeight;
	DibInfo->bmiHeader.biXPelsPerMeter =
	DibInfo->bmiHeader.biYPelsPerMeter = 120;
	DibInfo->bmiHeader.biClrUsed = PaletteSize;
	DibInfo->bmiHeader.biClrImportant = PaletteSize;

	if ( PaletteSize )
	{	/** [8] 把 RGBQUAD 陣列中的每一個元素全部填入 0 **/
		for ( i = 0 ; i < PaletteSize ; i ++ )
		{
			DibInfo->bmiColors[i].rgbRed = 0;
			DibInfo->bmiColors[i].rgbGreen = 0;
			DibInfo->bmiColors[i].rgbBlue = 0;
			DibInfo->bmiColors[i].rgbReserved = 0;
		}
	}
}


void CDib::CopyPalette ( CPalette& palette )
{
	int i;
	PALETTEENTRY *pe;

	if ( !DibInfo->bmiHeader.biBitCount ) return;
	if ( palette.GetEntryCount() != DibInfo->bmiHeader.biBitCount ) return;

	pe = new PALETTEENTRY[palette.GetEntryCount()];
	palette.GetPaletteEntries( 0, palette.GetEntryCount(), pe );
	
	for ( i = 0 ; i < palette.GetEntryCount() ; i ++ )
	{
		DibInfo->bmiColors[i].rgbRed = pe[i].peRed;
		DibInfo->bmiColors[i].rgbGreen = pe[i].peGreen;
		DibInfo->bmiColors[i].rgbBlue = pe[i].peBlue;
		DibInfo->bmiColors[i].rgbReserved = 0;
	}

	delete[] pe;
}

int CDib::GetHeaderSize() const
{
	int PaletteSize;
	switch ( DibInfo->bmiHeader.biBitCount )
	{
		case 1:
			PaletteSize = 2 * sizeof ( RGBQUAD ) + sizeof(BITMAPINFOHEADER);
			break;
		case 4:
			PaletteSize = 16 * sizeof ( RGBQUAD ) + sizeof(BITMAPINFOHEADER);
			break;
		case 8:
			PaletteSize = 256 * sizeof ( RGBQUAD ) + sizeof(BITMAPINFOHEADER);
			break;
		default:
			PaletteSize = sizeof(BITMAPINFOHEADER);
			break;
	}
	return PaletteSize;
}
