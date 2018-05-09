// TripleDES.cpp: implementation of the TripleDES class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TripleDES.h"
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TripleDES::TripleDES()
{

}

TripleDES::~TripleDES()
{

}






unsigned long TripleDES::DES_Encrypt( unsigned char *Key,__int16 KeyLen,  unsigned char *Source, unsigned long SrcLen, unsigned char *result)
{
	__int16 ip[]=
	{
			58,50,42,34,26,18,10,2,
			60,52,44,36,28,20,12,4,
			62,54,46,38,30,22,14,6,
			64,56,48,40,32,24,16,8,
			57,49,41,33,25,17,9 ,1,
			59,51,43,35,27,19,11,3,
			61,53,45,37,29,21,13,5,
			63,55,47,39,31,23,15,7
	};
	unsigned char fp[] =
	{
			40,8,48,16,56,24,64,32,
			39,7,47,15,55,23,63,31,
			38,6,46,14,54,22,62,30,
			37,5,45,13,53,21,61,29,
			36,4,44,12,52,20,60,28,
			35,3,43,11,51,19,59,27,
			34,2,42,10,50,18,58,26,
			33,1,41, 9,49,17,57,25
	};
	__int16 e[]=
	{
			32,1 ,2 ,3 ,4 ,5 ,
			4 ,5 ,6 ,7 ,8 ,9 ,
			8, 9 ,10,11,12,13,
			12,13,14,15,16,17,
			16,17,18,19,20,21,
			20,21,22,23,24,25,
			24,25,26,27,28,29,
			28,29,30,31,32,1
	};
	__int16 p[]=
	{
			16,7 ,20,21,
			29,12,28,17,
			1 ,15,23,26,
			5 ,18,31,10,
			2 ,8 ,24,14,
			32,27,3 ,9,
			19,13,30,6,
			22,11,4 ,25
	};
	unsigned char si[8][64] =
	{
			{14,4 ,13,1 ,2 ,15,11,8 ,3 ,10,6 ,12,5 ,9 ,0 ,7 ,
			0 ,15,7 ,4 ,14,2 ,13,1 ,10,6 ,12,11,9 ,5 ,3 ,8 ,
			4 ,1 ,14,8 ,13,6 ,2 ,11,15,12,9 ,7 ,3 ,10,5 ,0 ,
			15,12,8 ,2 ,4 ,9 ,1 ,7 ,5 ,11,3 ,14,10,0 ,6 ,13},

		       {15,1 ,8 ,14,6 ,11,3 ,4 ,9 ,7 ,2 ,13,12,0 ,5 ,10,
			3, 13,4 ,7 ,15,2 ,8 ,14,12,0 ,1 ,10,6 ,9 ,11,5 ,
			0, 14,7 ,11,10,4 ,13,1 ,5 ,8 ,12,6 ,9 ,3 ,2 ,15,
			13,8 ,10,1 ,3 ,15,4 ,2 ,11,6 ,7 ,12,0 ,5 ,14,9 },

		       {10,0 ,9 ,14,6 ,3 ,15,5 ,1 ,13,12,7 ,11,4 ,2 ,8 ,
			13,7 ,0 ,9 ,3 ,4 ,6 ,10,2 ,8 ,5 ,14,12,11,15,1 ,
			13,6 ,4 ,9 ,8 ,15,3 ,0 ,11,1 ,2 ,12,5 ,10,14,7 ,
			1 ,10,13,0 ,6 ,9 ,8 ,7 ,4 ,15,14,3 ,11,5 ,2 ,12},

		       {7 ,13,14,3 ,0 ,6 ,9 ,10,1 ,2 ,8 ,5 ,11,12,4 ,15,
			13,8 ,11,5 ,6 ,15,0 ,3 ,4 ,7 ,2 ,12,1 ,10,14,9 ,
			10,6 ,9 ,0 ,12,11,7 ,13,15,1 ,3 ,14,5 ,2 ,8 ,4 ,
			3 ,15,0 ,6 ,10,1 ,13,8 ,9 ,4 ,5 ,11,12,7 ,2 ,14},

		       {2 ,12,4 ,1 ,7 ,10,11,6 ,8 ,5 ,3 ,15,13,0 ,14,9 ,
			14,11,2 ,12,4 ,7 ,13,1 ,5 ,0 ,15,10,3 ,9 ,8 ,6,
			4 ,2 ,1 ,11,10,13,7 ,8 ,15,9 ,12,5 ,6 ,3 ,0 ,14,
			11,8 ,12,7 ,1 ,14,2 ,13,6 ,15,0 ,9 ,10,4 ,5 ,3 },

		       {12,1 ,10,15,9 ,2 ,6 ,8 ,0 ,13,3 ,4 ,14,7 ,5 ,11,
			10,15,4 ,2 ,7 ,12,9 ,5 ,6 ,1 ,13,14,0 ,11,3 ,8 ,
			9 ,14,15,5 ,2 ,8 ,12,3 ,7 ,0 ,4 ,10,1 ,13,11,6 ,
			4 ,3 ,2 ,12,9 ,5 ,15,10,11,14,1 ,7 ,6 ,0 ,8 ,13},

		       {4 ,11,2 ,14,15,0 ,8 ,13,3 ,12,9 ,7 ,5 ,10,6 ,1 ,
			13,0 ,11,7 ,4 ,9 ,1 ,10,14,3 ,5 ,12,2 ,15,8 ,6 ,
			1 ,4 ,11,13,12,3 ,7 ,14,10,15,6 ,8 ,0 ,5 ,9 ,2 ,
			6 ,11,13,8 ,1 ,4 ,10,7 ,9 ,5 ,0 ,15,14,2 ,3 ,12},

		       {13,2 ,8 ,4 ,6 ,15,11,1 ,10,9 ,3 ,14,5 ,0 ,12,7 ,
			1 ,15,13,8 ,10,3 ,7 ,4 ,12,5 ,6 ,11,0 ,14,9 ,2 ,
			7 ,11,4 ,1 ,9 ,12,14,2 ,0 ,6 ,10,13,15,3 ,5 ,8 ,
			2 ,1 ,14,7 ,4 ,10,8 ,13,15,12,9 ,0 ,3 ,5 ,6 ,11}
	};

	unsigned char src[1000][8],src_bit[64],ipt[64];
	unsigned char et[48],pt1[32],pt2[32],r[32],l[32],*pt;
	unsigned char Pkey[8],ChildKey[16][48];
	//unsigned char temp[16][6];
	unsigned char temp_a;
	unsigned char *res;
	unsigned long i,j,k,m,row,h,time,ReturnLen;

	i=0;
	if(SrcLen%8)
		i=1;
	i=SrcLen/8+i;
	ReturnLen=i*8;
	res=result;
	for(i=0;i<8;i++)	Pkey[i]=Key[i];
	if(KeyLen<8)
		for(i=0;(short)i<8-KeyLen;i++)	Pkey[KeyLen+(short)i]=0x00;
	Gen_Childkey(Pkey,ChildKey);
//group
	if((SrcLen%8)==0)
	{
		time=SrcLen/8;
		for(i=0;i<time;i++)
			for(j=0;j<8;j++)	src[i][j]=Source[i*8+j];
	}
	else
	{
		time=SrcLen/8+1;
		for(i=0;i<time;i++)
			for(j=0;j<8;j++)
				if((i*8+j)<SrcLen)	src[i][j]=Source[i*8+j];
				else			src[i][j]=0x00;
	}
//des
	for(m=0;m<time;m++)
	{
		Char_Bit(src[m],8,src_bit);
		for(i=0;i<64;i++)	ipt[i]=src_bit[ip[i]-1];	//ip
		for(i=0;i<32;i++)
		{						//r0,l0
			l[i]=ipt[i];
			r[i]=ipt[i+32];
		}
		for(k=0;k<16;k++)
		{
			pt=pt1;
			for(i=0;i<48;i++)	et[i]=r[e[i]-1];    	//e
			for(i=0;i<48;i++)	et[i]=et[i]^ChildKey[k][i];
									//xor
			for(i=0;i<8;i++)
			{
				row=et[i*6+0]*2+et[i*6+5];		//s[i]
				h=et[i*6+1]*8+et[i*6+2]*4+et[i*6+3]*2+et[i*6+4];
				temp_a=si[i][row*16+h];
				if(temp_a&0x08)	*pt++=0x01;
				else		*pt++=0x00;
				if(temp_a&0x04)	*pt++=0x01;
				else		*pt++=0x00;
				if(temp_a&0x02)	*pt++=0x01;
				else		*pt++=0x00;
				if(temp_a&0x01)	*pt++=0x01;
				else		*pt++=0x00;
			}
			for(i=0;i<32;i++)	pt2[i]=pt1[p[i]-1];
			for(i=0;i<32;i++)	pt2[i]=pt2[i]^l[i];	//r1
			for(i=0;i<32;i++)	{l[i]=r[i];	r[i]=pt2[i];}
		}
		for(i=0;i<32;i++)   	{ipt[i+32]=l[i];	ipt[i]=r[i];}
		for(i=0;i<64;i++)	src_bit[i]=ipt[fp[i]-1];
		Bit_Char(src_bit,64,res);
		res+=8;
	}
	return ReturnLen;
}

