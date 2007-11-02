#include <windows.h>
#include "gdiu.h"


bool TransparentBltU(
     HDC dcDest,         // handle to Dest DC
     int nXOriginDest,   // x-coord of destination upper-left corner
     int nYOriginDest,   // y-coord of destination upper-left corner
     int nWidthDest,     // width of destination rectangle
     int nHeightDest,    // height of destination rectangle
     HDC dcSrc,          // handle to source DC
     int nXOriginSrc,    // x-coord of source upper-left corner
     int nYOriginSrc,    // y-coord of source upper-left corner
     int nWidthSrc,      // width of source rectangle
     int nHeightSrc,     // height of source rectangle
     UINT crTransparent  // color to make transparent
  )
{
     if (nWidthDest < 1) return false;
     if (nWidthSrc < 1) return false; 
     if (nHeightDest < 1) return false;
     if (nHeightSrc < 1) return false;

     HDC dc = CreateCompatibleDC(NULL);
     HBITMAP bitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1,
            GetDeviceCaps(dc, BITSPIXEL), NULL);

     if (bitmap == NULL)
         return false;

     HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, bitmap);

     if (!BitBlt(dc, 0, 0, nWidthSrc, nHeightSrc, dcSrc,
                     nXOriginSrc, nYOriginSrc, SRCCOPY))
         return false;

     HDC maskDC = CreateCompatibleDC(NULL);
     HBITMAP maskBitmap = CreateBitmap(nWidthSrc,
            nHeightSrc, 1, 1, NULL);

     if (maskBitmap == NULL)
         return false;

     HBITMAP oldMask =  (HBITMAP)SelectObject(maskDC,
            maskBitmap);
 
     SetBkColor(maskDC, RGB(0,0,0));
     SetTextColor(maskDC, RGB(255,255,255));
     if (!BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,NULL,
                     0,0,BLACKNESS))
         return false;

     SetBkColor(dc, crTransparent);
     BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,dc,0,0,SRCINVERT);

     SetBkColor(dc, RGB(0,0,0));
     SetTextColor(dc, RGB(255,255,255));
     BitBlt(dc, 0,0,nWidthSrc,nHeightSrc,maskDC,0,0,SRCAND); 

     HDC newMaskDC = CreateCompatibleDC(NULL);
     HBITMAP newMask;
     newMask = CreateBitmap(nWidthDest, nHeightDest, 1,
                GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newMask == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         SelectObject(maskDC, oldMask);
         DeleteDC(maskDC);
          DeleteDC(newMaskDC);
         return false;
     }

     SetStretchBltMode(newMaskDC, COLORONCOLOR);
     HBITMAP oldNewMask = (HBITMAP) SelectObject(newMaskDC,
            newMask);
     StretchBlt(newMaskDC, 0, 0, nWidthDest, nHeightDest,
            maskDC, 0, 0, nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(maskDC, oldMask);
     DeleteDC(maskDC);
    
     HDC newImageDC = CreateCompatibleDC(NULL);
     HBITMAP newImage = CreateBitmap(nWidthDest, nHeightDest,
                1, GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newImage == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         DeleteDC(newMaskDC);
         return false;
     }

     HBITMAP oldNewImage = (HBITMAP)SelectObject(newImageDC,
                newImage);
     StretchBlt(newImageDC, 0, 0, nWidthDest, nHeightDest,
                dc, 0, 0, nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(dc, oldBitmap);
     DeleteDC(dc);

     BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest,
                nHeightDest, newMaskDC, 0, 0, SRCAND);

     BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest,
            nHeightDest, newImageDC, 0, 0, SRCPAINT);
 
     SelectObject(newImageDC, oldNewImage);
     DeleteDC(newImageDC);
     SelectObject(newMaskDC, oldNewMask);
     DeleteDC(newMaskDC);
 
     return true;
}

bool AlphaBlendU(HDC dcDest, int x, int y, int cx, int cy,
    HDC dcSrc, int sx, int sy, int scx, int scy, int alpha)
{
    BITMAPINFOHEADER BMI;
    // Fill in the header info.
    BMI.biSize = sizeof(BITMAPINFOHEADER);
    BMI.biWidth = cx;
    BMI.biHeight = cy;
    BMI.biPlanes = 1;
    BMI.biBitCount = 32;
    BMI.biCompression = BI_RGB;   // No compression
    BMI.biSizeImage = 0;
    BMI.biXPelsPerMeter = 0;
    BMI.biYPelsPerMeter = 0;
    BMI.biClrUsed = 0;           // Always use the whole palette.
    BMI.biClrImportant = 0;
    
    BYTE * pSrcBits;
    HBITMAP hbmSrc;
    // Create DIB section in shared memory
    hbmSrc = CreateDIBSection (dcSrc, (BITMAPINFO *)&BMI,
        DIB_RGB_COLORS, (void **)&pSrcBits, 0, 0l);

    BYTE * pDestBits;
    HBITMAP hbmDest;
    // Create DIB section in shared memory
    hbmDest = CreateDIBSection (dcDest, (BITMAPINFO *)&BMI,
        DIB_RGB_COLORS, (void **)&pDestBits, 0, 0l);
    

    // Copy our source and destination bitmaps onto our DIBSections,
    // so we can get access to their bits using the BYTE *'s we
    // passed into CreateDIBSection

    HDC dc = CreateCompatibleDC(NULL);
    
    HBITMAP dcOld = (HBITMAP) SelectObject(dc, hbmSrc);
    
    if (!StretchBlt(dc, 0, 0, cx, cy, dcSrc, sx, sy,
                    scx, scy, SRCCOPY))
        return false;

    SelectObject(dc, hbmDest);
    if(!StretchBlt(dc, 0, 0, cx, cy, dcDest, x, y, cx, cy, SRCCOPY))
        return false;

    SelectObject(dc, dcOld);
    DeleteDC(dc);

    for (int j = 0; j < cy; ++j)
    {
        LPBYTE pbDestRGB = (LPBYTE)&((DWORD*)pDestBits)[j * cx];
        LPBYTE pbSrcRGB = (LPBYTE)&((DWORD*)pSrcBits)[j * cx];

        for (int i = 0; i < cx; ++i)
        {
            pbSrcRGB[0]=(pbDestRGB[0] * (255-alpha) +
                    pbSrcRGB[0] * alpha)>>8;
            pbSrcRGB[1]=(pbDestRGB[1] * (255-alpha) +
                    pbSrcRGB[1] * alpha)>>8;
            pbSrcRGB[2]=(pbDestRGB[2] * (255-alpha) +
                    pbSrcRGB[2] * alpha)>>8;
            pbSrcRGB += 4;
            pbDestRGB += 4;
        }
    }

    dc = CreateCompatibleDC(NULL);
    
    dcOld = (HBITMAP) SelectObject(dc, hbmSrc);
    if (!BitBlt(dcDest, x, y, cx, cy, dc, 0, 0, SRCCOPY))
        return false;
    DeleteDC(dc);

    DeleteObject(hbmSrc);    
    DeleteObject(hbmDest);    
    
    return true;
}

