#ifndef _RGBAPIXMAP
#define _RGBAPIXMAP
#include <string>
#include <fstream>
#include <assert.h>
#include <iostream>
#include "IntRect.h"
#include "IntPoint.h"

using namespace std;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
// typedef string ustring;
fstream inf; 			// global in this file for convenience

class mRGB			// the name RGB is already used by Windows
{
 	public:
		uchar r, g, b;
		mRGB( ) { r = g = b = 0; }
		mRGB(mRGB& p)  { r = p.r; g = p.g; b = p.b; }
		mRGB(uchar rr, uchar gg, uchar bb)  { r = rr; g = gg; b = bb; }
		void set(uchar rr, uchar gg, uchar bb) { r = rr; g = gg; b = bb; }
};

class mRGBA
{
	public:
		uchar r, g, b, a;
		mRGBA( ) { r = g = b = a = 0; }
		mRGBA(mRGBA& p)  { r = p.r; g = p.g; b = p.b; a = p.a; }
		mRGBA(mRGB& p, uchar aa) {  r = p.r; g = p.g; b = p.b; a = aa; }
		mRGBA(uchar rr, uchar gg, uchar bb, uchar aa)  { r = rr; g = gg; b = bb; a = aa; }
		void set(uchar rr, uchar gg, uchar bb, uchar aa) { r = rr; g = gg; b = bb; a = aa; }
};