__int16 TripleDES::DES_Decrypt( unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned long SrcLen, unsigned char *result)
{
	__int16 ip[]=
	{
			58,50,42,34,26,18,10,2,
			60,52,44,36,28,20,12,4,
			62,54,46,38,30,22,14,6,
			64,56,48,40,32,24,16,8,
			57,49,41,33,25,17,9 ,1,
			59,51,43,35,27,19,11,3,
			61,53,45,37,29,21,13,5,
			63,55,47,39,31,23,15,7
	};
	unsigned char fp[] =  	
	{
			40,8,48,16,56,24,64,32,
			39,7,47,15,55,23,63,31,
			38,6,46,14,54,22,62,30,
			37,5,45,13,53,21,61,29,
			36,4,44,12,52,20,60,28,
			35,3,43,11,51,19,59,27,
			34,2,42,10,50,18,58,26,
			33,1,41, 9,49,17,57,25
	};
	__int16 e[]=	
	{
			32,1 ,2 ,3 ,4 ,5 ,
			4 ,5 ,6 ,7 ,8 ,9 ,
			8, 9 ,10,11,12,13,
			12,13,14,15,16,17,
			16,17,18,19,20,21,
			20,21,22,23,24,25,
			24,25,26,27,28,29,
			28,29,30,31,32,1
	};
	__int16 p[]=	
	{
			16,7 ,20,21,
			29,12,28,17,
			1 ,15,23,26,
			5 ,18,31,10,
			2 ,8 ,24,14,
			32,27,3 ,9,
			19,13,30,6,
			22,11,4 ,25
	};
	unsigned char si[8][64] =
	{
			{14,4 ,13,1 ,2 ,15,11,8 ,3 ,10,6 ,12,5 ,9 ,0 ,7 ,
			0 ,15,7 ,4 ,14,2 ,13,1 ,10,6 ,12,11,9 ,5 ,3 ,8 ,
			4 ,1 ,14,8 ,13,6 ,2 ,11,15,12,9 ,7 ,3 ,10,5 ,0 ,
			15,12,8 ,2 ,4 ,9 ,1 ,7 ,5 ,11,3 ,14,10,0 ,6 ,13},

		       {15,1 ,8 ,14,6 ,11,3 ,4 ,9 ,7 ,2 ,13,12,0 ,5 ,10,
			3, 13,4 ,7 ,15,2 ,8 ,14,12,0 ,1 ,10,6 ,9 ,11,5 ,
			0, 14,7 ,11,10,4 ,13,1 ,5 ,8 ,12,6 ,9 ,3 ,2 ,15,
			13,8 ,10,1 ,3 ,15,4 ,2 ,11,6 ,7 ,12,0 ,5 ,14,9 },

		       {10,0 ,9 ,14,6 ,3 ,15,5 ,1 ,13,12,7 ,11,4 ,2 ,8 ,
			13,7 ,0 ,9 ,3 ,4 ,6 ,10,2 ,8 ,5 ,14,12,11,15,1 ,
			13,6 ,4 ,9 ,8 ,15,3 ,0 ,11,1 ,2 ,12,5 ,10,14,7 ,
			1 ,10,13,0 ,6 ,9 ,8 ,7 ,4 ,15,14,3 ,11,5 ,2 ,12},

		       {7 ,13,14,3 ,0 ,6 ,9 ,10,1 ,2 ,8 ,5 ,11,12,4 ,15,
			13,8 ,11,5 ,6 ,15,0 ,3 ,4 ,7 ,2 ,12,1 ,10,14,9 ,
			10,6 ,9 ,0 ,12,11,7 ,13,15,1 ,3 ,14,5 ,2 ,8 ,4 ,
			3 ,15,0 ,6 ,10,1 ,13,8 ,9 ,4 ,5 ,11,12,7 ,2 ,14},

		       {2 ,12,4 ,1 ,7 ,10,11,6 ,8 ,5 ,3 ,15,13,0 ,14,9 ,
			14,11,2 ,12,4 ,7 ,13,1 ,5 ,0 ,15,10,3 ,9 ,8 ,6,
			4 ,2 ,1 ,11,10,13,7 ,8 ,15,9 ,12,5 ,6 ,3 ,0 ,14,
			11,8 ,12,7 ,1 ,14,2 ,13,6 ,15,0 ,9 ,10,4 ,5 ,3 },

		       {12,1 ,10,15,9 ,2 ,6 ,8 ,0 ,13,3 ,4 ,14,7 ,5 ,11,
			10,15,4 ,2 ,7 ,12,9 ,5 ,6 ,1 ,13,14,0 ,11,3 ,8 ,
			9 ,14,15,5 ,2 ,8 ,12,3 ,7 ,0 ,4 ,10,1 ,13,11,6 ,
			4 ,3 ,2 ,12,9 ,5 ,15,10,11,14,1 ,7 ,6 ,0 ,8 ,13},

		       {4 ,11,2 ,14,15,0 ,8 ,13,3 ,12,9 ,7 ,5 ,10,6 ,1 ,
			13,0 ,11,7 ,4 ,9 ,1 ,10,14,3 ,5 ,12,2 ,15,8 ,6 ,
			1 ,4 ,11,13,12,3 ,7 ,14,10,15,6 ,8 ,0 ,5 ,9 ,2 ,
			6 ,11,13,8 ,1 ,4 ,10,7 ,9 ,5 ,0 ,15,14,2 ,3 ,12},

		       {13,2 ,8 ,4 ,6 ,15,11,1 ,10,9 ,3 ,14,5 ,0 ,12,7 ,
			1 ,15,13,8 ,10,3 ,7 ,4 ,12,5 ,6 ,11,0 ,14,9 ,2 ,
			7 ,11,4 ,1 ,9 ,12,14,2 ,0 ,6 ,10,13,15,3 ,5 ,8 ,
			2 ,1 ,14,7 ,4 ,10,8 ,13,15,12,9 ,0 ,3 ,5 ,6 ,11}
	};

	unsigned char src[1000][8],src_bit[64],ipt[64];
	unsigned char et[48],pt1[32],pt2[32],r[32],l[32],*pt;
	unsigned char Pkey[8],ChildKey[16][48];
	//unsigned char temp[16][6];
	unsigned char temp_a;
	unsigned char *res;
	unsigned long i,j,k,m,row,h,time;
	res=result;
	for(i=0;i<8;i++)	Pkey[i]=Key[i];
	if(KeyLen<8)
		for(i=0;(short)i<8-KeyLen;i++)	Pkey[KeyLen+(short)i]=0x00;
	Gen_Childkey(Pkey,ChildKey);
//group
	if((SrcLen%8)==0)
		{
		time=SrcLen/8;
		for(i=0;i<time;i++)
			for(j=0;j<8;j++)	src[i][j]=Source[i*8+j];
		}
	else
		{
		time=SrcLen/8+1;
		for(i=0;i<time;i++)
			for(j=0;j<8;j++)
				if((i*8+j)<SrcLen)	src[i][j]=Source[i*8+j];
				else		src[i][j]=0x00;
		}
//des
	for(m=0;m<time;m++)
		{
		Char_Bit(src[m],8,src_bit);
		for(i=0;i<64;i++)	ipt[i]=src_bit[ip[i]-1];	//ip
		for(i=0;i<32;i++)
			{						//r0,l0
			l[i]=ipt[i];
			r[i]=ipt[i+32];
			}
		for(k=0;k<16;k++)
			{
			pt=pt1;
			for(i=0;i<48;i++)	et[i]=r[e[i]-1];    	//e
			for(i=0;i<48;i++)	et[i]=et[i]^ChildKey[15-k][i];
									//xor
			for(i=0;i<8;i++)
				{
				row=et[i*6+0]*2+et[i*6+5];		//s[i]
				h=et[i*6+1]*8+et[i*6+2]*4+et[i*6+3]*2+et[i*6+4];
				temp_a=si[i][row*16+h];
				if(temp_a&0x08)	*pt++=0x01;
				else		*pt++=0x00;
				if(temp_a&0x04)	*pt++=0x01;
				else		*pt++=0x00;
				if(temp_a&0x02)	*pt++=0x01;
				else		*pt++=0x00;
				if(temp_a&0x01)	*pt++=0x01;
				else		*pt++=0x00;
				}
			for(i=0;i<32;i++)	pt2[i]=pt1[p[i]-1];
			for(i=0;i<32;i++)	pt2[i]=pt2[i]^l[i];	//r1
			for(i=0;i<32;i++)	{l[i]=r[i];	r[i]=pt2[i];}
			}
		for(i=0;i<32;i++)   	{ipt[i+32]=l[i];	ipt[i]=r[i];}
		for(i=0;i<64;i++)	src_bit[i]=ipt[fp[i]-1];
		Bit_Char(src_bit,64,res);
		res+=8;
		}
	return 0;
}

