#ifndef _BITMAP__H_
#define _BITMAP__H_

//24 bit .bmp file
struct BitmapFile
{
	unsigned char *data;
	unsigned char *rgba_data;
	unsigned int w,h,size,rgba_size;

	BitmapFile();
	~BitmapFile();
	bool load(char *, bool alpha = true);
};

#endif