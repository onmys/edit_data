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
//	png�����͂œǂݍ��ݏ�������
//	
//*********************************************************************************************
namespace png
{
#define PNG_HEADER_NUM	8

typedef unsigned char Byte;

bool readPNG( const char* fileName );
bool writePNG( const char* fileName );

//	�r�b�g�������Ă��鐔
int flagOnCount( int bitFlag );

unsigned int ConvertEndian( unsigned int endian );

//	�`�����N�^�C�v
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
//	�`�����N�x�[�X�N���X
//	
//*********************************************************************************************
class ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@oaram	length		:	�f�[�^�̃T�C�Y�@���r�b�O�G���f�B�A��
	//	@oaram	chunkType	:	�`�����N�^�C�v
	//	@oaram	dataSize	:	�f�[�^�̃T�C�Y
	//*********************************************************************************************
	ChunkBase( const char* chunkType, unsigned int dataSize );
	ChunkBase();
	virtual ~ChunkBase();

	//	�`�����N�^�C�v���擾
	char* getChunkType() { return _chunkType; }
	//	�f�[�^���擾
	char* getData() { return _data; }
	//	�f�[�^�̃T�C�Y���擾
	unsigned int getDataSize();
	//	�`�����N�̃T�C�Y���擾
	unsigned int getSize();
	//	�G���[�`�F�b�N�l���擾
	unsigned int getCRC() { return _crc; }
	//	�f�[�^�̎擾
	virtual void* getDataElement( int dataType ) = 0;

protected:
	unsigned int _length;	//	�f�[�^�̃T�C�Y ���r�b�O�G���f�B�A���Ȃ̂Œ���
	char _chunkType[4];		//	�`�����N�^�C�v
	char* _data;		//	�f�[�^�̃|�C���^
	unsigned int _crc;		//	�G���[�`�F�b�N


	virtual bool setData( const char* data, unsigned int dataSize, unsigned int crc );
};

//*********************************************************************************************
//	
//	�`�����NIHDR�N���X
//	�C���[�W�w�b�_
//	
//*********************************************************************************************
class Ihdr : public ChunkBase
{
public:
	static const unsigned int CHUNK_DATA_SIZE;

	enum class ColorType
	{
		GRAY_SCALE			=	0,	//	�O���[�X�P�[���摜
		TRUE_COLOR			=	2,	//	�g�D���[�J���[�摜
		INDEX_COLOR			=	3,	//	�C���f�b�N�X�J���[�摜
		GRAY_SCALE_ALPHA	=	4,	//	�O���[�X�P�[���{�A���t�@�摜
		TRUE_COLOR_ALPHA	=	6,	//	�g�D���[�J���[�{�A���t�@�摜
	};

	//	�g����r�b�g�[�x
	const std::map<ColorType, char> permissionBitDepth = 
	{
		{ ColorType::GRAY_SCALE,		0x1F },
		{ ColorType::TRUE_COLOR,		0x18 },
		{ ColorType::INDEX_COLOR,		0xF  },
		{ ColorType::GRAY_SCALE_ALPHA,	0x18 },
		{ ColorType::TRUE_COLOR_ALPHA,	0x18 },
	};

