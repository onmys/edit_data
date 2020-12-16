#include "PNG.h"

//*********************************************************************************************
//	
//	PNGの読み込み書き込み
//	
//*********************************************************************************************
namespace png
{
z_stream zStream;

//	圧縮
bool compressionZlib( unsigned char* inputBuffer, const int inputBufferSize, unsigned char* outputBuffer, const int outputBufferSize )
{
	zStream.zalloc = Z_NULL;
	zStream.zfree = Z_NULL;
	zStream.opaque = Z_NULL;

	//	初期設定
	int result = deflateInit( &zStream, Z_DEFAULT_COMPRESSION );

	if( result != Z_OK )
	{
		printf( zStream.msg );
		return false;
	}

	//	入力、出力設定
	zStream.next_in = inputBuffer;
	zStream.avail_in = inputBufferSize;
	zStream.next_out = outputBuffer;
	zStream.avail_out = outputBufferSize;

	//	圧縮中
	do
	{
		result = deflate( &zStream, Z_FINISH );
	} while( result == Z_OK );

	if( result != Z_STREAM_END )
	{
		printf( zStream.msg );
		return false;
	}

	//	圧縮終了
	result = deflateEnd( &zStream );

	if( result != Z_OK )
	{
		printf( zStream.msg );
		return false;
	}

	return true;
}

//	解凍
bool uncompressionZlib( unsigned char* inputBuffer, const int inputBufferSize, unsigned char* outputBuffer, const int outputBufferSize )
{
	zStream.zalloc = Z_NULL;
	zStream.zfree = Z_NULL;
	zStream.opaque = Z_NULL;

	//	初期設定
	int result = inflateInit( &zStream );

	if( result != Z_OK )
	{
		printf( zStream.msg );
		return false;
	}

	//	入力、出力設定
	zStream.next_in = inputBuffer;
	zStream.avail_in = inputBufferSize;
	zStream.next_out = outputBuffer;
	zStream.avail_out = outputBufferSize;

	//	解凍中
	do
	{
		result = inflate( &zStream, Z_FINISH );
	} while( result == Z_OK );

	if( result != Z_STREAM_END )
	{
		printf( zStream.msg );
		return false;
	}

	//	解凍終了
	result = inflateEnd( &zStream );

	if( result != Z_OK )
	{
		printf( zStream.msg );
		return false;
	}

	return true;
}

int flagOnCount( int bitFlag )
{
	bitFlag = ( bitFlag & 0x55555555 ) + ( bitFlag >>  1 & 0x55555555 );
	bitFlag = ( bitFlag & 0x33333333 ) + ( bitFlag >>  2 & 0x33333333 );
	bitFlag = ( bitFlag & 0x0f0f0f0f ) + ( bitFlag >>  4 & 0x0f0f0f0f );
	bitFlag = ( bitFlag & 0x00ff00ff ) + ( bitFlag >>  8 & 0x00ff00ff );
	bitFlag = ( bitFlag & 0x0000ffff ) + ( bitFlag >> 16 & 0x0000ffff );
	return bitFlag;
}

unsigned int ConvertEndian( unsigned int endian )
{
	return	( ( endian & 0xFF ) << 24 ) | ( ( endian & 0xFF00 ) << 8 ) | 
			( ( endian & 0xFF0000 ) >> 8 ) | ( ( endian & 0xFF000000 ) >> 24 );
}

//*********************************************************************************************
//	
//	チャンクベースクラス
//	
//*********************************************************************************************
ChunkBase::ChunkBase( const char* chunkType, unsigned int dataSize )
{
	_length = 0;
	_chunkType[0] = chunkType[0];	_chunkType[1] = chunkType[1];
	_chunkType[2] = chunkType[2];	_chunkType[3] = chunkType[3];

	if( dataSize > 0 )
	{
		_data = new char[dataSize];
	}

	_crc = 0;
}

ChunkBase::ChunkBase() : ChunkBase( "", 1  )
{
}

ChunkBase::~ChunkBase()
{
	if( _data )
	{
		delete _data;
	}
}

unsigned int ChunkBase::getDataSize()
{
	return	ConvertEndian( _length ) ;
}

unsigned int ChunkBase::getSize()
{
	return _length + sizeof( _length ) + sizeof( _chunkType ) + sizeof( _crc );
}

bool ChunkBase::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	for( unsigned int i = 0; i < dataSize; i++ )
	{
		_data[i] = data[i];
	}
	_crc = crc;
	_length = ConvertEndian( dataSize );

