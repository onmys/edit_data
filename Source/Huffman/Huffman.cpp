#include "Huffman.h"

bool Huffman::encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData )
{
	_inputRawData = rawData;
	_nodes.clear();

	//	出現回数をチェック
	for( int i = 0; i < rawData.size(); i++ )
	{
		if( _nodes.count( ( int )( rawData[i] ) ) == 0 )
		{
			Node node = { 0, nullptr, nullptr, nullptr, INT_MAX, 0 };
			_nodes.insert( std::make_pair( ( int )( rawData[i] ), node ) );
		}

		_nodes[( int )( rawData[i] )].count++;
	}

	int offset = 256;
	Node* nodeLeft;		//	1番少ないノード
	Node* nodeRight;	//	2番目に少ないノード
	int nodeLeftDepth = 0;
	int nodeRightDepth = 0;
	Node dummyNode;
	int dummyNodeNum = 0;

	while( true )
	{
		nodeLeft = nullptr;
		nodeRight = nullptr;

		//	木の作成
		for( auto i = _nodes.begin(); i != _nodes.end(); i++ )
		{
			if( i->second.parent == nullptr )
			{
				if( nodeLeft == nullptr )
				{
					nodeLeft = &i->second;
					nodeLeftDepth = getNodeDepthNum( nodeLeft );
					continue;
				}
				if( nodeRight == nullptr )
				{
					if( i->second.count > nodeLeft->count )
					{
						nodeRight = &i->second;
						nodeRightDepth = getNodeDepthNum( nodeRight );
					}
					else
					{
						nodeRight = nodeLeft;
						nodeRightDepth = nodeLeftDepth;
						nodeLeft = &i->second;
						nodeLeftDepth = getNodeDepthNum( nodeLeft );
					}
					continue;
				}
				if( i->second.count <= nodeLeft->count )
				{
					nodeRight = nodeLeft;
					nodeRightDepth = nodeLeftDepth;
					nodeLeft = &i->second;
					nodeLeftDepth = getNodeDepthNum( nodeLeft );
					continue;
				}
				if( i->second.count <= nodeRight->count )
				{
					nodeRight = &i->second;
					nodeRightDepth = getNodeDepthNum( nodeRight );
				}
			}
		}

		if( nodeLeftDepth < nodeRightDepth )
		{
			Node* tmp = nodeLeft;
			nodeLeft = nodeRight;
			nodeRight = tmp;
		}

		dummyNode = { nodeLeft->count + nodeRight->count, nullptr, nodeLeft, nodeRight, INT_MAX, 0 };
		_nodes.insert( std::make_pair( ( int )( offset + dummyNodeNum ), dummyNode ) );
		nodeLeft->parent = &_nodes[offset + dummyNodeNum];
		nodeRight->parent = &_nodes[offset + dummyNodeNum];
		dummyNodeNum++;

		//	木が完成したかチェック
		int count = 0;

		for( auto i = _nodes.begin(); i != _nodes.end(); i++ )
		{
			if( i->second.parent == nullptr )
			{
				count++;
			}
		}

		if( count == 1 )
		{
			break;
		}
	}

	//	対応コードの作成
	int length = 0;
	int encode = 0;

	for( auto i = _nodes.begin(); i != _nodes.end(); i++ )
	{
		if( i->first < offset )
		{
			Node* node = &i->second;
			length = 0;
			encode = 0;

			while( node->parent )
			{
				int tmp = node == node->parent->childRight ? 0 : 1;

				node = node->parent;
				encode |= ( tmp << length );
				length++;
			}

			i->second.length = length;
			i->second.encode = encode;
		}
	}

	//	dummyNodeの削除
	for( auto i = _nodes.begin(); i != _nodes.end(); )
	{
		if( i->first >= offset )
		{
			i = _nodes.erase( i );
			continue;
		}

		i++;
	}

	//	対応コードの登録
	//	対応コード数
	compressedData.push_back( _nodes.size() - 1 );
	printf( "対応コード登録(数：%d）\n", compressedData[0] );

	for( auto i = _nodes.begin(); i != _nodes.end(); i++ )
	{
		//	実データ
		compressedData.push_back( BYTE( i->first ) );
		//	対応コードの長さ
		compressedData.push_back( BYTE( i->second.length ) );
		//	対応コード
		for( int k = 0; k < i->second.length; k++ )
		{
			compressedData.push_back( BYTE( i->second.encode >> ( k * 8 ) ) );
		}
	}

	//	実データを対応コードに差し替え
	length = 0;
	unsigned long encodeData = 0;
	int shift = 0;

	printf( "実データを対応コードに差し替え\n" );

	for( int i = 0; i < rawData.size(); i++ )
	{
		encodeData <<= _nodes[rawData[i]].length;
		encodeData |= _nodes[rawData[i]].encode;
		length += _nodes[rawData[i]].length;

		printf( "実コード：%d, 対応コード：%d\n", rawData[i], _nodes[rawData[i]].encode );

		if( length >= 8 )
		{
			compressedData.push_back( BYTE( encodeData >> ( length - 8 ) ) );
			shift = ( 32 - ( length - 8 ) );
			encodeData = shift >= 32 ? 0 : encodeData << shift;
			encodeData = shift >= 32 ? 0 : encodeData >> shift;
			length -= 8;
		}
	}

	if( length > 0 )
	{
		compressedData.push_back( BYTE( encodeData ) );
	}

	return true;
}

