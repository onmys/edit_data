#include "Huffman.h"

bool Huffman::encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData )
{
	nodes.clear();

	//	出現回数をチェック
	for( int i = 0; i < rawData.size(); i++ )
	{
		if( nodes.count( ( int )( rawData[i] ) ) == 0 )
		{
			Node node = { 0, nullptr, nullptr, nullptr, INT_MAX, 0 };
			nodes.insert( std::make_pair( ( int )( rawData[i] ), node ) );
		}

		nodes[( int )( rawData[i] )].count++;
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
		for( auto i = nodes.begin(); i != nodes.end(); i++ )
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
		nodes.insert( std::make_pair( ( int )( offset + dummyNodeNum ), dummyNode ) );
		nodeLeft->parent = &nodes[offset + dummyNodeNum];
		nodeRight->parent = &nodes[offset + dummyNodeNum];
		dummyNodeNum++;

		//	木が完成したかチェック
		int count = 0;

		for( auto i = nodes.begin(); i != nodes.end(); i++ )
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

	for( auto i = nodes.begin(); i != nodes.end(); i++ )
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
	for( auto i = nodes.begin(); i != nodes.end(); )
	{
		if( i->first >= offset )
		{
			i = nodes.erase( i );
			continue;
		}

		i++;
	}

	//	対応コードの登録
	//	対応コード数
	compressedData.push_back( nodes.size() );

	for( auto i = nodes.begin(); i != nodes.end(); i++ )
	{
		//	実データ
		compressedData.push_back( BYTE( i->first ) );
		//	対応コードの長さ
		compressedData.push_back( BYTE( i->second.length ) );
		//	対応コード
		for( int k = 0; k < i->second.length; k += 8 )
		{
			compressedData.push_back( BYTE( i->second.encode >> ( k * 8 ) ) );
		}
	}

	//	実データを対応コードに差し替え
	int totalSize = 0;
	int sizeMax = 0;
	for( int i = 0; i < rawData.size(); i++ )
	{
		//	TODO : ビット単位でデータを
		for( int k = 0; k < nodes[rawData[i]].length; k += 8 )
		{
			compressedData.push_back( BYTE( nodes[rawData[i]].encode >> ( k * 8 ) ) );
		}
		totalSize += nodes[rawData[i]].length;
		if( sizeMax < nodes[rawData[i]].length )
		{
			sizeMax = nodes[rawData[i]].length;
		}
		printf( "対応コードサイズ = %d, 合計サイズ = %d, 平均サイズ(1文字あたり) = %d\n", nodes[rawData[i]].length, totalSize, totalSize / ( i == 0 ? 1 : i ) );
	}
	printf( "最大対応コードサイズ = %d\n", sizeMax );
	printf( "未圧縮の合計サイズ = %d\n", rawData.size() * 8 );

	return true;
}

bool Huffman::decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData )
{








	return false;
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
