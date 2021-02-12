#pragma once
#include <vector>
#include <map>
#include <windows.h>

struct Node
{
	int count;
	Node* parent;
	Node* childLeft;
	Node* childRight;

	long length;
	unsigned long encode;
};

class Huffman
{
public:
	//*********************************************************************************************
	//	@brief	データを圧縮
	//	@param rawData			:	圧縮するデータ
	//	@param compressedData	:	圧縮されたデータ
	//*********************************************************************************************
	bool encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData );

	//*********************************************************************************************
	//	@brief	データを解凍
	//	@param compressedData	:	圧縮されたデータ
	//	@param rawData			:	圧縮するデータ
	//*********************************************************************************************
	bool decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData );

private:
	int getNodeDepthNum( Node* node );
	void find( long code, int codeLength, BYTE& key, Node& node );

	std::map<int, Node> _nodes;
	std::vector<BYTE> _inputRawData;
};

