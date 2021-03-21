#include <sys/stat.h>
#include "PNG/PNG.h"
#include "BMP/BMP.h"
#include "Huffman/Huffman.h"
#include "LZSS/LZSS.h"

void caseLZSS();
void caseHuffman();
void caseLZSSHuffman();
void caseHuffmanLZSS();

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

	//	PNG�Ŏg���Ă��鈳�k�A���S���Y���iLZSS�AHuffman�j�̎���
	enum class CompressedType
	{
		LZSS,
		HUFFMAN,
		LZSS_HUFFMAN,
		HUFFMAN_LZSS,
	} compressedType = CompressedType::LZSS_HUFFMAN;

	switch( compressedType )
	{
	case CompressedType::LZSS:
		caseLZSS();
		break;
	case CompressedType::HUFFMAN:
		caseHuffman();
		break;
	case CompressedType::LZSS_HUFFMAN:
		caseLZSSHuffman();
		break;
		case CompressedType::HUFFMAN_LZSS:
		caseHuffmanLZSS();
		break;
	}
	
	return 0;
}

void caseLZSS()
{
	//	�t�@�C���T�C�Y�擾
	int fileSize = 0;
	struct stat statBuf;
	if( stat( "Assets/input.txt", &statBuf ) == 0 )
	{
		fileSize = statBuf.st_size;
	}

	//	�t�@�C���ǂݍ���
	FILE* fp1 = nullptr;

	if( fopen_s( &fp1, "Assets/input.txt", "rb" ) != 0 )
	{
		return;
	}

	BYTE* data = new BYTE[fileSize];
	fread_s( data, sizeof( *data ) * fileSize, sizeof( *data ) * fileSize, 1, fp1 );
	fclose( fp1 );

	std::vector<BYTE> rawData( data, data + fileSize );
	std::vector<BYTE> compressedData;

	LZSS lzss;

	//	�f�[�^���k
	lzss.encode( rawData, compressedData );

	//	���k�����f�[�^���e�L�X�g�t�@�C���ɏo��
	BYTE* output = compressedData.data();
	FILE* fp2 = nullptr;
	if( fopen_s( &fp2, "Assets/compressedData.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedData.size(), 1, fp2 );
	fclose( fp2 );

	//	�f�[�^��
	rawData.clear();
	lzss.decode( compressedData, rawData );

	//	�𓀂����f�[�^���e�L�X�g�t�@�C���ɏo��
	output = rawData.data();
	FILE* fp3 = nullptr;
	if( fopen_s( &fp3, "Assets/rawData.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, rawData.size(), 1, fp3 );
	fclose( fp3 );
}

void caseHuffman()
{
	//	�t�@�C���T�C�Y�擾
	int fileSize = 0;
	struct stat statBuf;
	if( stat( "Assets/input.txt", &statBuf ) == 0 )
	{
		fileSize = statBuf.st_size;
	}

	//	�t�@�C���ǂݍ���
	FILE* fp1 = nullptr;

	if( fopen_s( &fp1, "Assets/input.txt", "rb" ) != 0 )
	{
		return;
	}

	BYTE* data = new BYTE[fileSize];
	fread_s( data, sizeof( *data ) * fileSize, sizeof( *data ) * fileSize, 1, fp1 );
	fclose( fp1 );

	std::vector<BYTE> rawData( data, data + fileSize );
	std::vector<BYTE> compressedData;

	Huffman huffman;

	//	�f�[�^���k
	huffman.encode( rawData, compressedData );

	//	���k�����f�[�^���e�L�X�g�t�@�C���ɏo��
	BYTE* output = compressedData.data();
	FILE* fp2 = nullptr;
	if( fopen_s( &fp2, "Assets/compressedData.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedData.size(), 1, fp2 );
	fclose( fp2 );

	//	�f�[�^��
	rawData.clear();
	huffman.decode(compressedData, rawData);

	//	�𓀂����f�[�^���e�L�X�g�t�@�C���ɏo��
	output = rawData.data();
	FILE* fp3 = nullptr;
	if( fopen_s( &fp3, "Assets/rawData.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, rawData.size(), 1, fp3 );
	fclose( fp3 );
}

void caseLZSSHuffman()
{
	//	�t�@�C���T�C�Y�擾
	int fileSize = 0;
	struct stat statBuf;
	if( stat( "Assets/input.txt", &statBuf ) == 0 )
	{
		fileSize = statBuf.st_size;
	}

	//	�t�@�C���ǂݍ���
	FILE* fp1 = nullptr;

	if( fopen_s( &fp1, "Assets/input.txt", "rb" ) != 0 )
	{
		return;
	}

	BYTE* data = new BYTE[fileSize];
	fread_s( data, sizeof( *data ) * fileSize, sizeof( *data ) * fileSize, 1, fp1 );
	fclose( fp1 );

	std::vector<BYTE> rawData( data, data + fileSize );
	std::vector<BYTE> compressedLZSSData;
	std::vector<BYTE> compressedHuffmanData;

	LZSS lzss;
	Huffman huffman;

	//	�f�[�^���k
	lzss.encode( rawData, compressedLZSSData );
	huffman.encode( compressedLZSSData, compressedHuffmanData );

	//	���k�����f�[�^���e�L�X�g�t�@�C���ɏo��
	BYTE* output = compressedHuffmanData.data();
	FILE* fpHuffman1 = nullptr;
	if( fopen_s( &fpHuffman1, "Assets/Confirm/compressedHuffman.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedHuffmanData.size(), 1, fpHuffman1 );
	fclose( fpHuffman1 );


	output = compressedLZSSData.data();
	FILE* fpLZSS1 = nullptr;
	if( fopen_s( &fpLZSS1, "Assets/Confirm/compressedLZSS.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedLZSSData.size(), 1, fpLZSS1 );
	fclose( fpLZSS1 );

	//	�f�[�^��
	rawData.clear();
	compressedLZSSData.clear();
	huffman.decode(compressedHuffmanData, compressedLZSSData);
	lzss.decode( compressedLZSSData, rawData );

	output = compressedLZSSData.data();
	FILE* fpLZSS2 = nullptr;
	if( fopen_s( &fpLZSS2, "Assets/Confirm/rawHuffman.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedLZSSData.size(), 1, fpLZSS2 );
	fclose( fpLZSS2 );

	//	�𓀂����f�[�^���e�L�X�g�t�@�C���ɏo��
	output = rawData.data();
	FILE* fp3 = nullptr;
	if( fopen_s( &fp3, "Assets/Confirm/rawLZSS.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, rawData.size(), 1, fp3 );
	fclose( fp3 );
}

void caseHuffmanLZSS()
{
	//	�t�@�C���T�C�Y�擾
	int fileSize = 0;
	struct stat statBuf;
	if( stat( "Assets/input.txt", &statBuf ) == 0 )
	{
		fileSize = statBuf.st_size;
	}

	//	�t�@�C���ǂݍ���
	FILE* fp1 = nullptr;

	if( fopen_s( &fp1, "Assets/input.txt", "rb" ) != 0 )
	{
		return;
	}

	BYTE* data = new BYTE[fileSize];
	fread_s( data, sizeof( *data ) * fileSize, sizeof( *data ) * fileSize, 1, fp1 );
	fclose( fp1 );

	std::vector<BYTE> rawData( data, data + fileSize );
	std::vector<BYTE> compressedLZSSData;
	std::vector<BYTE> compressedHuffmanData;

	LZSS lzss;
	Huffman huffman;

	//	�f�[�^���k
	huffman.encode( rawData, compressedHuffmanData );
	lzss.encode( compressedHuffmanData, compressedLZSSData );

	//	���k�����f�[�^���e�L�X�g�t�@�C���ɏo��
	BYTE* output = compressedHuffmanData.data();
	FILE* fpHuffman1 = nullptr;
	if( fopen_s( &fpHuffman1, "Assets/Confirm/compressedHuffman.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedHuffmanData.size(), 1, fpHuffman1 );
	fclose( fpHuffman1 );


	output = compressedLZSSData.data();
	FILE* fpLZSS1 = nullptr;
	if( fopen_s( &fpLZSS1, "Assets/Confirm/compressedLZSS.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedLZSSData.size(), 1, fpLZSS1 );
	fclose( fpLZSS1 );

	//	�f�[�^��
	rawData.clear();
	compressedHuffmanData.clear();
	lzss.decode( compressedLZSSData, compressedHuffmanData );
	huffman.decode(compressedHuffmanData, rawData);

	output = compressedHuffmanData.data();
	FILE* fpLZSS2 = nullptr;
	if( fopen_s( &fpLZSS2, "Assets/Confirm/rawLZSS.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, compressedHuffmanData.size(), 1, fpLZSS2 );
	fclose( fpLZSS2 );

	//	�𓀂����f�[�^���e�L�X�g�t�@�C���ɏo��
	output = rawData.data();
	FILE* fp3 = nullptr;
	if( fopen_s( &fp3, "Assets/Confirm/rawHuffman.txt", "wb" ) != 0 )
	{
		return;
	}
	fwrite( output, rawData.size(), 1, fp3 );
	fclose( fp3 );
}