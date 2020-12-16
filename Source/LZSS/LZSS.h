#pragma once
#include <vector>
#include <windows.h>

class LZSS
{
public:
	//*********************************************************************************************
	//	@brief	�f�[�^�����k
	//	@param rawData			:	���k����f�[�^
	//	@param compressedData	:	���k���ꂽ�f�[�^
	//*********************************************************************************************
	bool encode( const std::vector<BYTE> rawData, std::vector<BYTE>& compressedData );

	//*********************************************************************************************
	//	@brief	�f�[�^����
	//	@param compressedData	:	���k���ꂽ�f�[�^
	//	@param rawData			:	���k����f�[�^
	//*********************************************************************************************
	bool decode( const std::vector<BYTE> compressedData, std::vector<BYTE>& rawData );

private:
	const int LENGTH_MAX = 7;	//	�G���R�[�h�ő啶���� 19, 18
	const int LENGTH_MIN = 4;	//	�G���R�[�h�ŏ������� 4, 3
	const int LENGTH_SIZE = 3;	//	�G���R�[�h���̒���(bit) 4
	const int POSITION_SIZE = 12 + 1;	//	�G���R�[�h���̈ʒu(bit)
	const int REFERENCE_SIZE = 1 << POSITION_SIZE;	//	�Q�ƃT�C�Y

	WORD encodeData;
};

