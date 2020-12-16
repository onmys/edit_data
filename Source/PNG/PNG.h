#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <zlib.h>

#pragma comment(lib, "zlibd.lib")

//*********************************************************************************************
//	
//	pngを自力で読み込み書き込み
//	
//*********************************************************************************************
namespace png
{
#define PNG_HEADER_NUM	8

typedef unsigned char Byte;

bool readPNG( const char* fileName );
bool writePNG( const char* fileName );

//	ビットが立っている数
int flagOnCount( int bitFlag );

unsigned int ConvertEndian( unsigned int endian );

//	チャンクタイプ
enum class ChunkType
{
	IHDR,
	GAMA,
	CHRM,
	SRGB,
	ICCP,
	SBIT,
	PLTE,
	TRNS,
	BKGD,
	PHYS,
	SPLT,
	HIST,
	IDAT,
	TEXT,
	ZTXT,
	ITXT,
	TIME,
	IEND,
	MIN = IHDR,
	MAX = TIME
};

//*********************************************************************************************
//	
//	チャンクベースクラス
//	
//*********************************************************************************************
class ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@oaram	length		:	データのサイズ　※ビッグエンディアン
	//	@oaram	chunkType	:	チャンクタイプ
	//	@oaram	dataSize	:	データのサイズ
	//*********************************************************************************************
	ChunkBase( const char* chunkType, unsigned int dataSize );
	ChunkBase();
	virtual ~ChunkBase();

	//	チャンクタイプを取得
	char* getChunkType() { return _chunkType; }
	//	データを取得
	char* getData() { return _data; }
	//	データのサイズを取得
	unsigned int getDataSize();
	//	チャンクのサイズを取得
	unsigned int getSize();
	//	エラーチェック値を取得
	unsigned int getCRC() { return _crc; }
	//	データの取得
	virtual void* getDataElement( int dataType ) = 0;

protected:
	unsigned int _length;	//	データのサイズ ※ビッグエンディアンなので注意
	char _chunkType[4];		//	チャンクタイプ
	char* _data;		//	データのポインタ
	unsigned int _crc;		//	エラーチェック


	virtual bool setData( const char* data, unsigned int dataSize, unsigned int crc );
};

//*********************************************************************************************
//	
//	チャンクIHDRクラス
//	イメージヘッダ
//	
//*********************************************************************************************
class Ihdr : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class ColorType
	{
		GRAY_SCALE			=	0,	//	グレースケール画像
		TRUE_COLOR			=	2,	//	トゥルーカラー画像
		INDEX_COLOR			=	3,	//	インデックスカラー画像
		GRAY_SCALE_ALPHA	=	4,	//	グレースケール＋アルファ画像
		TRUE_COLOR_ALPHA	=	6,	//	トゥルーカラー＋アルファ画像
	};

	//	使えるビット深度
	const std::map<ColorType, char> permissionBitDepth = 
	{
		{ ColorType::GRAY_SCALE,		0x1F },
		{ ColorType::TRUE_COLOR,		0x18 },
		{ ColorType::INDEX_COLOR,		0xF  },
		{ ColorType::GRAY_SCALE_ALPHA,	0x18 },
		{ ColorType::TRUE_COLOR_ALPHA,	0x18 },
	};

	//	データ取得用
	enum class DataType
	{
		WIDTH,			//	画像の幅を取得
		HEIGHT,			//	画像の高さを取得
		BIT_DEPTH,		//	ビット深度を取得
		COLOR_TYPE,		//	カラータイプを取得
		COMPRESSION,	//	圧縮手法を取得
		FILTER,			//	フィルター手法を取得
		INTERLACE,		//	インターレース手法を取得
		MIN = WIDTH,
		MAX = INTERLACE,
	};

public:
	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data	:	データ
	//	@param crc	:	エラーチェック
	//*********************************************************************************************
	Ihdr( const char* data, unsigned int crc );

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@oaram	width		:	幅
	//	@oaram	height		:	高さ
	//	@oaram	bitDepth	:	ビット深度
	//	@oaram	colorType	:	カラータイプ
	//	@oaram	compression	:	圧縮手法
	//	@oaram	filter		:	フィルター手法
	//	@oaram	interlace	:	インターレース手法
	//	@oaram	crc			:	エラーチェック
	//*********************************************************************************************
	Ihdr( unsigned int width, unsigned int height, 
		  unsigned char bitDepth, unsigned char colorType, unsigned char compression,
		  unsigned char filter, unsigned char interlace, unsigned int crc );
	~Ihdr() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;