	//	�f�[�^�擾�p
	enum class DataType
	{
		WIDTH,			//	�摜�̕����擾
		HEIGHT,			//	�摜�̍������擾
		BIT_DEPTH,		//	�r�b�g�[�x���擾
		COLOR_TYPE,		//	�J���[�^�C�v���擾
		COMPRESSION,	//	���k��@���擾
		FILTER,			//	�t�B���^�[��@���擾
		INTERLACE,		//	�C���^�[���[�X��@���擾
		MIN = WIDTH,
		MAX = INTERLACE,
	};

public:
	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@param data	:	�f�[�^
	//	@param crc	:	�G���[�`�F�b�N
	//*********************************************************************************************
	Ihdr( const char* data, unsigned int crc );

	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@oaram	width		:	��
	//	@oaram	height		:	����
	//	@oaram	bitDepth	:	�r�b�g�[�x
	//	@oaram	colorType	:	�J���[�^�C�v
	//	@oaram	compression	:	���k��@
	//	@oaram	filter		:	�t�B���^�[��@
	//	@oaram	interlace	:	�C���^�[���[�X��@
	//	@oaram	crc			:	�G���[�`�F�b�N
	//*********************************************************************************************
	Ihdr( unsigned int width, unsigned int height, 
		  unsigned char bitDepth, unsigned char colorType, unsigned char compression,
		  unsigned char filter, unsigned char interlace, unsigned int crc );
	~Ihdr() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;

private:
	//*********************************************************************************************
	//	@brief	�r�b�g�[�x�ƃJ���[�^�C�v�̑g�ݍ��킹����
	//	@param bitDepth		:	�r�b�g�[�x
	//	@param colorType	:	�J���[�^�C�v
	//	@return bool		:	������Ă��邩
	//*********************************************************************************************
	bool checkBitDepth( char bitDepth, char colorType );
};

//*********************************************************************************************
//	
//	�`�����NPLTE�N���X
//	�p���b�g
//	
//*********************************************************************************************
class Plte : public ChunkBase
{
public:
	//	�J���[��
	static const int COLOR_NUM;

	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Plte( const char* data, unsigned int dataSize, unsigned int crc );
	~Plte() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataElement	:	�f�[�^�̗v�f�ԍ�
	//	@param void*		:	�f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataElement ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NIDAT�N���X
//	�C���[�W�f�[�^
//	
//*********************************************************************************************
class Idat : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Idat( const char* data, unsigned int dataSize, unsigned int crc );
	~Idat() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataElement	:	�f�[�^�̗v�f�ԍ�
	//	@param void*		:	�f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataElement ) override;
};

//*********************************************************************************************
//	
//	�`�����NIEND�N���X
//	�C���[�W�I�[
//	
//*********************************************************************************************
class Iend : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//*********************************************************************************************
	Iend( unsigned int crc );
	~Iend() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataElement	:	�f�[�^�^�C�v
	//	@param void*		:	�f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;
};

//*********************************************************************************************
//	
//	�`�����NTRNS�N���X
//	�����x
//	
//*********************************************************************************************
class Trns : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Trns( const char* data, unsigned int dataSize, unsigned int crc, char colorType );
	~Trns() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;

private:
	char _colorType;
};

//*********************************************************************************************
//	
//	�`�����NGAMA�N���X
//	�C���[�W�K���}
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Gama( const char* data, unsigned int crc );
	~Gama() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NCHRM�N���X
//	��b�F�x
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Chrm( const char* data, unsigned int crc );
	~Chrm() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NSRGB�N���X
//	�W��RGB�J���[�X�y�[�X
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Srgb( const char* data, unsigned int crc );
	~Srgb() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NICCP�N���X
//	�g�ݍ���ICC�v���t�B�[��
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Iccp( const char* data, unsigned int dataSize, unsigned int crc );
	~Iccp() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NTEXT�N���X
//	�e�L�X�g�f�[�^
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Text( const char* data, unsigned int dataSize, unsigned int crc );
	~Text() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NZTXT�N���X
//	���k���ꂽ�e�L�X�g�f�[�^	
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Ztxt( const char* data, unsigned int dataSize, unsigned int crc );
	~Ztxt() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NITXT�N���X
//	���ۓI�ȃe�L�X�g�f�[�^
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Itxt( const char* data, unsigned int dataSize, unsigned int crc );
	~Itxt() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NBKGD�N���X
//	�w�i�F
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Bkgd( const char* data, unsigned int crc );
	~Bkgd() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NPHYS�N���X
//	�����I�ȃs�N�Z�����@
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Phys( const char* data, unsigned int crc );
	~Phys() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NSBIT�N���X
//	�L���ȃr�b�g
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Sbit( const char* data, unsigned int crc );
	~Sbit() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NSPLT�N���X
//	�����p���b�g
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Splt( const char* data, unsigned int dataSize, unsigned int crc );
	~Splt() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NHIST�N���X
//	�p���b�g�q�X�g�O����
//	
//*********************************************************************************************
class Hist : public ChunkBase
{
public:
	//*********************************************************************************************
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Hist( const char* data, unsigned int dataSize, unsigned int crc );
	~Hist() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataElement	:	�f�[�^�̗v�f�ԍ�
	//	@param void*		:	�f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataElement ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	bool setData( const char* data, unsigned int dataSize, unsigned int crc ) override;
};

//*********************************************************************************************
//	
//	�`�����NTIME�N���X
//	�C���[�W�̍ŏI�X�V����
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
	//	@brief	�R���X�g���N�^
	//	@param data		:	�f�[�^
	//	@param dataSize	:	�f�[�^
	//	@param crc		:	�G���[�`�F�b�N
	//*********************************************************************************************
	Time( const char* data, unsigned int crc );
	~Time() {};

	//*********************************************************************************************
	//	@brief	�f�[�^�̎擾
	//	@param dataType	:	�f�[�^�^�C�v
	//	@param void*	:	�^�C�v���Ƃ̃f�[�^�|�C���^
	//*********************************************************************************************
	virtual void* getDataElement( int dataType ) override;

protected:
	//*********************************************************************************************
	//	@brief	�f�[�^���Z�b�g
	//	@param data		:	�`�����N�f�[�^
	//	@param dataSize	:	�`�����N�f�[�^�̃T�C�Y
	//	@param crc		:	�G���[�`�F�b�N
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
			printf( "���݂��Ȃ��`�����N���擾���悤�Ƃ��܂����BChunk Type [ %s ]\n", chunkType );
			return nullptr;
		}

		if( _chunkMap[chunkType].size() <= number )
		{
			printf( "���݂��Ȃ��v�f���擾���悤�Ƃ��܂����BChunk Type [ %s ]\n", chunkType );
		}

		return _chunkMap[chunkType][number];
	}

private:
	std::map<ChunkType, std::vector<ChunkBase*>> _chunkMap;

	//	�K�{�`�����N
	Ihdr* ihdr;
	Plte* plte;
	std::vector<Idat*> idat;
	Iend* iend;

	//	�⏕�`�����N
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

	//	PNG�ǂݍ���
	bool readPNG( FILE* fp );

	//	�K�{�`�����N�ǂݍ���
	bool readIHDR( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readPLTE( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readIDAT( FILE* fp, char* data, unsigned int dataSize, int crc );
	bool readIEND( FILE* fp, char* data, unsigned int dataSize, int crc );
	//	�⏕�`�����N�ǂݍ���
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

	//	PNG��������
	bool writePNG( FILE* fp );

	//	�K�{�`�����N��������
	bool writeIHDR( FILE* fp );
	bool writePLTE( FILE* fp );
	bool writeIDAT( FILE* fp );
	bool writeIEND( FILE* fp );

	//	�⏕�`�����N��������
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
