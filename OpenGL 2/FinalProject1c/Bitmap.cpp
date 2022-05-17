#include <windows.h>
#include <stdio.h>

#include "bitmap.h"

BitmapFile::BitmapFile()
{
	data = 0;
	rgba_data = 0;
}

BitmapFile::~BitmapFile()
{
	if (data)
		delete data;
		
	if (rgba_data)
		delete rgba_data;
		
}

bool BitmapFile::load(char *filename, bool alpha)
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	FILE *fp;

	if (!(fp = fopen(filename,"rb")))
		return false;

	fread(&bfh,sizeof(BITMAPFILEHEADER),1,fp);
	fread(&bih,sizeof(BITMAPINFOHEADER),1,fp);

	w = bih.biWidth;
	h = bih.biHeight;
	size = w * h * 3;
	rgba_size = w * h * 4;

	if (data)
		delete data;
		
	data = new UCHAR[size];

	if (rgba_data)
		delete rgba_data;
		
	rgba_data = new UCHAR[rgba_size];
	if (!data)
	{
		fclose(fp);
		return false;
	}

	fread(data,size,1,fp);

	for (UINT i = 0; i < w * h; i++)
	{
			if(data[i*3] == 0 && data[i*3+1] == 0 && data[i*3+2] == 0)
                rgba_data[i*4+3] = 0;  
            else
                rgba_data[i*4+3] = 255;

			UCHAR temp = data[i*3];
			data[i*3] = data[i*3+2];
			data[i*3+2] = temp;
			
			if (alpha)
			{
				rgba_data[i*4] = data[i*3];
				rgba_data[i*4+1] = data[i*3+1];
				rgba_data[i*4+2] = data[i*3+2];
				rgba_data[i*4+3] = (data[i*3]+data[i*3+1]+data[i*3+2])/3;
			}
			else
			{
				rgba_data[i*4] = data[i*3];
				rgba_data[i*4+1] = data[i*3+1];
				rgba_data[i*4+2] = data[i*3+2];
				rgba_data[i*4+3] = (data[i*3]+data[i*3+1]+data[i*3+2])/3;
			}

	}

	fclose(fp);

	return true;
}