void TripleDES::Bit_Char(unsigned char *Src,__int16 BitLen,unsigned char *Res)
{       
	__int16 i,j,a;
	for(i=0;i<(BitLen/8);i++)
	{
		a=128;
		Res[i]=0x00;
		for(j=0;j<8;j++)
		{
			Res[i]=Res[i]+Src[j+i*8]*a;
			a/=2;
		}
	}
}

void TripleDES::Char_Bit(unsigned char *Src,__int16 CharLen,unsigned char *Res)
{       __int16 i,j;
	unsigned char temp;
	for(i=0;i<CharLen;i++)
		for(j=0;j<8;j++){
			temp=(Src[i]<<j)&0x80;
			if(temp)	Res[i*8+j]=0x01;
			else		Res[i*8+j]=0x00;
			}
	return;
}

void TripleDES::Gen_Childkey(unsigned char Key[8],unsigned char Childkey[16][48])
{
	__int16 pc_1[] =	{
			57,49,41,33,25,17,9 ,
			1 ,58,50,42,34,26,18,
			10,2 ,59,51,43,35,27,
			19,11,3 ,60,52,44,36,
			63,55,47,39,31,23,15,
			7 ,62,54,46,38,30,22,
			14,6 ,61,53,45,37,29,
			21,13,5 ,28,20,12,4
			};
	__int16 sht[] = 	{
			1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 , 1 ,2 ,2 ,2 ,2 ,2 ,2 ,1
			};
	__int16 pc_2[] =	{
			14,17,11,24,1 ,5 ,
			3 ,28,15,6 ,21,10,
			23,19,12,4 ,26,8 ,
			16,7 ,27,20,13,2 ,
			41,52,31,37,47,55,
			30,40,51,45,33,48,
			44,49,39,56,34,53,
			46,42,50,36,29,32
			};
	__int16 i,j,k;
	unsigned char pc_1t[56],temp;
	unsigned char keybit[64];
	Char_Bit(Key,8,keybit);
	for(i=0;i<56;i++)                       //pc-1
		pc_1t[i]=keybit[pc_1[i]-1];
	for(k=0;k<16;k++)
		{
		for(i=0;i<sht[k];i++)
			{			//c1, d1
			temp=pc_1t[0];
			for(j=0;j<28;j++)
				if(j==27)	pc_1t[j]=temp;
				else		pc_1t[j]=pc_1t[j+1];

			temp=pc_1t[28];
			for(j=28;j<56;j++)
				if(j==55)	pc_1t[j]=temp;
				else		pc_1t[j]=pc_1t[j+1];
			}
		for(i=0;i<48;i++)		//pc-2
			Childkey[k][i]=pc_1t[pc_2[i]-1];
		}
}




