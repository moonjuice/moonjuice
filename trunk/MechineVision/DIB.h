// DIB.h

#ifndef __DIB_H__
#define __DIB_H__

class CDib
{

public:
	void CCopyImage( unsigned char *array4);
	CDib();
	~CDib();
	BOOL Load( const char * );
	BOOL Save( const char * );
	BOOL Save2( unsigned char *,const char * );
	BOOL Draw( CDC *, int nX = 0, int nY = 0, int nWidth = -1, int nHeight = -1 );
	BOOL Draw2( CDC *, int nX = 0, int nY = 0, int nWidth = -1, int nHeight = -1 ,unsigned char *array=NULL, int Width=0, int Height=0);
	BOOL SetPalette( CDC * );
    int ** Get_Image_Array(int height,int width);
	BITMAPINFOHEADER *m_pBIH;
private:

	CPalette m_Palette;
    
	unsigned char *m_pDibBits; 
	DWORD m_dwDibSize;
	unsigned char *m_pDib; // ¹³ÂI°}¦C //
	RGBQUAD *m_pPalette;
	int m_nPaletteEntries;
	unsigned char *pDib ;

};
#endif