private:
	//*********************************************************************************************
	//	@brief	ビット深度とカラータイプの組み合わせ許可
	//	@param bitDepth		:	ビット深度
	//	@param colorType	:	カラータイプ
	//	@return bool		:	許可されているか
	//*********************************************************************************************
	bool checkBitDepth( char bitDepth, char colorType );
};

//*********************************************************************************************
//	
//	チャンクPLTEクラス
//	パレット
//	
//*********************************************************************************************
class Plte : public ChunkBase
{
public:
	//	カラー数
	static const int COLOR_NUM;

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Plte( const char* data, unsigned int dataSize, unsigned int crc );
	~Plte() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataElement	:	データの要素番号
	//	@param void*		:	データポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataElement ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクIDATクラス
//	イメージデータ
//	
//*********************************************************************************************
class Idat : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Idat( const char* data, unsigned int dataSize, unsigned int crc );
	~Idat() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataElement	:	データの要素番号
	//	@param void*		:	データポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataElement ) override;
};

//*********************************************************************************************
//	
//	チャンクIENDクラス
//	イメージ終端
//	
//*********************************************************************************************
class Iend : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	コンストラクタ
	//*********************************************************************************************
	Iend( unsigned int crc );
	~Iend() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataElement	:	データタイプ
	//	@param void*		:	データポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;
};

//*********************************************************************************************
//	
//	チャンクTRNSクラス
//	透明度
//	
//*********************************************************************************************
class Trns : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Trns( const char* data, unsigned int dataSize, unsigned int crc, char colorType );
	~Trns() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;

private:
	char _colorType;
};

//*********************************************************************************************
//	
//	チャンクGAMAクラス
//	イメージガンマ
//	
//*********************************************************************************************
class Gama : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		GAMMA,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Gama( const char* data, unsigned int crc );
	~Gama() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクCHRMクラス
//	基礎色度
//	
//*********************************************************************************************
class Chrm : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		WHITE_POINT_X,
		WHITE_POINT_Y,
		RED_X,
		RED_Y,
		GREEN_X,
		GREEN_Y,
		BLUE_X,
		BLUE_Y,
		MIN = WHITE_POINT_X,
		MAX = BLUE_Y,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Chrm( const char* data, unsigned int crc );
	~Chrm() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクSRGBクラス
//	標準RGBカラースペース
//	
//*********************************************************************************************
class Srgb : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		RENDERING,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Srgb( const char* data, unsigned int crc );
	~Srgb() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクICCPクラス
//	組み込みICCプロフィール
//	
//*********************************************************************************************
class Iccp : public ChunkBase
{
public:
	enum class DataType
	{
		PROFILE,
		COMPRESSION,
		COMPRESSION_PROFILE,
		MIN = PROFILE,
		MAX = COMPRESSION_PROFILE,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Iccp( const char* data, unsigned int dataSize, unsigned int crc );
	~Iccp() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクTEXTクラス
//	テキストデータ
//	
//*********************************************************************************************
class Text : public ChunkBase
{
public:
	enum class DataType
	{
		KEYWORD,
		TEXT,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Text( const char* data, unsigned int dataSize, unsigned int crc );
	~Text() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクZTXTクラス
//	圧縮されたテキストデータ	
//	
//*********************************************************************************************
class Ztxt : public ChunkBase
{
public:
	enum class DataType
	{
		KEYWORD,
		COMPRESSION_FORMAT,
		COMPRESSION_TEXT,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Ztxt( const char* data, unsigned int dataSize, unsigned int crc );
	~Ztxt() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクITXTクラス
//	国際的なテキストデータ
//	
//*********************************************************************************************
class Itxt : public ChunkBase
{
public:
	enum class DataType
	{
		KEYWORD,
		COMPRESSION_FLAG,
		COMPRESSION_FORMAT,
		LANGUAGE,
		TRANSLATION_KEYWORD,
		TEXT,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Itxt( const char* data, unsigned int dataSize, unsigned int crc );
	~Itxt() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクBKGDクラス
//	背景色
//	
//*********************************************************************************************
class Bkgd : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		GRAY_LEVEL,
		BACKGROUND_RED,
		BACKGROUND_GREEN,
		BACKGROUND_BLUE,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Bkgd( const char* data, unsigned int crc );
	~Bkgd() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクPHYSクラス
//	物理的なピクセル寸法
//	
//*********************************************************************************************
class Phys : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		NUM_OF_PIXEL_PERL_UNIT_X,
		NUM_OF_PIXEL_PERL_UNIT_Y,
		UNIT_SPECIFIER,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Phys( const char* data, unsigned int crc );
	~Phys() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクSBITクラス
//	有効なビット
//	
//*********************************************************************************************
class Sbit : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		RED_IMPORTANT_BIT_NUM,
		GREEN_IMPORTANT_BIT_NUM,
		BLUE_IMPORTANT_BIT_NUM,
		GRAY_IMPORTANT_BIT_NUM,
		ALPHA_IMPORTANT_BIT_NUM,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Sbit( const char* data, unsigned int crc );
	~Sbit() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクSPLTクラス
//	推奨パレット
//	
//*********************************************************************************************
class Splt : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		PALET,
		SAMPLE_ACCURACY,
		RED,
		GREEN,
		BLUE,
		ALPHA,
		FREQUENCY,
		MIN = PALET,
		MAX = FREQUENCY,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Splt( const char* data, unsigned int dataSize, unsigned int crc );
	~Splt() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクHISTクラス
//	パレットヒストグラム
//	
//*********************************************************************************************
class Hist : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Hist( const char* data, unsigned int dataSize, unsigned int crc );
	~Hist() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataElement	:	データの要素番号
	//	@param void*		:	データポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataElement ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	チャンクTIMEクラス
//	イメージの最終更新時間
//	
//*********************************************************************************************
class Time : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class DataType
	{
		YEAR,
		MONTH,
		DAY,
		HOUR,
		MINUTE,
		SECOUND,
	};