void TripleDES::InvertByte(unsigned char *src,int len,unsigned char *des)		/////////////////////////////
{
	for(int i=0;i<len;i++)
	{
		des[i] = ~src[i];
	}
}

void TripleDES::CharToInt(unsigned int *des, UCHAR *src, int pair_len)		/////////////////////////////
{
	unsigned char *p2;
	char ch[9];
	ch[8]=0;

	des[0] = pair_len;
	for(int i=0;i<pair_len;i++)
	{
		p2=&src[(pair_len-i-1)*8];
		memcpy(ch,p2,8);
		sscanf_s((const char*)p2,"%x",&des[i+1]);
	}
}

void TripleDES::IntToChar(unsigned char*des,unsigned long*src,int nLen)		/////////////////////////////
{
	unsigned char ch[4];
	unsigned char *p;

	for(int i=0;i<nLen;i++)
	{
		p= (unsigned char*)&src[i];
		for(int j=0;j<4;j++)
		{
			ch[j] = p[3-j];

		}
		HexToChar(&des[i*8],ch,4);
	}
}

void TripleDES::HexToChar(unsigned char *Dest, unsigned char *Src, unsigned short SrcLen)
{

	char	ddl,ddh;
	
	Dest[SrcLen*2] = 0;
	for (int i=0;i<SrcLen;i++)
	{
		ddh = 48 + Src[i] / 16;
		ddl = 48 + Src[i] % 16;
		if (ddh > 57)  ddh = ddh + 7;
		if (ddl > 57)  ddl = ddl + 7;
		Dest[i*2] = ddh;
		Dest[i*2+1] = ddl;
	}

}

