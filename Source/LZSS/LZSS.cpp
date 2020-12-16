#include "LZSS.h"

bool LZSS::encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData )
{
	//	�X���C�h��
	std::vector<BYTE> slide( REFERENCE_SIZE * 2 + LENGTH_MAX );
	//	�X���C�h�ʒu
	int slidePostion = REFERENCE_SIZE;
	//	�ǂݍ��݂������v�T�C�Y
	int totalLoadSize = 0;
	int loadSize = 0;

	int encodePosition = 0;
	int encodeLength = 0;

	int offset = 0;

	//	�����k���i�f�o�b�O�p�j
	int rawNum = 0;
	//	���k�œ������T�C�Y�i�f�o�b�O�p�j�[�Ȃ�t�ɑ����Ă���
	int cancompressedSize = 0;

	while( totalLoadSize < rawData.size() )
	{
		//	���f�[�^����X���C�h���ɃR�s�[����T�C�Y
		loadSize = rawData.size() - totalLoadSize >= REFERENCE_SIZE + LENGTH_MAX ? REFERENCE_SIZE + LENGTH_MAX : rawData.size() - totalLoadSize;

		//	���k�O�f�[�^���X���C�h���ɃR�s�[
		std::copy( rawData.begin() + totalLoadSize, rawData.begin() + totalLoadSize + loadSize, slide.begin() + REFERENCE_SIZE);
		offset = LENGTH_MAX;

		int slideLimitSize = loadSize == REFERENCE_SIZE + LENGTH_MAX ? REFERENCE_SIZE * 2 : REFERENCE_SIZE + loadSize/* + LENGTH_MAX*/;

		//	�X���C�h�ʒu�̍X�V
		while( slidePostion < slideLimitSize )
		{
			//	TODO : �ő��v�����񌟍�
			int referencePosition = slidePostion - REFERENCE_SIZE;
			for( int i = referencePosition; i < slidePostion; i++ )
			{
				if( slide[i] == slide[slidePostion] )
				{
					int length = 1;

					for( int k = 1; k < LENGTH_MAX; k++ )
					{
						if( slide[i + k] == slide[slidePostion + k] )
						{
							length++;
							continue;
						}

						break;
					}

					if( length > encodeLength )
					{
						encodePosition = i - referencePosition;
						encodeLength = length;

						if( encodePosition == 0 )
						{
							encodePosition = 0;
						}
					}
				}
			}

			//	compressedData�֔��f
			if( encodeLength >= LENGTH_MIN )
			{
				printf( "�����k�̕������F%d\n", rawNum );
				cancompressedSize += encodeLength - LENGTH_MIN + 1;
				printf( "���k�œ������T�C�Y�F%d\n\n", cancompressedSize );

				printf( "������̈�v�F������ = %d\n", encodeLength );
				printf( "encode���镶����F" );
				for( int i = 0; i < encodeLength; i++ )
				{
					printf( "%02x ", slide[slidePostion - REFERENCE_SIZE + encodePosition + i] );
				}
				compressedData.push_back( 0x01 );
				encodeData = ( encodePosition << LENGTH_SIZE ) | ( encodeLength - LENGTH_MIN );
				printf( "encode����������F%02x %02x", encodeData >> 8, encodeData & 0xFF );
				printf("\n\n");
				compressedData.push_back( encodeData >> 8 );
				compressedData.push_back( encodeData & 0xFF );
				slidePostion += encodeLength;
			}
			else if( encodeLength > 0 )
			{
				for( int i = 0; i < encodeLength; i++ )
				{
					compressedData.push_back( 0x00 );
					compressedData.push_back( slide[slidePostion] );
					slidePostion++;
					rawNum++;
					cancompressedSize--;
				}
			}
			else
			{
				compressedData.push_back( 0x00 );
				compressedData.push_back( slide[slidePostion] );
				slidePostion++;
				rawNum++;
				cancompressedSize--;
			}

			//	������
			encodePosition = 0;
			encodeLength = 0;
		}

		//	�X���C�h���̃f�[�^��擪�փR�s�[
		std::copy( slide.begin() + REFERENCE_SIZE, slide.begin() + REFERENCE_SIZE * 2 + LENGTH_MAX, slide.begin() );

		//	�������A�X�V
		slidePostion = REFERENCE_SIZE;
		totalLoadSize += loadSize;
	}

	return true;
}

bool LZSS::decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData )
{
	//	�ǂݍ��݃T�C�Y
	int loadSize = 0;
	//	�X���C�h�ʒu
	int slidePostion = 0;

	while( compressedData.size() > loadSize )
	{
		if( compressedData[loadSize] == 0x00 )
		{
			rawData.push_back( compressedData[++loadSize] );
			//printf( "%x\n", rawData[rawData.size() - 1] );
			//printf( "\n\n" );
		}
		else
		{
			if( loadSize > 1500 )
			{
				int a = 0;
				a = 0;
			}
			BYTE a = compressedData[++loadSize];
			BYTE b = compressedData[++loadSize];
			WORD encodeData = ( a << 8 ) | b;

			for( int i = 0; i < ( encodeData & 0x07 ) + LENGTH_MIN; i++ )
			{
				rawData.push_back( rawData[rawData.size() - REFERENCE_SIZE + ( encodeData >> LENGTH_SIZE )] );
				//printf( "%x\n", rawData[rawData.size() - 1] );
			}
			//printf( "\n\n" );
		}

		loadSize++;
	}

	return true;
}