bool AlphaBlendCK(HDC dcDest, int x, int y, int cx, int cy,
                  HDC dcSrc, int sx, int sy, int scx, int scy,
                  int alpha, COLORREF rgbMask)
{
    BITMAPINFOHEADER BMI;
    // Fill in the header info.
    BMI.biSize = sizeof(BITMAPINFOHEADER);
    BMI.biWidth = cx;
    BMI.biHeight = cy;
    BMI.biPlanes = 1;
    BMI.biBitCount = 32;
    BMI.biCompression = BI_RGB;   // No compression
    BMI.biSizeImage = 0;
    BMI.biXPelsPerMeter = 0;
    BMI.biYPelsPerMeter = 0;
    BMI.biClrUsed = 0;           // Always use the whole palette.
    BMI.biClrImportant = 0;
    
    BYTE * pSrcBits;
    HBITMAP hbmSrc;
    // Create DIB section in shared memory
    hbmSrc = CreateDIBSection (dcSrc, (BITMAPINFO *)&BMI,
        DIB_RGB_COLORS, (void **)&pSrcBits, 0, 0l);
    
    BYTE * pDestBits;
    HBITMAP hbmDest;
    // Create DIB section in shared memory
    hbmDest = CreateDIBSection (dcDest, (BITMAPINFO *)&BMI,
        DIB_RGB_COLORS, (void **)&pDestBits, 0, 0l);
    
    
    // Copy our source and destination bitmaps onto our DIBSections,
    // so we can get access to their bits using the BYTE *'s we
    // passed into CreateDIBSection
    
    HDC dc = CreateCompatibleDC(NULL);
    
    HBITMAP dcOld = (HBITMAP) SelectObject(dc, hbmSrc);
    
    if (!StretchBlt(dc, 0, 0, cx, cy, dcSrc, sx, sy,
                    scx, scy, SRCCOPY))
        return false;

    SelectObject(dc, hbmDest);
    if(!StretchBlt(dc, 0, 0, cx, cy, dcDest, x, x, cx, cy, SRCCOPY))
        return false;

    SelectObject(dc, dcOld);
    DeleteDC(dc);
    
    int red = GetRValue(rgbMask);
    int green = GetGValue(rgbMask);
    int blue = GetBValue(rgbMask);
    
    for (int j = 0; j < cy; ++j)
    {
        LPBYTE pbDestRGB = (LPBYTE)&((DWORD*)pDestBits)[j * cx];
        LPBYTE pbSrcRGB = (LPBYTE)&((DWORD*)pSrcBits)[j * cx];
        
        for (int i = 0; i < cx; ++i)
        {
            if (pbSrcRGB[0] != blue ||
                pbSrcRGB[1] != green ||
                pbSrcRGB[2] != red)
            {    
                pbSrcRGB[0]=(pbDestRGB[0] * (255-alpha) +
                        pbSrcRGB[0] * alpha)>>8;
                pbSrcRGB[1]=(pbDestRGB[1] * (255-alpha) +
                        pbSrcRGB[1] * alpha)>>8;
                pbSrcRGB[2]=(pbDestRGB[2] * (255-alpha) +
                        pbSrcRGB[2] * alpha)>>8;
            }
            else
            {    
                pbSrcRGB[0]=pbDestRGB[0];
                pbSrcRGB[1]=pbDestRGB[1];
                pbSrcRGB[2]=pbDestRGB[2];
            }
            
            pbSrcRGB += 4;
            pbDestRGB += 4;
        }
    }
    
    dc = CreateCompatibleDC(NULL);
    
    dcOld = (HBITMAP) SelectObject(dc, hbmSrc);
    if (!BitBlt(dcDest, x, y, cx, cy, dc, 0, 0, SRCCOPY))
        return false;
    DeleteDC(dc);
    
    DeleteObject(hbmSrc);    
    DeleteObject(hbmDest);    
    
    return true;
}
//End of File