void TripleDES::CharToHex(char *hh, BYTE *dd,int len)
{
	char h1,h2;
	BYTE s1,s2;

	for (int i=0;i<len;i++){
		h1 = hh[2*i];
		h2 = hh[2*i+1];
		s1 = toupper(h1) - 0x30;
		if (s1 > 9) s1 -= 7;
		s2 = toupper(h2) - 0x30;
		if (s2 > 9) s2 -= 7;
		dd[i] = s1*16 + s2;
    }
}

void TripleDES::TriDesEncrypt(unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned short SrcLen, unsigned char *Dest)
{

	unsigned char result[1024], temp[1024];
	
	DES_Encrypt(&Key[0],8,Source,SrcLen ,temp);
	DES_Decrypt(&Key[8],8,temp,SrcLen,result);
	DES_Encrypt(&Key[0],8,result,SrcLen ,Dest);

	//HexToChar(Dest,temp,SrcLen);
	Dest[SrcLen]=0x00;
}

void TripleDES::TriDesDecrypt(unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned short SrcLen, unsigned char *Dest)
{
	unsigned char result[1024],temp[1024];

	//CharToHex((char *)Source,temp,SrcLen);

	DES_Decrypt(&Key[0],8,Source,SrcLen ,result);
	DES_Encrypt(&Key[8],8,result,SrcLen,temp);
	DES_Decrypt(&Key[0],8,temp,SrcLen ,Dest);

	//HexToChar(Dest,result,SrcLen);
	Dest[SrcLen]=0x00;
}

