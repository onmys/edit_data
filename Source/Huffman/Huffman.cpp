#include "Huffman.h"

bool Huffman::encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData )
{
	nodes.clear();

	//	�o���񐔂��`�F�b�N
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
	Node* nodeLeft;		//	1�ԏ��Ȃ��m�[�h
	Node* nodeRight;	//	2�Ԗڂɏ��Ȃ��m�[�h
	int nodeLeftDepth = 0;
	int nodeRightDepth = 0;
	Node dummyNode;
	int dummyNodeNum = 0;

	while( true )
	{
		nodeLeft = nullptr;
		nodeRight = nullptr;

		//	�؂̍쐬
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

		//	�؂������������`�F�b�N
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

	//	�Ή��R�[�h�̍쐬
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

	//	dummyNode�̍폜
	for( auto i = nodes.begin(); i != nodes.end(); )
	{
		if( i->first >= offset )
		{
			i = nodes.erase( i );
			continue;
		}

		i++;
	}

	//	�Ή��R�[�h�̓o�^
	//	�Ή��R�[�h��
	compressedData.push_back( nodes.size() );

	for( auto i = nodes.begin(); i != nodes.end(); i++ )
	{
		//	���f�[�^
		compressedData.push_back( BYTE( i->first ) );
		//	�Ή��R�[�h�̒���
		compressedData.push_back( BYTE( i->second.length ) );
		//	�Ή��R�[�h
		for( int k = 0; k < i->second.length; k += 8 )
		{
			compressedData.push_back( BYTE( i->second.encode >> ( k * 8 ) ) );
		}
	}

	//	���f�[�^��Ή��R�[�h�ɍ����ւ�
	int totalSize = 0;
	int sizeMax = 0;
	for( int i = 0; i < rawData.size(); i++ )
	{
		//	TODO : �r�b�g�P�ʂŃf�[�^��
		for( int k = 0; k < nodes[rawData[i]].length; k += 8 )
		{
			compressedData.push_back( BYTE( nodes[rawData[i]].encode >> ( k * 8 ) ) );
		}
		totalSize += nodes[rawData[i]].length;
		if( sizeMax < nodes[rawData[i]].length )
		{
			sizeMax = nodes[rawData[i]].length;
		}
		printf( "�Ή��R�[�h�T�C�Y = %d, ���v�T�C�Y = %d, ���σT�C�Y(1����������) = %d\n", nodes[rawData[i]].length, totalSize, totalSize / ( i == 0 ? 1 : i ) );
	}
	printf( "�ő�Ή��R�[�h�T�C�Y = %d\n", sizeMax );
	printf( "�����k�̍��v�T�C�Y = %d\n", rawData.size() * 8 );

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
