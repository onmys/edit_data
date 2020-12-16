#include <sys/stat.h>
#include "PNG/PNG.h"
#include "BMP/BMP.h"
#include "LZSS/LZSS.h"

int main( int argc, char* argv[] )
{
	png::PNG _png;
	_png.load( "Assets/a.png" );
	_png.write( "Assets/output.png" );

	auto a = _png.getChunkData( png::ChunkType::IHDR );
	int width = *( int* )a->getDataElement( ( int )png::Ihdr::DataType::WIDTH );

	bmp::BMP _bmp;
	_bmp.load( "Assets/input.bmp" );
	_bmp.write( "Assets/output.bmp" );

	//-------------------------------------------------------------------

	//	ファイルサイズ取得
	int fileSize = 0;
	struct stat statBuf;
	if( stat( "Assets/input.txt", &statBuf ) == 0 )
	{
		fileSize = statBuf.st_size;
	}

	//	ファイル読み込み
	FILE* fp1 = nullptr;

	if( fopen_s( &fp1, "Assets/input.txt", "rb" ) != 0 )
	{
		return 0;
	}

	BYTE* data = new BYTE[fileSize];
	fread_s( data, sizeof( *data ) * fileSize, sizeof( *data ) * fileSize, 1, fp1 );
	fclose( fp1 );

	std::vector<BYTE> rawData( data, data + fileSize );
	std::vector<BYTE> compressedData;
	compressedData = {};

	LZSS lzss;

	//	データ圧縮
	lzss.encode( rawData, compressedData );

	BYTE* output = compressedData.data();

	FILE* fp2 = nullptr;
	if( fopen_s( &fp2, "Assets/compressedData.txt", "wb" ) != 0 )
	{
		return 0;
	}
	fwrite( output, compressedData.size(), 1, fp2 );
	fclose( fp2 );

	//	データ解凍
	rawData.clear();
	lzss.decode( compressedData, rawData );

	output = rawData.data();

	FILE* fp3 = nullptr;
	if( fopen_s( &fp3, "Assets/rawData.txt", "wb" ) != 0 )
	{
		return 0;
	}
	fwrite( output, rawData.size(), 1, fp3 );
	fclose( fp3 );

	int compressedNum = 0;
	int rawNum = 0;
	for( int i = 0; i < compressedData.size(); i+=2 )
	{
		if( compressedData[i] == 0x01 )
		{
			compressedNum++;
		}
		else
		{
			rawNum++;
		}
	}
	printf( "圧縮された数 : %d", compressedNum );
	printf( "圧縮されなかった数 : %d", rawNum );

	//-------------------------------------------------------------------
	
	return 0;
}

/*
// グレースケールに変換
	for( int j = 0; j < bitmap.height; j++ )
	{
		for( i = 0; i < bitmap.width; i++ )
		{
			sum = 0;
			for( int c = 0; c < bitmap.ch; c++ )
			{
				sum += bitmap.data[bitmap.ch * ( i + j * bitmap.width ) + c];
			}
			
			ave = sum / bitmap.ch;
		
			for( int c = 0; c < bitmap.ch; c++ )
			{
				bitmap.data[bitmap.ch * (i + j * bitmap.width) + c] = ave;
			}
		}
	}
*/

/*
//	エンディアンの確認
union
	{
		uint32_t b4;	// 4byte
		uint16_t b2[2];	// 2byte×2
		uint8_t b1[4];	// 1byte×4
	} bytes;

	 bytes.b4 = 0x12345678;
	 printf( "bytes.b4: %08X\n", bytes.b4 );
	 printf( "bytes.b2: %04X, %04X\n", bytes.b2[0], bytes.b2[1] );
	 printf( "bytes.b1: %02X, %02X, %02X, %02X\n", bytes.b1[0], bytes.b1[1], bytes.b1[2], bytes.b1[3] );

*/