void TripleDES::des0(int argc, BYTE *src, BYTE *key, BYTE *result)
{
	BYTE	source[64], dest[64], key_main[64];
	int		flag, i, j;
	
	if (argc == 0)
	{
		flag = 0;
	}
	else
	{
		flag = 1 ;
	}

	for (i=0; i<8; i++)
	{
		dtob(src[i],source+i*8);
		dtob(key[i],key_main+i*8);
	}
	des_algo(source, dest, key_main, flag);
	for (i=0; i<8; i++)
	{
		result[i] = 0;
		for (j=0;j<8;j++)
		{
			result[i] |= (1<<(7-j))*dest[8*i+j];
		}
	}
}

void TripleDES::circle(BYTE *str, int leng)	// to complete left circel shift
{                                           // 1 bit per time
	BYTE	tmp;
	int		i;

	tmp = str[0];
	for (i=0; i<leng-1; i++)
	{
		str[i] = str[i+1];
	}
	str[leng-1] = tmp;
}

void TripleDES::dtob(BYTE dd,BYTE *hh)
{
	int i;

	for (i=0;i<8;i++)
	{
		hh[i] = 0;
		if (dd > 0x7F)
		{
			hh[i] = 1;
		}
		dd = dd << 1;
	}
}

void TripleDES::des_algo(BYTE *src, BYTE *dst, BYTE *key, int flag)
{
	BYTE	sub_key[48], tmp[32], buffer[48];//, cal_tmp[32];
	BYTE	left[32], right[32];
	int		i;
	BYTE	IP[] = 
	{
	57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7,
	56, 48, 40, 32, 24, 16,  8,  0, 58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4, 62, 54, 46, 38, 30, 22, 14,  6,
	255};
	BYTE	IP_1[] = 
	{
	39,  7, 47, 15, 55, 23, 63, 31, 38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29, 36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27, 34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25, 32,  0, 40,  8, 48, 16, 56, 24,
	255};
	BYTE	E[] = 
	{
	31,  0,  1,  2,  3,  4,  3,  4,  5,  6,  7,  8,  7,  8,  9, 10,
	11, 12,	11, 12, 13, 14, 15, 16, 15, 16, 17, 18, 19, 20, 19, 20,
	21, 22, 23, 24,	23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31,  0,
	255};
	BYTE	P[] = 
	{
	15,  6, 19, 20, 28, 11, 27, 16,  0, 14, 22, 25,  4, 17, 30,  9,
	 1,  7, 23, 13, 31, 26,  2,  8, 18, 12, 29,  5, 21, 10,  3, 24,
	255};



	transfer(src, dst, IP);

	for (i=0; i<32; i++) {
		left[i] = dst[i];
		right[i] = dst[i+32];				// generate L0, R0
	}

	for (i=0; i<16; i++) {					// the main circle
		if (flag == 0)
			key_generate(key, sub_key, i); 	// generate the sub key
		else                                // flag = 0 is encypher; 1 is decypher
			key_generate(key, sub_key, 15-i);
		str_cpy(right, tmp, 32);			// save RIGHT_half to tmp

		transfer(right, buffer, E);			// R(i-1) from 32 bit to 48 bit
		str_xor(sub_key, buffer, 48);		// then XOR with k(i)
		S_change(buffer);            		// change result with S-table
		transfer(buffer, right, P);			// transfer using P-table ==> f

		str_xor(left, right, 32);			// generate R(i)
		str_cpy(tmp, left, 32);				// generate L(i)
	}

	for (i=0; i<32; i++) {
		src[i] = right[i];
		src[32+i] = left[i];
	}                       				// generate source(R[15]L[15])

	transfer(src, dst, IP_1);				// get result in dst
}


