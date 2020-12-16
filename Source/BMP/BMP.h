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
		BYTE type[2];	//	ファイルタイプ
		DWORD size;	//	ファイルサイズ
		WORD reservationArea[2];	//	予約領域
		DWORD offset;	//	ファイル先頭から画像データまでのオフセット
	};	//	合計 14 byte

	struct InfoHeader
	{
		DWORD size;	//	ヘッダサイズ
		DWORD width;	//	画像の幅
		DWORD height;	//	画像の高さ
		WORD plane;	//	プレーン数
		WORD bitCount;	//	1画素あたりのデータサイズ
		DWORD compressionFormat;	//	圧縮形式
		DWORD imageSize;	//	画像データ部のサイズ
		DWORD pixelPerMeterX;	//	X方向解像度 (1mあたりの画素数)
		DWORD pixelPerMeterY;	//	Y方向解像度 (1mあたりの画素数)
		DWORD UseColorPalette;	//	格納されているパレット数 (使用色数)
		DWORD importantPaletteIndex;	//	重要なパレットのインデックス
	};	//	合計 40 byte

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