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
  );

bool AlphaBlendU(HDC dcDest, int x, int y, int cx, int cy,
                 HDC dcSrc, int sx, int sy, int scx, int scy,
                 int alpha);

bool AlphaBlendCK(HDC dcDest, int x, int y, int cx, int cy,
                  HDC dcSrc, int sx, int sy, int scx, int scy,
                  int alpha, COLORREF rgbMask);
//End of File