void TripleDES::transfer(BYTE *src, BYTE *dst, BYTE *table)
{
	int	table_leng, i;
	
	table_leng = 0;
	while (table[table_leng] != 255)
	{
		table_leng++;
	}
	
	for (i=0; i<table_leng; i++) 
	{
		dst[i] = src[table[i]];
	}
}


void TripleDES::str_xor(BYTE *src, BYTE *dst, int len)
{
	int	i;

	for (i=0; i<len; i++)
		dst[i] = (src[i] + dst[i]) % 2;
}

void TripleDES::str_cpy(BYTE *src, BYTE *dst, int len)
{
	int	i;

	for (i=0; i<len; i++)
		dst[i] = src[i];
}
void TripleDES::S_change(BYTE *str)
{
	BYTE	src[8][6], dst[8][4];
	int		i, j, adr;
	BYTE	S[8][64] = {
	{14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	  0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	  4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	 },

	{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	  3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	  0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	 13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	 },

	{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	 13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	 13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	  1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	 },

	{ 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	 13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	 10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	  3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	 },

	{ 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	 14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	  4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	 11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	 },

	{12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	 10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	  9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	  4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	 },

	{ 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	 13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	  1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	  6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	 },

	{13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	  1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	  7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	  2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}};
	
	for (i=0; i<8; i++) 
		for (j=0; j<6; j++) 
			src[i][j] = str[i*6+j];				// seperate to 8 parts

	for (i=0; i<8; i++) {
		j = src[i][1]*8+src[i][2]*4+src[i][3]*2+src[i][4];
		adr = (src[i][0]*2+src[i][5])*16 + j;
		j = S[i][adr];
		dst[i][0] = j / 8;
		j %= 8;
		dst[i][1] = j / 4;
		j %= 4;
		dst[i][2] = j / 2;
		dst[i][3] = j % 2;                      // tansfer 6 bits to 4 bits per part
	}

	for (i=0; i<8; i++) 
		for (j=0; j<4; j++)
			str[i*4+j] = dst[i][j];             // link them to 32 bits
}
void TripleDES::key_generate(BYTE *key_in, BYTE *key_sub, int cnt)
{
	BYTE	buffer[56];
	BYTE	C0[28], D0[28];
	int				i;
	BYTE	shift[] = {
	 1,  2,  4,  6,  8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28,};
	BYTE	PC_1[] = {
	56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35,
	62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3,
	255};
	BYTE	PC_2[] = {
	13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9, 22, 18, 11,  3,
	25, 7,	15,  6, 26, 19, 12,  1, 40, 51, 30, 36, 46, 54, 29, 39,
	50, 44,	32, 47,	43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31,
	255};


	transfer(key_in, buffer, PC_1);

	for (i=0; i<28; i++) {
		C0[i] = buffer[i];
		D0[i] = buffer[i+28];
	}
	for (i=0; i<shift[cnt]; i++) 
	{
		circle(C0, 28);
		circle(D0, 28);
	}
	for (i=0; i<28; i++) 
	{
		buffer[i] = C0[i];
		buffer[i+28] = D0[i];
	}

	transfer(buffer, key_sub, PC_2);
}