	return true;
}

//*********************************************************************************************
//	
//	チャンクIHDRクラス
//	
//*********************************************************************************************
const unsigned int Ihdr::CHUNK_DATA_SIZE = 13;

Ihdr::Ihdr( const char* data, unsigned int crc ) : ChunkBase( "IHDR", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

Ihdr::Ihdr( unsigned int width, unsigned int height, unsigned char bitDepth, unsigned char colorType, unsigned char compression, unsigned char filter, unsigned char interlace, unsigned int crc ) : ChunkBase( "IHDR", CHUNK_DATA_SIZE )
{
	char data[CHUNK_DATA_SIZE];
	data[0] = ( width >> 24 )	& 0xFF;
	data[1] = ( width >> 16 )	& 0xFF;
	data[2] = ( width >>  8 )	& 0xFF;
	data[3] =   width			& 0xFF;
	data[4] = ( height >> 24 )	& 0xFF;
	data[5] = ( height >> 16 )	& 0xFF;
	data[6] = ( height >>  8 )	& 0xFF;
	data[7] =   height			& 0xFF;
	data[8] = bitDepth;
	data[9] = colorType;
	data[10] = compression;
	data[11] = filter;
	data[12] = interlace;

	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Ihdr::getDataElement( int dataType )
{
	DataType type = ( DataType )dataType;

	if( type < DataType::MIN || type > DataType::MAX )
	{
		return nullptr;
	}

	switch( type )
	{
	case DataType::WIDTH:
	{
		int width = ( ( _data[3] ) | ( _data[2] << 8 ) | ( _data[1] << 16 ) | ( _data[0] << 24 ) );
		return &width;
	}
	
	case DataType::HEIGHT:
	{
		int height = ( ( _data[7] ) | ( _data[6] << 8 ) | ( _data[5] << 16 ) | ( _data[4] << 24 ) );
		return &height;
	}
	case DataType::BIT_DEPTH:
		return &_data[8];
	case DataType::COLOR_TYPE:
		return &_data[9];
	case DataType::COMPRESSION:
		return &_data[10];
	case DataType::FILTER:
		return &_data[11];
	case DataType::INTERLACE:
		return &_data[12];
	default:
		return nullptr;
	}
}

bool Ihdr::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	if( !checkBitDepth( data[8], data[9] ) )
	{
		//	警告：許可されていない組み合わせ
		return false;
	}

	return ChunkBase::setData( data, dataSize, crc );
}

bool Ihdr::checkBitDepth( char bitDepth, char colorType )
{
	return ( flagOnCount( bitDepth ) == 1 ) && 
		( permissionBitDepth.at( (ColorType)colorType ) & bitDepth );
}

//*********************************************************************************************
//	
//	チャンクGAMAクラス
//	
//*********************************************************************************************
const unsigned int Gama::CHUNK_DATA_SIZE = 4;

Gama::Gama( const char* data, unsigned int crc ) : ChunkBase( "gAMA", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Gama::getDataElement( int dataType )
{
	return nullptr;
}

bool Gama::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクCHRMクラス
//	
//*********************************************************************************************
const unsigned int Chrm::CHUNK_DATA_SIZE = 32;

Chrm::Chrm( const char* data, unsigned int crc ) : ChunkBase( "cHRM", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Chrm::getDataElement( int dataType )
{
	return nullptr;
}

bool Chrm::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクSRGBクラス
//	
//*********************************************************************************************
const unsigned int Srgb::CHUNK_DATA_SIZE = 1;

Srgb::Srgb( const char* data, unsigned int crc ) : ChunkBase( "sRGB", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Srgb::getDataElement( int dataType )
{
	return nullptr;
}

bool Srgb::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクICCPクラス
//	
//*********************************************************************************************
Iccp::Iccp( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "iCCP", dataSize )
{
	setData( data, dataSize, crc );
}

void* Iccp::getDataElement( int dataType )
{
	return nullptr;
}

bool Iccp::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクSBITクラス
//	
//*********************************************************************************************
const unsigned int Sbit::CHUNK_DATA_SIZE = 3;

Sbit::Sbit( const char* data, unsigned int crc ) : ChunkBase( "sBIT", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Sbit::getDataElement( int dataType )
{
	return nullptr;
}

bool Sbit::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクPLTEクラス
//	
//*********************************************************************************************
const int Plte::COLOR_NUM = 3;

Plte::Plte( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "PLTE", dataSize )
{
	setData( data, dataSize, crc );
}

void* Plte::getDataElement( int dataElement )
{
	return &_data[dataElement * COLOR_NUM];
}

bool Plte::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	if( dataSize % COLOR_NUM != 0 )
	{
		//	警告：サイズが3の倍数じゃない
		return false;
	}

	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクTRNSクラス
//	
//*********************************************************************************************
Trns::Trns( const char* data, unsigned int dataSize, unsigned int crc, char colorType ) : ChunkBase( "tRNS", dataSize )
{
	_colorType = colorType;
	setData( data, dataSize, crc );
}

void* Trns::getDataElement( int dataType )
{
	return nullptr;
}

bool Trns::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクBKGDクラス
//	
//*********************************************************************************************
Bkgd::Bkgd( const char* data, unsigned int crc ) : ChunkBase( "bKGD", 1 )
{
	setData( data, 1, crc );
}

void* Bkgd::getDataElement( int dataType )
{
	return nullptr;
}

bool Bkgd::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクPHYSクラス
//	
//*********************************************************************************************
const unsigned int Phys::CHUNK_DATA_SIZE = 9;

Phys::Phys( const char* data, unsigned int crc ) : ChunkBase( "pHYs", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Phys::getDataElement( int dataType )
{
	return nullptr;
}

bool Phys::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクSPLTクラス
//	
//*********************************************************************************************
Splt::Splt( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "sPLT", dataSize )
{
	setData( data, dataSize, crc );
}

void* Splt::getDataElement( int dataType )
{
	return nullptr;
}

bool Splt::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクHISTクラス
//	
//*********************************************************************************************
Hist::Hist( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "hIST", dataSize )
{
	setData( data, dataSize, crc );
}

void* Hist::getDataElement( int dataElement )
{
	return nullptr;
}

bool Hist::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクIDATクラス
//	
//*********************************************************************************************
Idat::Idat( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "IDAT", dataSize )
{
	setData( data, dataSize, crc );
}

void* Idat::getDataElement( int dataElement )
{
	return &_data[dataElement];
}

//*********************************************************************************************
//	
//	チャンクTEXTクラス
//	
//*********************************************************************************************
Text::Text( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "tEXT", dataSize )
{
	setData( data, dataSize, crc );
}

void* Text::getDataElement( int dataType )
{
	return nullptr;
}

bool Text::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクZTXTクラス
//	
//*********************************************************************************************
Ztxt::Ztxt( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "zTXT", dataSize )
{
	setData( data, dataSize, crc );
}

void* Ztxt::getDataElement( int dataType )
{
	return nullptr;
}

bool Ztxt::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクITXTクラス
//	
//*********************************************************************************************
Itxt::Itxt( const char* data, unsigned int dataSize, unsigned int crc ) : ChunkBase( "iTXT", dataSize )
{
	setData( data, dataSize, crc );
}

void* Itxt::getDataElement( int dataType )
{
	return nullptr;
}

bool Itxt::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクTIMEクラス
//	
//*********************************************************************************************
const unsigned int Time::CHUNK_DATA_SIZE = 7;

Time::Time( const char* data, unsigned int crc ) : ChunkBase( "tIME", CHUNK_DATA_SIZE )
{
	setData( data, CHUNK_DATA_SIZE, crc );
}

void* Time::getDataElement( int dataType )
{
	return nullptr;
}

bool Time::setData( const char* data, unsigned int dataSize, unsigned int crc )
{
	return ChunkBase::setData( data, dataSize, crc );
}

//*********************************************************************************************
//	
//	チャンクIENDクラス
//	
//*********************************************************************************************
Iend::Iend( unsigned int crc ) : ChunkBase( "IEND", 0 )
{
	setData( nullptr, 0, crc );
}

void* Iend::getDataElement( int dataType )
{
	return nullptr;
}

//*********************************************************************************************
//	
//	PNGクラス
//	
//*********************************************************************************************
PNG::PNG()
{
	_chunkMap = {};
	ihdr = nullptr;
	iend = nullptr;
	trns = nullptr;
	gama = nullptr;
	chrm = nullptr;
	srgb = nullptr;
	iccp = nullptr;
	text = {};
	ztxt = {};
	itxt = {};
	bkgd = nullptr;
	phys = nullptr;
	sbit = nullptr;
	splt = {};
	hist = nullptr;
	time = nullptr;
	zStream = {};
}

PNG::~PNG()
{
	release();
}

bool PNG::load( const char* fileNamePath )
{
	FILE* fp = nullptr;

	if( fopen_s( &fp, fileNamePath, "rb" ) != 0 )
	{
		printf( "%sは開けません\n", fileNamePath );
		return false;
	}

	if( !readPNG( fp ) )
	{
		printf( "%sはPNGではありません\n", fileNamePath );
		return false;
	}

	int length = 0;
	char chunkType[4] = {};
	char* data = nullptr;
	int crc = 0;

	while( true )
	{
		if( data )
		{
			delete[] data;
			data = nullptr;
		}

		fread_s( &length, sizeof( length ), sizeof( length ), 1, fp );
		fread_s( chunkType, sizeof( chunkType ), sizeof( chunkType ), 1, fp );

		length = ConvertEndian( length );
		if( length >= 0 )
		{
			data = new char[length];
			fread_s( data, sizeof( *data ) * length, sizeof( *data ) * length, 1, fp );
		}
		fread_s( &crc, sizeof( crc ), sizeof( crc ), 1, fp );

		if( !strncmp( chunkType, "IHDR", 4 ) )
		{
			readIHDR( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "PLTE", 4 ) )
		{
			readPLTE( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "IDAT", 4 ) )
		{
			readIDAT( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "IEND", 4 ) )
		{
			readIEND( fp, data, length, crc );
			break;
		}
		else if( !strncmp( chunkType, "tRNS", 4 ) )
		{
			readTRNS( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "gAMA", 4 ) )
		{
			readGAMA( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "cHRM", 4 ) )
		{
			readCHRM( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "sRGB", 4 ) )
		{
			readSRGB( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "iCCP", 4 ) )
		{
			readICCP( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "tEXT", 4 ) )
		{
			readTEXT( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "zTXT", 4 ) )
		{
			readZTXT( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "iTXT", 4 ) )
		{
			readITXT( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "bKGD", 4 ) )
		{
			readBKGD( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "pHYs", 4 ) )
		{
			readPHYS( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "sBIT", 4 ) )
		{
			readSBIT( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "sPLT", 4 ) )
		{
			readSPLT( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "hIST", 4 ) )
		{
			readHIST( fp, data, length, crc );
		}
		else if( !strncmp( chunkType, "tIME", 4 ) )
		{
			readTIME( fp, data, length, crc );
		}
		else
		{
			printf( "Chunk Type [ %s ] は存在しません\n", chunkType );
			return false;
		}
	}

	fclose( fp );

	return true;
}

bool PNG::write( const char* fileNamePath )
{
	FILE* fp = nullptr;

	if( fopen_s( &fp, fileNamePath, "wb" ) != 0 )
	{
		printf( "%sは開けません\n", fileNamePath );
		return false;
	}

	writePNG( fp );
	writeIHDR( fp );
	writeGAMA( fp );
	writeCHRM( fp );
	writeSRGB( fp );
	writeICCP( fp );
	writeSBIT( fp );
	writePLTE( fp );
	writeTRNS( fp );
	writeBKGD( fp );
	writePHYS( fp );
	writeSPLT( fp );
	writeHIST( fp );
	writeIDAT( fp );
	writeTEXT( fp );
	writeZTXT( fp );
	writeITXT( fp );
	writeTIME( fp );
	writeIEND( fp );

	fclose( fp );

	return true;
}

void PNG::release()
{
	for( auto chunkMap : _chunkMap )
	{
		for( auto chunkVector : chunkMap.second )
		{
			if( chunkVector )
			{
				delete chunkVector;
			}
		}
	}
}

void PNG::setChunkData( ChunkType chunkType, const char* data, unsigned int crc, unsigned int dataSize )
{
	ChunkBase* chunk = nullptr;

	switch( chunkType )
	{
	case ChunkType::IHDR: ihdr = new Ihdr( data, crc ); break;
	case ChunkType::PLTE:	break;
	case ChunkType::IDAT: idat.push_back( new Idat( data, dataSize, crc ) ); break;
	case ChunkType::IEND:	break;
	case ChunkType::TRNS: trns = new Trns( data, dataSize, crc, *( char* )ihdr->getDataElement( ( int )Ihdr::DataType::COLOR_TYPE ) ); break;
	case ChunkType::GAMA: gama = new Gama( data, crc ); break;
	case ChunkType::CHRM: chrm = new Chrm( data, crc ); break;
	case ChunkType::SRGB: srgb = new Srgb( data, crc ); break;
	case ChunkType::ICCP: iccp = new Iccp( data, dataSize, crc ); break;
	case ChunkType::TEXT: text.push_back( new Text( data, dataSize, crc ) ); break;
	case ChunkType::ZTXT: ztxt.push_back( new Ztxt( data, dataSize, crc ) ); break;
	case ChunkType::ITXT: itxt.push_back( new Itxt( data, dataSize, crc ) ); break;
	case ChunkType::BKGD: bkgd = new Bkgd( data, crc ); break;
	case ChunkType::PHYS: phys = new Phys( data, crc ); break;
	case ChunkType::SBIT: sbit = new Sbit( data, crc ); break;
	case ChunkType::SPLT: splt.push_back( new Splt( data, dataSize, crc ) ); break;
	case ChunkType::HIST: hist = new Hist( data, dataSize, crc ); break;
	case ChunkType::TIME: time = new Time( data, crc ); break;
	default: return;
	}

	_chunkMap[chunkType].push_back( chunk );
}

bool PNG::readPNG( FILE* fp )
{
	char png[PNG_HEADER_NUM];
	char pngHeader[PNG_HEADER_NUM] =
	{
		( char )0x89,( char )0x50,( char )0x4E,( char )0x47,( char )0x0D,( char )0x0A,( char )0x1A,( char )0x0A
	};

	fread_s( png, sizeof( char ) * PNG_HEADER_NUM, sizeof( char ) * PNG_HEADER_NUM, 1, fp );

	for( int i = 0; i < PNG_HEADER_NUM; i++ )
	{
		if( png[i] != pngHeader[i] )
		{
			return false;
		}
	}

	return true;
}

bool PNG::readIHDR( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	ihdr = new Ihdr( data, crc );
	_chunkMap[ChunkType::IHDR].push_back( ihdr );

	return true;
}

bool PNG::readPLTE( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	plte = new Plte( data, dataSize, crc );
	_chunkMap[ChunkType::PLTE].push_back( plte );

	return false;
}

bool PNG::readIDAT( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	idat.push_back( new Idat( data, dataSize, crc ) );
	_chunkMap[ChunkType::PLTE].push_back( idat.back() );

	return true;
}

bool PNG::readIEND( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	iend = new Iend( crc );
	_chunkMap[ChunkType::IEND].push_back( iend );

	return true;
}

bool PNG::readTRNS( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	trns = new Trns( data, dataSize, crc, *( char* )ihdr->getDataElement( ( int )Ihdr::DataType::COLOR_TYPE ) );
	_chunkMap[ChunkType::TRNS].push_back( trns );

	return true;
}

bool PNG::readGAMA( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	gama = new Gama( data, crc );
	_chunkMap[ChunkType::GAMA].push_back( gama );

	return true;
}

bool PNG::readCHRM( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	chrm = new Chrm( data, crc );
	_chunkMap[ChunkType::CHRM].push_back( chrm );

	return true;
}

bool PNG::readSRGB( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	srgb = new Srgb( data, crc );
	_chunkMap[ChunkType::SRGB].push_back( srgb );

	return true;
}

bool PNG::readICCP( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	iccp = new Iccp( data, dataSize, crc );
	_chunkMap[ChunkType::ICCP].push_back( iccp );

	return true;
}

bool PNG::readTEXT( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	text.push_back( new Text( data, dataSize,  crc ) );
	_chunkMap[ChunkType::TEXT].push_back( text.back() );
	
	return true;
}

bool PNG::readZTXT( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	ztxt.push_back( new Ztxt( data, dataSize, crc ) );
	_chunkMap[ChunkType::ZTXT].push_back( ztxt.back() );
	
	return true;
}

bool PNG::readITXT( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	itxt.push_back( new Itxt( data, dataSize, crc ) );
	_chunkMap[ChunkType::ITXT].push_back( itxt.back() );
	
	return true;
}

bool PNG::readBKGD( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	bkgd = new Bkgd( data, crc );
	_chunkMap[ChunkType::BKGD].push_back( bkgd );
	

	return true;
}

bool PNG::readPHYS( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	phys = new Phys( data, crc );
	_chunkMap[ChunkType::PHYS].push_back( phys );

	return true;
}

bool PNG::readSBIT( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	sbit = new Sbit( data, crc );
	_chunkMap[ChunkType::SBIT].push_back( sbit );

	return true;
}

bool PNG::readSPLT( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	splt.push_back( new Splt( data, dataSize, crc ) );
	_chunkMap[ChunkType::SPLT].push_back( splt.back() );

	return true;
}

bool PNG::readHIST( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	hist = new Hist( data, dataSize, crc );
	_chunkMap[ChunkType::HIST].push_back( hist );

	return true;
}

bool PNG::readTIME( FILE* fp, char* data, unsigned int dataSize, int crc )
{
	time = new Time( data, crc );
	_chunkMap[ChunkType::TIME].push_back( time );

	return true;
}

bool PNG::writePNG( FILE* fp )
{
	char pngHeader[PNG_HEADER_NUM] =
	{
		( char )0x89,( char )0x50,( char )0x4E,( char )0x47,( char )0x0D,( char )0x0A,( char )0x1A,( char )0x0A
	};

	return ( fwrite( pngHeader, sizeof( pngHeader ), 1, fp ) >= sizeof( pngHeader ) );
}

bool PNG::writeIHDR( FILE* fp )
{
	if( !ihdr )
	{
		return false;
	}

	int dataSize = ConvertEndian( ihdr->getDataSize() );
	int crc = ihdr->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( ihdr->getChunkType(), 4, 1, fp );
	fwrite( ihdr->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writePLTE( FILE* fp )
{
	if( plte == nullptr )
	{
		return false;
	}

	int dataSize = ConvertEndian( plte->getDataSize() );
	int crc = plte->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( plte->getChunkType(), 4, 1, fp );
	fwrite( plte->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeIDAT( FILE* fp )
{
	if( idat.size() <= 0 )
	{
		return false;
	}

	for( int i = 0; i < idat.size(); i++ )
	{
		int dataSize = ConvertEndian( idat[i]->getDataSize() );
		int crc = idat[i]->getCRC();

		fwrite( &dataSize, sizeof( dataSize ), 1, fp );
		fwrite( idat[i]->getChunkType(), 4, 1, fp );
		fwrite( idat[i]->getData(), ConvertEndian( dataSize ), 1, fp );
		fwrite( &crc, sizeof( crc ), 1, fp );
	}

	return true;
}

bool PNG::writeIEND( FILE* fp )
{
	if( iend == nullptr )
	{
		return false;
	}

	int dataSize = ConvertEndian( iend->getDataSize() );
	int crc = iend->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( iend->getChunkType(), 4, 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeTRNS( FILE* fp )
{
	if( trns == nullptr )
	{
		return false;
	}

	int dataSize = ConvertEndian( trns->getDataSize() );
	int crc = trns->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( trns->getChunkType(), 4, 1, fp );
	fwrite( trns->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeGAMA( FILE* fp )
{
	if( gama == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( gama->getDataSize() );
	int crc = gama->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( gama->getChunkType(), 4, 1, fp );
	fwrite( gama->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeCHRM( FILE* fp )
{
	if( chrm == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( chrm->getDataSize() );
	int crc = chrm->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( chrm->getChunkType(), 4, 1, fp );
	fwrite( chrm->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeSRGB( FILE* fp )
{
	if( srgb == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( srgb->getDataSize() );
	int crc = srgb->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( srgb->getChunkType(), 4, 1, fp );
	fwrite( srgb->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeICCP( FILE* fp )
{
	if( iccp == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( iccp->getDataSize() );
	int crc = iccp->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( iccp->getChunkType(), 4, 1, fp );
	fwrite( iccp->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeTEXT( FILE* fp )
{
	if( text.size() <= 0 )
	{
		return false;
	}

	for( int i = 0; i < text.size(); i++ )
	{
		int dataSize = ConvertEndian( text[i]->getDataSize() );
		int crc = text[i]->getCRC();

		fwrite( &dataSize, sizeof( dataSize ), 1, fp );
		fwrite( text[i]->getChunkType(), 4, 1, fp );
		fwrite( text[i]->getData(), ConvertEndian( dataSize ), 1, fp );
		fwrite( &crc, sizeof( crc ), 1, fp );
	}

	return true;
}

bool PNG::writeZTXT( FILE* fp )
{
	if( ztxt.size() <= 0 )
	{
		return false;
	}

	for( int i = 0; i < ztxt.size(); i++ )
	{
		int dataSize = ConvertEndian( ztxt[i]->getDataSize() );
		int crc = ztxt[i]->getCRC();

		fwrite( &dataSize, sizeof( dataSize ), 1, fp );
		fwrite( ztxt[i]->getChunkType(), 4, 1, fp );
		fwrite( ztxt[i]->getData(), ConvertEndian( dataSize ), 1, fp );
		fwrite( &crc, sizeof( crc ), 1, fp );
	}

	return true;
}

bool PNG::writeITXT( FILE* fp )
{
	if( itxt.size() <= 0 )
	{
		return false;
	}

	for( int i = 0; i < itxt.size(); i++ )
	{
		int dataSize = ConvertEndian( itxt[i]->getDataSize() );
		int crc = itxt[i]->getCRC();

		fwrite( &dataSize, sizeof( dataSize ), 1, fp );
		fwrite( itxt[i]->getChunkType(), 4, 1, fp );
		fwrite( itxt[i]->getData(), ConvertEndian( dataSize ), 1, fp );
		fwrite( &crc, sizeof( crc ), 1, fp );
	}

	return true;
}

bool PNG::writeBKGD( FILE* fp )
{
	if( bkgd == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( bkgd->getDataSize() );
	int crc = bkgd->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( bkgd->getChunkType(), 4, 1, fp );
	fwrite( bkgd->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writePHYS( FILE* fp )
{
	if( phys == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( phys->getDataSize() );
	int crc = phys->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( phys->getChunkType(), 4, 1, fp );
	fwrite( phys->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeSBIT( FILE* fp )
{
	if( sbit == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( sbit->getDataSize() );
	int crc = sbit->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( sbit->getChunkType(), 4, 1, fp );
	fwrite( sbit->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeSPLT( FILE* fp )
{
	if( splt.size() <= 0 )
	{
		return false;
	}

	for( int i = 0; i < splt.size(); i++ )
	{
		int dataSize = ConvertEndian( splt[i]->getDataSize() );
		int crc = splt[i]->getCRC();

		fwrite( &dataSize, sizeof( dataSize ), 1, fp );
		fwrite( splt[i]->getChunkType(), 4, 1, fp );
		fwrite( splt[i]->getData(), ConvertEndian( dataSize ), 1, fp );
		fwrite( &crc, sizeof( crc ), 1, fp );
	}

	return true;
}

bool PNG::writeHIST( FILE* fp )
{
	if( hist == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( hist->getDataSize() );
	int crc = hist->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( hist->getChunkType(), 4, 1, fp );
	fwrite( hist->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}

bool PNG::writeTIME( FILE* fp )
{
	if( time == nullptr )
	{
		return true;
	}

	int dataSize = ConvertEndian( time->getDataSize() );
	int crc = time->getCRC();

	fwrite( &dataSize, sizeof( dataSize ), 1, fp );
	fwrite( time->getChunkType(), 4, 1, fp );
	fwrite( time->getData(), ConvertEndian( dataSize ), 1, fp );
	fwrite( &crc, sizeof( crc ), 1, fp );

	return true;
}
}