	//*********************************************************************************************
	//	@brief	コンストラクタ
	//	@param data		:	データ
	//	@param dataSize	:	データ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	Time( const char* data, unsigned int crc );
	~Time() {};

	//*********************************************************************************************
	//	@brief	データの取得
	//	@param dataType	:	データタイプ
	//	@param void*	:	タイプごとのデータポインタ
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	データをセット
	//	@param data		:	チャンクデータ
	//	@param dataSize	:	チャンクデータのサイズ
	//	@param crc		:	エラーチェック
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

class PNG
{
public:
	PNG();
	~PNG();

	bool load( const char* fileNamePath );
	bool write( const char* fileNamePath );
	void release();

	void setChunkData( ChunkType chunkType, const char* data, unsigned int crc, unsigned int dataSize = 0 );

	ChunkBase* getChunkData( ChunkType chunkType, int number = 0 )
	{
		if( !_chunkMap.count(chunkType) )
		{
			printf( "存在しないチャンクを取得しようとしました。Chunk Type [ %s ]\n", chunkType );
			return nullptr;
		}

		if( _chunkMap[chunkType].size() <= number )
		{
			printf( "存在しない要素を取得しようとしました。Chunk Type [ %s ]\n", chunkType );
		}

		return _chunkMap[chunkType][number];
	}

private:
	std::map<ChunkType, std::vector<ChunkBase*>> _chunkMap;

	//	必須チャンク
	Ihdr* ihdr;
	Plte* plte;
	std::vector<Idat*> idat;
	Iend* iend;

	//	補助チャンク
	Trns* trns;
	Gama* gama;
	Chrm* chrm;
	Srgb* srgb;
	Iccp* iccp;
	std::vector<Text*> text;
	std::vector<Ztxt*> ztxt;
	std::vector<Itxt*> itxt;
	Bkgd* bkgd;
	Phys* phys;
	Sbit* sbit;
	std::vector<Splt*> splt;
	Hist* hist;
	Time* time;

	//	zlib
	z_stream zStream;

	//	PNG読み込み
	bool readPNG( FILE* fp );

	//	必須チャンク読み込み
	bool readIHDR( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readPLTE( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readIDAT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readIEND( FILE* fp, char* data, unsigned int dataSize, int crc );
	//	補助チャンク読み込み
	bool readTRNS( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readGAMA( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readCHRM( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readSRGB( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readICCP( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readTEXT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readZTXT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readITXT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readBKGD( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readPHYS( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readSBIT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readSPLT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readHIST( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readTIME( FILE* fp, char* data, unsigned int dataSize, int crc );

	//	PNG書き込み
	bool writePNG( FILE* fp );

	//	必須チャンク書き込み
	bool writeIHDR( FILE* fp );
	bool writePLTE( FILE* fp );
	bool writeIDAT( FILE* fp );
	bool writeIEND( FILE* fp );

	//	補助チャンク書き込み
	bool writeTRNS( FILE* fp );
	bool writeGAMA( FILE* fp );
	bool writeCHRM( FILE* fp );
	bool writeSRGB( FILE* fp );
	bool writeICCP( FILE* fp );
	bool writeTEXT( FILE* fp );
	bool writeZTXT( FILE* fp );
	bool writeITXT( FILE* fp );
	bool writeBKGD( FILE* fp );
	bool writePHYS( FILE* fp );
	bool writeSBIT( FILE* fp );
	bool writeSPLT( FILE* fp );
	bool writeHIST( FILE* fp );
	bool writeTIME( FILE* fp );
};
}
