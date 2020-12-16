#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <map>
#include <windows.h>

namespace bmp
{
class BMP
{
public:
	static const int HEADER_SIZE;

	enum class BITMAP
	{
		BitMap_1,
		BitMap_4,
		BitMap_8,
		BitMap_24,
		BitMap_32,
	};

	struct FileHeader
	{
		BYTE type[2];	//	�t�@�C���^�C�v
		DWORD size;	//	�t�@�C���T�C�Y
		WORD reservationArea[2];	//	�\��̈�
		DWORD offset;	//	�t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g
	};	//	���v 14 byte

	struct InfoHeader
	{
		DWORD size;	//	�w�b�_�T�C�Y
		DWORD width;	//	�摜�̕�
		DWORD height;	//	�摜�̍���
		WORD plane;	//	�v���[����
		WORD bitCount;	//	1��f������̃f�[�^�T�C�Y
		DWORD compressionFormat;	//	���k�`��
		DWORD imageSize;	//	�摜�f�[�^���̃T�C�Y
		DWORD pixelPerMeterX;	//	X�����𑜓x (1m������̉�f��)
		DWORD pixelPerMeterY;	//	Y�����𑜓x (1m������̉�f��)
		DWORD UseColorPalette;	//	�i�[����Ă���p���b�g�� (�g�p�F��)
		DWORD importantPaletteIndex;	//	�d�v�ȃp���b�g�̃C���f�b�N�X
	};	//	���v 40 byte

	struct Colorpalette
	{
		BYTE red, green, blue, reserved;
	};

public:
	BMP();
	~BMP();

	bool load( std::string fileNamePath );
	bool write( std::string fileNamePath );
	void release();

private:
	std::string fileNamePath;
	FileHeader fileHeader;
	InfoHeader infoHeader;
	Colorpalette* colorPalette;
	BYTE* imageData;

	bool readFileHeader( FILE* fp );
	bool readInfoHeader( FILE* fp );
	bool readImage( FILE* fp );

	bool writeFileHeader( FILE* fp );
	bool writeInfoHeader( FILE* fp );
	bool writeImage( FILE* fp );

};
}