#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"



int main(int argc, char *argv[])
{
    FILE *openfile;
    FILE *writefile;
    
    printf("enter 32 chars: ");
    scanf("%[^\n]", &pass);
    //printf("you enter: %s\n", pass);
    printf("encrypt or decrypt? (e/d)\n");
    scanf(" %c", &encdec);
    
    switch(encdec)
    {
        case 'e':
            printf("enter filename to encrypt\n");
            fflush(stdin);
            fflush(stdout);
            scanf("%[^\n]", &filein);
            printf("you enter: %s\n", filein);
            printf("enter filename to save\n");
            fflush(stdin);
            fflush(stdout);
            scanf("%[^\n]", &fileout);
            printf("you enter: %s\n", fileout);
            printf("working...\n");

            KeyExpansion();
            openfile = fopen(filein, "rb");
            writefile = fopen(fileout, "wb");

            
            fread(buffer2, 1, 16, openfile);
            //fseek(openfile, 16, SEEK_SET);
            fread(buffer1, 1, 16, openfile);
            Encrypt(buffer2);
            fwrite(buffer2, 1, 16, writefile);
            for(int i=0; i<16; i++)
            {
                buffer1[i] ^= buffer2[i];
            }
            Encrypt(buffer1);
            fwrite(buffer1, 1, 16, writefile);

            while(1)
            {
                for(int i=0; i<16; i++)
                {
                    buffer2[i] = buffer1[i];
                }

                if(!fread(buffer1, 1, 16, openfile))
                    break;
                
                for(int i=0; i<16; i++)
                {
                    buffer1[i] ^= buffer2[i];
                }
                Encrypt(buffer1);
                fwrite(buffer1, 1, 16, writefile);
            }

            printf("finish\n");
            break;
        case 'd':
            printf("enter filename to decrypt\n");
            fflush(stdin);
            fflush(stdout);
            scanf("%[^\n]", &filein);
            printf("you enter: %s\n", filein);
            printf("enter filename to save\n");
            fflush(stdin);
            fflush(stdout);
            scanf("%[^\n]", &fileout);
            printf("you enter: %s\n", fileout);
            printf("working...\n");

            KeyExpansion();
            openfile = fopen(filein, "rb");
            writefile = fopen(fileout, "wb");

            while(fread(buffer1, 1, 16, openfile))
            {
                Decrypt(buffer1);
                fwrite(buffer1, 1, 16, writefile);
            }


            printf("finish\n");
            break;
        default:
            printf("error wrong character\n");
    }
    
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
void KeyExpansion()
{
    uchar temp[4];
    int rconCounter = 1;
    
    for(int i=0; i<16; i++)
    {
        expandedKey[0][i] = pass[i];
        expandedKey[1][i] = pass[i+16];
    }
    
    for(int i=2; i<14; i++)
    {
        temp[0] = expandedKey[i-1][12];
        temp[1] = expandedKey[i-1][13];
        temp[2] = expandedKey[i-1][14];
        temp[3] = expandedKey[i-1][15];
        
        uchar t = temp[0];
        temp[0] = temp[1];
        temp[1] = temp[2];
        temp[2] = temp[3];
        temp[3] = t;
        
        temp[0] = sbox[temp[0]];
        temp[1] = sbox[temp[1]];
        temp[2] = sbox[temp[2]];
        temp[3] = sbox[temp[3]];
        
        temp[0] ^= rcon[rconCounter++];
        
        expandedKey[i][0] = expandedKey[i-1][0] ^ temp[0];
        expandedKey[i][1] = expandedKey[i-1][1] ^ temp[1];
        expandedKey[i][2] = expandedKey[i-1][2] ^ temp[2];
        expandedKey[i][3] = expandedKey[i-1][3] ^ temp[3];
        //-----------------------------
        expandedKey[i][4] = expandedKey[i-1][4] ^ expandedKey[i][0];
        expandedKey[i][5] = expandedKey[i-1][5] ^ expandedKey[i][1];
        expandedKey[i][6] = expandedKey[i-1][6] ^ expandedKey[i][2];
        expandedKey[i][7] = expandedKey[i-1][7] ^ expandedKey[i][3];

        expandedKey[i][8] = expandedKey[i-1][8] ^ expandedKey[i][4];
        expandedKey[i][9] = expandedKey[i-1][9] ^ expandedKey[i][5];
        expandedKey[i][10] = expandedKey[i-1][10] ^ expandedKey[i][6];
        expandedKey[i][11] = expandedKey[i-1][11] ^ expandedKey[i][7];
        
        expandedKey[i][8] = sbox[expandedKey[i][8]];
        expandedKey[i][9] = sbox[expandedKey[i][9]];
        expandedKey[i][10] = sbox[expandedKey[i][10]];
        expandedKey[i][11] = sbox[expandedKey[i][11]];
        
        expandedKey[i][12] = expandedKey[i-1][12] ^ expandedKey[i][8];
        expandedKey[i][13] = expandedKey[i-1][13] ^ expandedKey[i][9];
        expandedKey[i][14] = expandedKey[i-1][14] ^ expandedKey[i][10];
        expandedKey[i][15] = expandedKey[i-1][15] ^ expandedKey[i][11];
    }
}
////////////////////////////////////////////////////////////////////////////////
void SubBytes(uchar *block)
{
    for(int i=0; i<16; i++)
    {
        block[i] = sbox[block[i]];
    }
}
////////////////////////////////////////////////////////////////////////////////
void InvSubBytes(uchar *block)
{
    for(int i=0; i<16; i++)
    {
        block[i] = invsbox[block[i]];
    }
}
////////////////////////////////////////////////////////////////////////////////
void ShiftRows(uchar *block)
{
    
    uchar temp[16];
    
    temp[0] = block[0];
    temp[1] = block[5];
    temp[2] = block[10];
    temp[3] = block[15];
    
    temp[4] = block[4];
    temp[5] = block[9];
    temp[6] = block[14];
    temp[7] = block[3];
    
    temp[8] = block[8];
    temp[9] = block[13];
    temp[10] = block[2];
    temp[11] = block[7];
    
    temp[12] = block[12];
    temp[13] = block[1];
    temp[14] = block[6];
    temp[15] = block[11];
    
    for(int i=0; i<16; i++){
        block[i] = temp[i];
    }
}
////////////////////////////////////////////////////////////////////////////////
void InvShiftRows(uchar *block)
{
    uchar temp[16];
    
    temp[0] = block[0];
    temp[1] = block[13];
    temp[2] = block[10];
    temp[3] = block[7];
    
    temp[4] = block[4];
    temp[5] = block[1];
    temp[6] = block[14];
    temp[7] = block[11];
    
    temp[8] = block[8];
    temp[9] = block[5];
    temp[10] = block[2];
    temp[11] = block[15];
    
    temp[12] = block[12];
    temp[13] = block[9];
    temp[14] = block[6];
    temp[15] = block[3];
    
    for(int i=0; i<16; i++)
    {
        block[i] = temp[i];
    }
}
////////////////////////////////////////////////////////////////////////////////
void AddRoundKey(uchar *block, uchar key[])
{
    for(int i=0; i<16; i++)
    {
        block[i] ^= key[i];
    }
}
////////////////////////////////////////////////////////////////////////////////
void MixColumns(uchar *block)
{
    uchar tmp[16];
    
    tmp[0] = mul2[block[0]] ^ mul3[block[1]] ^ block[2] ^ block[3]; 
    tmp[1] = block[0] ^ mul2[block[1]] ^ mul3[block[2]] ^ block[3]; 
    tmp[2] = block[0] ^ block[1] ^ mul2[block[2]] ^ mul3[block[3]]; 
    tmp[3] = mul3[block[0]] ^ block[1] ^ block[2] ^ mul2[block[3]]; 
    
    tmp[4] = mul2[block[4]] ^ mul3[block[5]] ^ block[6] ^ block[7]; 
    tmp[5] = block[4] ^ mul2[block[5]] ^ mul3[block[6]] ^ block[7]; 
    tmp[6] = block[4] ^ block[5] ^ mul2[block[6]] ^ mul3[block[7]]; 
    tmp[7] = mul3[block[4]] ^ block[5] ^ block[6] ^ mul2[block[7]]; 
    
    tmp[8] = mul2[block[8]] ^ mul3[block[9]] ^ block[10] ^ block[11]; 
    tmp[9] = block[8] ^ mul2[block[9]] ^ mul3[block[10]] ^ block[11]; 
    tmp[10] = block[8] ^ block[9] ^ mul2[block[10]] ^ mul3[block[11]]; 
    tmp[11] = mul3[block[8]] ^ block[9] ^ block[10] ^ mul2[block[11]]; 
    
    tmp[12] = mul2[block[12]] ^ mul3[block[13]] ^ block[14] ^ block[15]; 
    tmp[13] = block[12] ^ mul2[block[13]] ^ mul3[block[14]] ^ block[15]; 
    tmp[14] = block[12] ^ block[13] ^ mul2[block[14]] ^ mul3[block[15]]; 
    tmp[15] = mul3[block[12]] ^ block[13] ^ block[14] ^ mul2[block[15]];
    
    for(int i=0; i<16; i++)
    {
        block[i] = tmp[i];
    }
}
////////////////////////////////////////////////////////////////////////////////
void InvMixColumns(uchar *block)
{
    uchar tmp[16];
    
    tmp[0] = mul14[block[0]] ^ mul11[block[1]] ^ mul13[block[2]] ^ mul9[block[3]]; 
    tmp[1] = mul9[block[0]] ^ mul14[block[1]] ^ mul11[block[2]] ^ mul13[block[3]]; 
    tmp[2] = mul13[block[0]] ^ mul9[block[1]] ^ mul14[block[2]] ^ mul11[block[3]]; 
    tmp[3] = mul11[block[0]] ^ mul13[block[1]] ^ mul9[block[2]] ^ mul14[block[3]]; 
    
    tmp[4] = mul14[block[4]] ^ mul11[block[5]] ^ mul13[block[6]] ^ mul9[block[7]]; 
    tmp[5] = mul9[block[4]] ^ mul14[block[5]] ^ mul11[block[6]] ^ mul13[block[7]]; 
    tmp[6] = mul13[block[4]] ^ mul9[block[5]] ^ mul14[block[6]] ^ mul11[block[7]]; 
    tmp[7] = mul11[block[4]] ^ mul13[block[5]] ^ mul9[block[6]] ^ mul14[block[7]]; 
    
    tmp[8] = mul14[block[8]] ^ mul11[block[9]] ^ mul13[block[10]] ^ mul9[block[11]]; 
    tmp[9] = mul9[block[8]] ^ mul14[block[9]] ^ mul11[block[10]] ^ mul13[block[11]]; 
    tmp[10] = mul13[block[8]] ^ mul9[block[9]] ^ mul14[block[10]] ^ mul11[block[11]]; 
    tmp[11] = mul11[block[8]] ^ mul13[block[9]] ^ mul9[block[10]] ^ mul14[block[11]];
    
    tmp[12] = mul14[block[12]] ^ mul11[block[13]] ^ mul13[block[14]] ^ mul9[block[15]]; 
    tmp[13] = mul9[block[12]] ^ mul14[block[13]] ^ mul11[block[14]] ^ mul13[block[15]]; 
    tmp[14] = mul13[block[12]] ^ mul9[block[13]] ^ mul14[block[14]] ^ mul11[block[15]]; 
    tmp[15] = mul11[block[12]] ^ mul13[block[13]] ^ mul9[block[14]] ^ mul14[block[15]]; 
    
    for(int i=0; i<16; i++)
    {
        block[i] = tmp[i];
    }
}
////////////////////////////////////////////////////////////////////////////////
void Encrypt(uchar *block)
{
    AddRoundKey(block, expandedKey[0]);
    
    for(int i=1; i<=12; i++)
    {
        SubBytes(block);
        ShiftRows(block);
        MixColumns(block);
        AddRoundKey(block, expandedKey[i]);
    }
    
    SubBytes(block);
    ShiftRows(block);
    AddRoundKey(block, expandedKey[13]);
}
////////////////////////////////////////////////////////////////////////////////
void Decrypt(unsigned char *block)
{
    AddRoundKey(block, expandedKey[13]);
    InvShiftRows(block);
    InvSubBytes(block);
    
    for(int i=12; i>=1; i--)
    {
        AddRoundKey(block, expandedKey[i]);
        InvMixColumns(block);
        InvShiftRows(block);
        InvSubBytes(block);
    }
    
    AddRoundKey(block, expandedKey[0]);
}
////////////////////////////////////////////////////////////////////////////////







