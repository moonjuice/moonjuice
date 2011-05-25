#pragma once
//#include <afxwin.h>

class CDib : public CObject
{
public:
	// 讀入檔案 //
	CDib ( const char* bmpfile );
	CDib ( CFile& file );
	// 轉換自 DDB //
	CDib ( CBitmap& ddb, CPalette* palette );
	// 建立空白 DIB //
	CDib ( int Width, int Height, int BitsPerPixel );

	~CDib ( );

// DIB 相關資訊 //
	// 傳回像點陣列 //
	const void*		GetBits() const;
	// 傳回每像點的位元數 //
	int			GetBitsPerPixel ( ) const;
	// 傳回影像尺寸 //
	BOOL		GetDimension ( CSize& size ) const;
    DWORD		Width() const;
    DWORD		Height() const;
	// 調色盤的大小 //
	int			GetPaletteCount () const;
	// 每條掃瞄線所需的位元組數 //
	DWORD		BytesPerLine() const;
	// 取得調色盤 //
	CPalette*	GetPalette();

// 繪圖函數 //
	// 在 DC 上縮放輸出 //
	int			StretchToDC ( CDC& dc, CRect& src, CRect& dst, DWORD rop = SRCCOPY );
	// 在 DC 上等尺寸輸出 //
	int			SetToDC ( CDC& dc, CRect& src, CPoint& dst );
	// 輸出到 DDB //
	BOOL		CopyTo ( CBitmap& );

// 檔案相關 //
	// 存檔 //
	BOOL		SaveFile ( char* bmpfile );
	// 寫入指定檔案 //
	BOOL		DoWrite ( CFile& file );
	// 自指定檔案讀入 //
	BOOL		DoRead ( CFile& file );

protected:
	// 計算像點陣列的大小 //
	long		GetBodySize() const;
	// 計算標頭含調色盤的大小 //
	int			GetHeaderSize() const;
	// 初設標頭部的資料 //
	void		InitDibInfo ( int , int = 0, int = 0 );
	// 複製調色盤 //
	void		CopyPalette ( CPalette& );

	// 標頭 //
	BITMAPINFO*	DibInfo;
	// 像點陣列 //
	void*		DibBits;	
};

inline int CDib::GetBitsPerPixel() const { return ( !DibInfo ) ? 0 : DibInfo->bmiHeader.biBitCount; }
inline const void* CDib::GetBits() const { return DibBits; }
inline DWORD CDib::Width() const { return DibInfo->bmiHeader.biWidth; }
inline DWORD CDib::Height() const { return DibInfo->bmiHeader.biHeight; }
inline long CDib::GetBodySize() const { return BytesPerLine() * DibInfo->bmiHeader.biHeight; }
inline DWORD CDib::BytesPerLine() const 
{ 
	DWORD bytes_per_line;
	// fillup byte //
    bytes_per_line = ( DibInfo->bmiHeader.biWidth * GetBitsPerPixel()+7)/8;
	// quad-byte alignment //
    bytes_per_line = ( bytes_per_line + 3 ) / 4;
    return bytes_per_line*4;
}

inline BOOL CDib::GetDimension ( CSize& size ) const
{
	if ( !DibInfo ) return FALSE;	
	size.cx = DibInfo->bmiHeader.biWidth;
	size.cy = DibInfo->bmiHeader.biHeight;
	return TRUE;
}

