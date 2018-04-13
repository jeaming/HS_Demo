
#pragma warning(disable:4996)

enum DataType
{
	BIN2HEX,
	HEX2BIN
};

int Hex2Bin(unsigned char* outBin, unsigned char* inHex, long lPair_length)
{
	char src1,src2,factor1,factor2;
	long len;
	factor1 = '7';
	factor2 = '7';
    _strupr( (char *)inHex );

	for (len=0; len < lPair_length; len++) 
	{
		src1 = *(inHex+len*2);
		src2 = *(inHex+len*2+1);
		if ((src1>='0') && (src1<='9'))
		    factor1 = '0';
	    else if ((src1>='A') && (src1<='F'))
		    factor1 = '7';
	    else 
			return 1;
		if ((src2>='0') && (src2<='9'))
		    factor2 = '0';
	    else if ((src2>='A') && (src2<='F'))
		    factor2 = '7';
	    else 
			return 1;
		src1 = src1 - factor1;
		src2 = src2 - factor2;
		*outBin++ = (src1 << 4) | src2; 
	}
	return 0;
}


int Bin2Hex(unsigned char* outHex, unsigned char* inBin, long length)
{
	UCHAR hLowbit,hHighbit;
    long i;
	for(i=0;i<length*2;i=i+2)
	{
		hLowbit=inBin[i/2]&0x0f;
		hHighbit=inBin[i/2]/16;
		if(hHighbit>=10)
			outHex[i]=hHighbit+'7';
		else
			outHex[i]=hHighbit+'0';
		if(hLowbit>=10)
			outHex[i+1]=hLowbit+'7';
		else
			outHex[i+1]=hLowbit+'0';
	}
	outHex[length*2]='\0';

	return 0;
}

//HEX2BIN 为宏 数据不大于300 Bytes
int SwitchDataMode(unsigned char* pszData, int* iDataLen, int iSwitchMode)
{
	unsigned char szDataHex[600], szDataBin[300];
	if(!pszData)
		return -1;

	if(iSwitchMode == HEX2BIN)
	{
		Hex2Bin(szDataBin, pszData, *iDataLen);
		memcpy(pszData, szDataBin, *iDataLen/2);
		pszData[*iDataLen/2] = '\0';
		*iDataLen = *iDataLen/2;
		return -1;
	}
	
	if(iSwitchMode == BIN2HEX)
	{
		Bin2Hex(szDataHex, pszData, *iDataLen);
		memcpy(pszData, szDataHex, *iDataLen*2);
		pszData[*iDataLen*2] = '\0';
		*iDataLen = *iDataLen *2;
		return -1;
	}
	return 0;
}
