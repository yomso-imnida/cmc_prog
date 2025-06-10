#include "modul.h" //подключаем модуль, наход. в одной директ. с прогр.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    /*-----------------Input, Output-----------------*/
    char* Inp;
    char* Out;
    i_o_flags(argc, argv, &Inp, &Out);
    FILE* InputFile = fopen(Inp, "r");
    if (InputFile == NULL)
        InputFile = stdin;
    FILE* OutputFile = fopen(Out, "w");
    if (OutputFile == NULL)
        OutputFile = stdout;
    /*----------------------BOM----------------------*/
    int BOM_FL = 1; //по умолчанию (LE-порядок)
    //1 - LE-порядок, 0 - BE-порядок
    BOM_FL = ByteOrder_On_CommandLine(argc, argv);
    if (BOM_FL == -1)
        return 1;
    unsigned short BOM;
    if (BOM_FL == 1)
        BOM = 0xFFFE;
    else
        BOM = 0xFEFF;
    BOM = (BOM << 8) | (BOM >> 8);
    
    fwrite(&BOM, sizeof(unsigned short), 1, OutputFile);
    /*----------------------Convert----------------------*/
    unsigned char utf8;
    unsigned short utf16;
    //если считали < 1 объект, т.е. 0 => конец текста
    while (fread(&utf8, sizeof(unsigned char), 1, InputFile) == 1)
    {
        int damaged_unicode = 0; //поврежденный юникод
        utf16 = 0x0;
        //1 байт; диапазон 0..127
        if ((utf8 >> 7) == 0x0)
            utf16 = (unsigned short)utf8;
        //2 байта; диапазон 128..2047
        else if ((utf8 >> 5) == 0x6) {
            utf16 = (unsigned short)((utf8 & 0x1F) << 6); //0x1F = 0..01 1111
            if (fread(&utf8, sizeof(unsigned char), 1, InputFile) != 1)
                break;
            if ((utf8 >> 6) == 0x2)
                utf16 = utf16 | (utf8 & 0x3F); //0x3F = 0..011 1111
            else
                damaged_unicode = 1;
        }
        //3 байта; диапазон 2048..65535
        else if ((utf8 >> 4) == 0xE) {
            utf16 = (unsigned short)((utf8 & 0xF) << 12);
            if (fread(&utf8, sizeof(unsigned char), 1, InputFile) != 1)
                break;
            if ((utf8 >> 6) == 0x2) {
                utf16 = utf16 | ((utf8 & 0x3F) << 6);
                if (fread(&utf8, sizeof(unsigned char), 1, InputFile) != 1)
                    break;
                if ((utf8 >> 6) == 0x2)
                    utf16 = utf16 | (utf8 & 0x3F);
                else
                    damaged_unicode = 1;
            }
            else
                damaged_unicode = 1;
        }
        //если не увидели ни один из трёх случаев -> битый код
        else
            damaged_unicode = 1;
        if (!BOM_FL) //если BE-порядок
            utf16 = (utf16 << 8) |(utf16 >> 8);
        if (!damaged_unicode)
            fwrite(&utf16, sizeof(unsigned short), 1, OutputFile);
    }
    
    fclose(InputFile);
    fclose(OutputFile);
    return 0;
}
