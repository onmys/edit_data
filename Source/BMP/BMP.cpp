#include "BMP.h"

namespace bmp
{
const int BMP::HEADER_SIZE = 14;

BMP::BMP()
{
}

BMP::~BMP()
{
}

bool BMP::load( std::string fileNamePath )
{
	this->fileNamePath = fileNamePath;
	FILE* fp = nullptr;

	if( fopen_s( &fp, this->fileNamePath.c_str(), "rb" ) != 0 )
	{
		printf( "%s‚ÍŠJ‚¯‚Ü‚¹‚ñ\n", this->fileNamePath );
		fclose( fp );
		return false;
	}

	if( !readFileHeader( fp ) )
	{
		fclose( fp );
		return false;
	}

	if( !readInfoHeader( fp ) )
	{
		fclose( fp );
		return false;
	}

	if( !readImage( fp ) )
	{
		fclose( fp );
		return false;
	}

	fclose( fp );
	return true;
}

bool BMP::write( std::string fileNamePath )
{
	FILE* fp = nullptr;

	if( fopen_s( &fp, fileNamePath.c_str(), "wb" ) != 0 )
	{
		printf( "%s‚ÍŠJ‚¯‚Ü‚¹‚ñ\n", fileNamePath );
		return false;
	}

	if( !writeFileHeader( fp ) )
	{
		fclose( fp );
		return false;
	}

	if( !writeInfoHeader( fp ) )
	{
		fclose( fp );
		return false;
	}

	if( !writeImage( fp ) )
	{
		fclose( fp );
		return false;
	}

	fclose( fp );
	return true;
}

void BMP::release()
{
}

bool BMP::readFileHeader( FILE* fp )
{
	fread_s( fileHeader.type, sizeof( BYTE ) * 2, sizeof( BYTE ) * 2, 1, fp );
	fread_s( &fileHeader.size, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( fileHeader.reservationArea, sizeof( WORD ) * 2, sizeof( WORD ) * 2, 1, fp );
	fread_s( &fileHeader.offset, sizeof( DWORD ), sizeof( DWORD ), 1, fp );

	if( fileHeader.type[0] != 'B' || fileHeader.type[1] != 'M' )
	{
		printf( "%s‚ÍBMP‚Å‚Í‚ ‚è‚Ü‚¹‚ñ\n", fileNamePath );
		return false;
	}

	return true;
}

bool BMP::readInfoHeader( FILE* fp )
{
	fread_s( &infoHeader.size, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.width, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.height, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.plane, sizeof( WORD ), sizeof( WORD ), 1, fp );
	fread_s( &infoHeader.bitCount, sizeof( WORD ), sizeof( WORD ), 1, fp );
	fread_s( &infoHeader.compressionFormat, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.imageSize, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.pixelPerMeterX, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.pixelPerMeterY, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.UseColorPalette, sizeof( DWORD ), sizeof( DWORD ), 1, fp );
	fread_s( &infoHeader.importantPaletteIndex, sizeof( DWORD ), sizeof( DWORD ), 1, fp );

	return true;
}

bool BMP::readImage( FILE* fp )
{
	int size = 0;

	if( infoHeader.UseColorPalette == 0 )
	{
		int width = infoHeader.width;
		int height = infoHeader.height;

		switch( ( BITMAP )infoHeader.bitCount )
		{
		case BITMAP::BitMap_1:
			size = width * height / 8 + 1;
			break;

		case BITMAP::BitMap_4:
			size = width * height / 4 + 1;
			break;

		case BITMAP::BitMap_8:
			size = width * height;
			break;

		case BITMAP::BitMap_24:
			size = width * height * 3;
			break;

		case BITMAP::BitMap_32:
			size = width * height * 4;
			break;
		}

		imageData = new BYTE[size];
	}
	else
	{
		size = infoHeader.UseColorPalette;
		colorPalette = new Colorpalette[size];

		for( int i = 0; i < size; i++ )
		{
			fread_s( &colorPalette[i], sizeof( BYTE ) * 4, sizeof( BYTE ) * 4, 1, fp );
		}

		size = infoHeader.width * infoHeader.height;
		imageData = new BYTE[size];
	}

	fread_s( imageData, size, size, 1, fp );

	return true;
}
bool BMP::writeFileHeader( FILE* fp )
{
	fwrite( fileHeader.type, sizeof( BYTE ) * 2, 1, fp );
	fwrite( &fileHeader.size, sizeof( DWORD ), 1, fp );
	fwrite( fileHeader.reservationArea, sizeof( WORD ) * 2, 1, fp );
	fwrite( &fileHeader.offset, sizeof( DWORD ), 1, fp );
	return true;
}
bool BMP::writeInfoHeader( FILE* fp )
{
	fwrite( &infoHeader.size, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.width, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.height, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.plane, sizeof( WORD ), 1, fp );
	fwrite( &infoHeader.bitCount, sizeof( WORD ), 1, fp );
	fwrite( &infoHeader.compressionFormat, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.imageSize, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.pixelPerMeterX, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.pixelPerMeterY, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.UseColorPalette, sizeof( DWORD ), 1, fp );
	fwrite( &infoHeader.importantPaletteIndex, sizeof( DWORD ), 1, fp );
	return true;
}
bool BMP::writeImage( FILE* fp )
{
	for( int i = 0; i < infoHeader.UseColorPalette; i++ )
	{
		fwrite( &colorPalette[i], sizeof( BYTE ) * 4, 1, fp );
	}

	int size = infoHeader.width * infoHeader.height;
	fwrite( imageData, sizeof( BYTE ) * size, 1, fp );
	return true;
}
}