bool Huffman::decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData )
{
	//	対応コードの取得
	int dataPosition = 0;
	int codeSize = compressedData[dataPosition++];
	//std::map<int, Node> codes;
	_nodes.clear();

	long key = 0;
	long length = 0;
	unsigned long encode;
	Node node;

	for( int i = 0; i <= codeSize; i++ )
	{
		encode = 0;
		key = compressedData[dataPosition++];
		length = compressedData[dataPosition++];

		for( int k = 0; k < length; k++ )
		{
			encode |= ( compressedData[dataPosition++] << ( k * 8 ) );
		}

		node = { 0, nullptr, nullptr, nullptr, length, encode };
		_nodes.insert( std::make_pair( ( int )( key ), node ) );
	}

	unsigned long buffer = compressedData[dataPosition++];
	int bufferLength = 8;
	int matchCodeLength = 0;
	BYTE raw;
	int shift = 0;

	//	対応コードを実データに差し替え
	for( int i = dataPosition; i < compressedData.size(); )
	{
		node.encode = 0;
		node.length = 0;

		find( buffer, bufferLength, raw, node );

		if( node.length > 0 )
		{
			rawData.push_back( raw );
			shift = 32 - ( bufferLength - node.length );
			buffer = shift >= 32 ? 0 : buffer << shift;
			buffer = shift >= 32 ? 0 : buffer >> shift;
			bufferLength -= node.length;
		}
		else
		{
			buffer <<= 8;
			buffer |= compressedData[i];
			bufferLength += 8;
			i++;
		}
	}


	return true;
}

int Huffman::getNodeDepthNum( Node* node )
{
	int depthNum = 0;

	if( node->childLeft )
	{
		Node* child = node->childLeft;

		while( child )
		{
			depthNum++;
			child = child->childLeft;
		}
	}

	return depthNum;
}

void Huffman::find( long code, int codeLength, BYTE& key, Node& node )
{
	long tmp = 0;
	int length = 0;

	for( int i = codeLength - 1; i >= 0; i-- )
	{
		tmp = code >> i;
		length = codeLength - i;

		for( auto itr = _nodes.begin(); itr != _nodes.end(); itr++ )
		{
			if( itr->second.length != length )
			{
				continue;
			}

			if( itr->second.encode == tmp )
			{
				key = itr->first;
				node = itr->second;
				return;
			}
		}
	}
}
