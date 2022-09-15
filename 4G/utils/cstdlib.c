#include "cstdlib.h"
//����ת�����ַ���
char* itoa(int num,char* str,int radix)
{/*������*/
    char index[]="0123456789ABCDEF";
    unsigned unum;/*�м����*/
    int i=0,j,k;
    /*ȷ��unum��ֵ*/
    if(radix==10&&num<0)/*ʮ���Ƹ���*/
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;/*�������*/
    /*ת��*/
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
       }while(unum);
    str[i]='\0';
    /*����*/
    if(str[0]=='-')
        k=1;/*ʮ���Ƹ���*/
    else
        k=0;
     
    for(j=k;j<=(i-1)/2;j++)
    {       char temp;
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}


//����ת�����ַ���
char* AsciiHexNumber(uint8_t num,char* str)
{
	uint16_t Hbuffer,Lbuffer;
	Hbuffer= num&0xF0;
	Lbuffer= num&0x0F;
	if(Hbuffer!=0&&Lbuffer!=0)
	{
		itoa(num,str,16);
	}
	else 
	{
		if(Hbuffer==0)
		{
			*(str)='0';
			itoa(num,str+1,16);		
		}
		else if(Lbuffer==0)
		{
			*(str+1)='0';
			itoa(num,str,16);	
		}
		else
		{
		*(str+1)=*(str)='0';
		}
	}
	return str;
}


//void AscIIToHex(char *Ascchar ,unsigned char *hex)
//{
//		if(Ascchar>=48&&Ascchar<=57)
//		{
//		*hex=Ascchar-48;
//		}			
//}

/*mode : 1 small_mode  or 0 big_mode*/
void Bitmerge(bool mode,uint8_t source_data[4],uint32_t *data)
{
	uint8_t var=0;
	*data=0;
	if(mode)
	{

		*data|=source_data[var+3];
		*data<<=8;
		*data|=source_data[var+2];
		*data<<=8;
		*data|=source_data[var+1];
		*data<<=8;
		*data|=source_data[var];
		
	}
}	