//$$$$$$$$$$$$$$$$$ RGBAPixmap class $$$$$$$$$$$$$$$
class RGBApixmap{
private: 
	mRGB* pixel; 				// array of pixels
	mRGBA* apixel ;		 		// array of pixels

public:
	int nRows, nCols; 				// dimensions of the pixmap
	RGBApixmap() { nRows = nCols = 0; pixel = 0; apixel = 0; }
	RGBApixmap(int rows, int cols) 		//constructor
	{
		nRows = rows;
		nCols = cols;
		pixel = new mRGB[rows*cols]; 
		apixel = new mRGBA[rows*cols];
	}

//<<<<<<<<<<<<<<<< setAlpha >>>>>>>>>>>
void setAlpha(uchar alpha)
{ 		
	int rows, cols;
	long count = 0;
	for(rows = 0; rows < nRows; rows++)
	{
		for(cols = 0; cols < nCols; cols++)
		{
			apixel[count].a = alpha;
			count++;
		}
	}
}

//	int readBMPFile(string fname);			 // read BMP file into this pixmap
//<<<<<<<<<<<<<<<<<< RGBPixmap:: readBmpFile>>>>>>>>>>>>>
// int RGBApixmap:: readBMPFile(string fname) 
int readBMPFile(string fname) 
{  // Read into memory an mRGB image from an uncompressed BMP file.
	// return 0 on failure, 1 on success
	inf.open(fname.c_str(), ios::in|ios::binary); 		//read binary char's
	if(!inf){ cout << " can't open file: " << fname << endl; return 0;}
	int k, row, col, numPadBytes, nBytesInRow;
	// read the file header information
	char ch1, ch2;
	inf.get(ch1); inf.get(ch2); 				//type: always 'BM'
	ulong fileSize =      getLong();
	ushort reserved1 =    getShort();    			// always 0
	ushort reserved2= 	getShort();     			// always 0 
	ulong offBits =		getLong(); 		// offset to image - unreliable
	ulong headerSize =     getLong();     			// always 40
	ulong numCols =		getLong(); 		// number of columns in image
	ulong numRows = 		getLong(); 		// number of rows in image
	ushort planes=    	getShort();      			// always 1 
	ushort bitsPerPixel=   getShort();    			//8 or 24; allow 24 here
	ulong compression =    getLong();      			// must be 0 for uncompressed 
	ulong imageSize = 	getLong();       			// total bytes in image 
	ulong xPels =    	getLong();    			// always 0 
	ulong yPels =    	getLong();    			// always 0 
	ulong numLUTentries = getLong();   			 // 256 for 8 bit, otherwise 0 
	ulong impColors = 	getLong();       			// always 0 
	if(bitsPerPixel != 24) 					// error - must be a 24 bit uncompressed image
	{ 
		cout << "not a 24 bit/pixelimage, or is compressed!\n";
		inf.close(); return 0;
	} 
	//add bytes at end of each row so total # is a multiple of 4
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3)/4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; 		// need this many
	nRows = numRows; 					// set class's data members
	nCols = numCols;
	pixel = new mRGB[nRows * nCols]; 			//make space for array
	apixel = new mRGBA[nRows * nCols]; 			//make space for array
	if(!pixel) return 0; 					// out of memory!
	long count = 0;
	char dum;
	for(row = 0; row < nRows; row++) // read pixel values
	{
		for(col = 0; col < nCols; col++)
		{
			char r,g,b;
			inf.get(b); inf.get(g); inf.get(r); 		//read bytes
			apixel[count].r = r; 			//place them in colors
			apixel[count].g = g;
			apixel[count].b = b;
			apixel[count].a = 255;
			pixel[count].r = r; 			//place them in colors
			pixel[count].g = g;
			pixel[count].b = b;
			count++;
		}
   		for(k = 0; k < numPadBytes ; k++) //skip pad bytes at row's end
			inf >> dum;
	}
	inf.close(); return 1; 					// success
}

	void freeIt() 					// give back memory for this pixmap
	{
		delete []pixel;
		delete []apixel;
		nRows = nCols = 0;
	}
	//<<<<<<<<<<<<<<<<<< copy >>>>>>>>>>>>>>>>>>>
	void copy(IntPoint from, IntPoint to, int x, int y, int width, int height)
	{ // copy a region of the display back onto the display
		if(nRows == 0 || nCols == 0) return;
		glCopyPixels(x, y, width, height,GL_COLOR);
	}
	//<<<<<<<<<<<<<<<<<<< blend >>>>>>>>>>>>>>>>>
	void blend()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		adraw();
	}
	//<<<<<<<<<<<<<<<<<<< draw >>>>>>>>>>>>>>>>>
	void draw()
	{ // draw this pixmap at current raster position
		if(nRows == 0 || nCols == 0) return;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glDrawPixels(nCols, nRows,GL_RGB, GL_UNSIGNED_BYTE,pixel);
	}
	//<<<<<<<<<<<<<<<<<<< draw >>>>>>>>>>>>>>>>>
	void adraw()
	{ // draw this pixmap at current raster position
		if(nRows == 0 || nCols == 0) return;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glDrawPixels(nCols, nRows,GL_RGBA, GL_UNSIGNED_BYTE,apixel);
	}
	//<<<<<<<<<<<<<<<<< read >>>>>>>>>>>>>>>>
	int read(int x, int y, int wid, int ht)
	{ // read a rectangle of pixels into this pixmap
		nRows = ht;
		nCols = wid;
		pixel = new mRGB[nRows *nCols]; if(!pixel) return -1;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadPixels(x, y, nCols, nRows, GL_RGB,GL_UNSIGNED_BYTE,pixel);
		return 0;
	}
	//<<<<<<<<<<<<<<<<< read from IntRect >>>>>>>>>>>>>>>>
	int read(IntRect r)
	{ // read a rectangle of pixels into this pixmap
		nRows = r.t - r.b;
		nCols = r.r - r.l;
		pixel = new mRGB[nRows *nCols]; if(!pixel) return -1;
		//tell OpenGL: don’t align pixels to 4 byte boundaries in memory
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		glReadPixels(r.l,r.b, nCols, nRows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		return 0;
	}
	//<<<<<<<<<<<<<< setPixel >>>>>>>>>>>>>
	void setPixel(int x, int y, mRGB color)
	{
		if(x>=0 && x <nCols && y >=0 && y < nRows)
			pixel[nCols * y + x] = color;
	}
	//<<<<<<<<<<<<<<<< getPixel >>>>>>>>>>>
	mRGB getPixel(int x, int y)
	{ 		
		mRGB bad(255,255,255);
		assert(x >= 0 && x < nCols);
		assert(y >= 0 && y < nRows);
		return pixel[nCols * y + x];
	}
	//<<<<<<<<<<<<<<<< getPixela >>>>>>>>>>>
	mRGBA getPixela(int x, int y)
	{ 		
		mRGBA bad(255,255,255, 255);
		assert(x >= 0 && x < nCols);
		assert(y >= 0 && y < nRows);
		return apixel[nCols * y + x];
	}
	//<<<<<<<<<<<<<<<< setChromaKey >>>>>>>>>>>
	void setChromaKey(mRGBA c)
	{ 		
		long count = 0;
		for(int row = 0; row < nCols; row++)
		{
			for(int col=0; col < nRows; col++)
			{
				mRGBA p = apixel[count];
				if(p.r == c.r && p.g == c.g && p.b == c.b)
					apixel[count++].a = 0;
				else
					apixel[count++].a = 255;
			}
		}
	}
//<<<<<<<<<<<<<<<<<<<<< getShort >>>>>>>>>>>>>>>>>>>>
ushort getShort() //helper function
{ //BMP format uses little-endian integer types
  // get a 2-byte integer stored in little-endian form
		char ic;
		ushort ip;
		inf.get(ic); ip = ic;  //first byte is little one 
		inf.get(ic);  ip |= ((ushort)ic << 8); // or in high order byte
		return ip;
}
//<<<<<<<<<<<<<<<<<<<< getLong >>>>>>>>>>>>>>>>>>>
ulong getLong() //helper function
{  //BMP format uses little-endian integer types
   // get a 4-byte integer stored in little-endian form
		ulong ip = 0;
		char ic = 0;
		unsigned char uc = ic;
		inf.get(ic); uc = ic; ip = uc;
		inf.get(ic); uc = ic; ip |=((ulong)uc << 8);
		inf.get(ic); uc = ic; ip |=((ulong)uc << 16);
		inf.get(ic); uc = ic; ip |=((ulong)uc << 24);
		return ip;
	}
}; //end of class RGBApixmap
#endif