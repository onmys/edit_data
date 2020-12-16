#include "LZSS.h"

bool LZSS::encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData )
{
	//	スライド窓
	std::vector<BYTE> slide( REFERENCE_SIZE * 2 + LENGTH_MAX );
	//	スライド位置
	int slidePostion = REFERENCE_SIZE;
	//	読み込みした合計サイズ
	int totalLoadSize = 0;
	int loadSize = 0;

	int encodePosition = 0;
	int encodeLength = 0;

	int offset = 0;

	//	未圧縮数（デバッグ用）
	int rawNum = 0;
	//	圧縮で得したサイズ（デバッグ用）ーなら逆に増えている
	int cancompressedSize = 0;

	while( totalLoadSize < rawData.size() )
	{
		//	生データからスライド窓にコピーするサイズ
		loadSize = rawData.size() - totalLoadSize >= REFERENCE_SIZE + LENGTH_MAX ? REFERENCE_SIZE + LENGTH_MAX : rawData.size() - totalLoadSize;

		//	圧縮前データをスライド窓にコピー
		std::copy( rawData.begin() + totalLoadSize, rawData.begin() + totalLoadSize + loadSize, slide.begin() + REFERENCE_SIZE);
		offset = LENGTH_MAX;

		int slideLimitSize = loadSize == REFERENCE_SIZE + LENGTH_MAX ? REFERENCE_SIZE * 2 : REFERENCE_SIZE + loadSize/* + LENGTH_MAX*/;

		//	スライド位置の更新
		while( slidePostion < slideLimitSize )
		{
			//	TODO : 最大一致文字列検索
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

			//	compressedDataへ反映
			if( encodeLength >= LENGTH_MIN )
			{
				printf( "未圧縮の文字数：%d\n", rawNum );
				cancompressedSize += encodeLength - LENGTH_MIN + 1;
				printf( "圧縮で得したサイズ：%d\n\n", cancompressedSize );

				printf( "文字列の一致：文字数 = %d\n", encodeLength );
				printf( "encodeする文字列：" );
				for( int i = 0; i < encodeLength; i++ )
				{
					printf( "%02x ", slide[slidePostion - REFERENCE_SIZE + encodePosition + i] );
				}
				compressedData.push_back( 0x01 );
				encodeData = ( encodePosition << LENGTH_SIZE ) | ( encodeLength - LENGTH_MIN );
				printf( "encodeした文字列：%02x %02x", encodeData >> 8, encodeData & 0xFF );
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

			//	初期化
			encodePosition = 0;
			encodeLength = 0;
		}

		//	スライド窓のデータを先頭へコピー
		std::copy( slide.begin() + REFERENCE_SIZE, slide.begin() + REFERENCE_SIZE * 2 + LENGTH_MAX, slide.begin() );

		//	初期化、更新
		slidePostion = REFERENCE_SIZE;
		totalLoadSize += loadSize;
	}

	return true;
}

bool LZSS::decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData )
{
	//	読み込みサイズ
	int loadSize = 0;
	//	スライド位置
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
