#pragma once
//#include <afxwin.h>

class CDib : public CObject
{
public:
	// Ū�J�ɮ� //
	CDib ( const char* bmpfile );
	CDib ( CFile& file );
	// �ഫ�� DDB //
	CDib ( CBitmap& ddb, CPalette* palette );
	// �إߪť� DIB //
	CDib ( int Width, int Height, int BitsPerPixel );

	~CDib ( );

// DIB ������T //
	// �Ǧ^���I�}�C //
	const void*		GetBits() const;
	// �Ǧ^�C���I���줸�� //
	int			GetBitsPerPixel ( ) const;
	// �Ǧ^�v���ؤo //
	BOOL		GetDimension ( CSize& size ) const;
    DWORD		Width() const;
    DWORD		Height() const;
	// �զ�L���j�p //
	int			GetPaletteCount () const;
	// �C�����˽u�һݪ��줸�ռ� //
	DWORD		BytesPerLine() const;
	// ���o�զ�L //
	CPalette*	GetPalette();

// ø�Ϩ�� //
	// �b DC �W�Y���X //
	int			StretchToDC ( CDC& dc, CRect& src, CRect& dst, DWORD rop = SRCCOPY );
	// �b DC �W���ؤo��X //
	int			SetToDC ( CDC& dc, CRect& src, CPoint& dst );
	// ��X�� DDB //
	BOOL		CopyTo ( CBitmap& );

// �ɮ׬��� //
	// �s�� //
	BOOL		SaveFile ( char* bmpfile );
	// �g�J���w�ɮ� //
	BOOL		DoWrite ( CFile& file );
	// �۫��w�ɮ�Ū�J //
	BOOL		DoRead ( CFile& file );

protected:
	// �p�⹳�I�}�C���j�p //
	long		GetBodySize() const;
	// �p����Y�t�զ�L���j�p //
	int			GetHeaderSize() const;
	// ��]���Y������� //
	void		InitDibInfo ( int , int = 0, int = 0 );
	// �ƻs�զ�L //
	void		CopyPalette ( CPalette& );

	// ���Y //
	BITMAPINFO*	DibInfo;
	// ���I�}�C //
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

