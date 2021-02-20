#include <sys/stat.h>
#include "PNG/PNG.h"
#include "BMP/BMP.h"
#include "Huffman/Huffman.h"
#include "LZSS/LZSS.h"

void printb(unsigned int v) {
  unsigned int mask = (int)1 << (sizeof(v) * CHAR_BIT - 1);
  do putchar(mask & v ? '1' : '0');
  while (mask >>= 1);
}

int main( int argc, char* argv[] )
{
	png::PNG _png;
	_png.load( "Assets/a.png" );
	_png.write( "Assets/output.png" );

	auto a = _png.getChunkData( png::ChunkType::IHDR );
	auto width = *( int* )a->getDataElement( ( int )png::Ihdr::DataType::WIDTH );

	bmp::BMP _bmp;
	_bmp.load( "Assets/input.bmp" );
	_bmp.write( "Assets/output.bmp" );

	//	PNGで使われている圧縮アルゴリズム（LZSS、Huffman）の実装

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
	std::vector<BYTE> compressedLZSSData;
	std::vector<BYTE> compressedHuffmanData;

	LZSS lzss;
	Huffman huffman;

	//	データ圧縮
	lzss.encode( rawData, compressedLZSSData );
	huffman.encode( compressedLZSSData, compressedHuffmanData );
	//huffman.encode( rawData, compressedHuffmanData );

	BYTE* output = compressedHuffmanData.data();

	FILE* fp2 = nullptr;
	if( fopen_s( &fp2, "Assets/compressedData.txt", "wb" ) != 0 )
	{
		return 0;
	}
	fwrite( output, compressedHuffmanData.size(), 1, fp2 );
	fclose( fp2 );

	//-------------------------------------------------------------------

	//	データ解凍
	rawData.clear();
	compressedLZSSData.clear();
	huffman.decode(compressedHuffmanData, compressedLZSSData);
	lzss.decode( compressedLZSSData, rawData );

	output = rawData.data();

	FILE* fp3 = nullptr;
	if( fopen_s( &fp3, "Assets/rawData.txt", "wb" ) != 0 )
	{
		return 0;
	}
	fwrite( output, rawData.size(), 1, fp3 );
	fclose( fp3 );

	//int compressedNum = 0;
	//int rawNum = 0;
	//for( int i = 0; i < compressedData.size(); i+=2 )
	//{
	//	if( compressedData[i] == 0x01 )
	//	{
	//		compressedNum++;
	//	}
	//	else
	//	{
	//		rawNum++;
	//	}
	//}
	//printf( "圧縮された数 : %d", compressedNum );
	//printf( "圧縮されなかった数 : %d", rawNum );

	//-------------------------------------------------------------------
	
	return 0;
}