void TripleDES::MAC0(BYTE *data, int length, BYTE *key, BYTE * random, BYTE *mac)
{
	int i,j,len;
	BYTE sou[8];
	for (i=0;i<4;i++)
	{
		mac[i] = random[i];
	}
	for (i=4;i<8;i++)
	{
		mac[i] = 0;
	}
	data[length] = 0x80;
	for (i=length+1;i<length+8;i++)
	{
		data[i] = 0;
	}
	len = length / 8 +1;
	for (i=0;i<len;i++) 
	{
		for (j=0;j<8;j++) 
		{
			sou[j] = mac[j] ^ data[i*8+j];
		}
		des0(0,sou,key,mac);
	}
	for (j=0;j<8;j++) 
	{
		sou[j] = mac[j];
	}
	des0(1,sou,key+8,mac);
	for (j=0;j<8;j++) 
	{
		sou[j] = mac[j];
	}
	des0(0,sou,key,mac);
}

void TripleDES::CalMAC(unsigned char* pszKey, int iKeyLen, unsigned char* pszVector, unsigned char* pszSrc, int iSrcLen, unsigned char* pszMAC)
{
	unsigned char szTemp[10], szData[1024];
	unsigned char szMAC[10];
	int i,j,iLen;

	memset(szTemp, 0, sizeof(szTemp));
	memset(szData, 0, sizeof(szData));
	memset(szMAC, 0, sizeof(szMAC));

	memcpy(szData, pszSrc, iSrcLen);
	memcpy(szMAC, pszVector, 8);

	szData[iSrcLen] = 0x80;

	iLen = iSrcLen + 1;

	while(iLen%8 != 0)
	{
		szData[iLen] = 0x0;
		++iLen;
	}

	//
	for(i = 0; i != iLen; i += 8)
	{
		for(j = 0; j != 8; ++j)
		{
			szTemp[j] = szMAC[j] ^ szData[i + j];
		}
		DES_Encrypt(pszKey, 8, szTemp, 8, szMAC);
	}

	if(iKeyLen == 16)
	{
		DES_Decrypt(pszKey + 8, 8, szMAC, 8, szTemp);
		DES_Encrypt(pszKey, 8, szTemp, 8, szMAC);
	}

	memcpy(pszMAC, szMAC, 4);

	pszMAC[4] = 0;
}

// PBOC key diversite
void TripleDES::KeyDiv(unsigned char* pszKey, unsigned char* pszDiv, unsigned char* pszSubKey)
{
	unsigned char szTemp[17];

	memset(szTemp, 0, sizeof(szTemp));

	memcpy(szTemp, pszDiv, 8);

	for(int i = 0; i != 8; ++i)
	{
		szTemp[i + 8] = ~szTemp[i];
	}

	TriDesEncrypt(pszKey, 16, szTemp, 16, pszSubKey);
	pszSubKey[16] = 0;
}
