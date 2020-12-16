#pragma once
#include <vector>
#include <windows.h>

class LZSS
{
public:
	//*********************************************************************************************
	//	@brief	データを圧縮
	//	@param rawData			:	圧縮するデータ
	//	@param compressedData	:	圧縮されたデータ
	//*********************************************************************************************
	bool encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData );

	//*********************************************************************************************
	//	@brief	データを解凍
	//	@param compressedData	:	圧縮されたデータ
	//	@param rawData			:	圧縮するデータ
	//*********************************************************************************************
	bool decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData );

private:
	const int LENGTH_MAX = 7;	//	エンコード最大文字数 19, 18
	const int LENGTH_MIN = 4;	//	エンコード最小文字数 4, 3
	const int LENGTH_SIZE = 3;	//	エンコード時の長さ(bit) 4
	const int POSITION_SIZE = 12 + 1;	//	エンコード時の位置(bit)
	const int REFERENCE_SIZE = 1 << POSITION_SIZE;	//	参照サイズ

	WORD encodeData;
};

