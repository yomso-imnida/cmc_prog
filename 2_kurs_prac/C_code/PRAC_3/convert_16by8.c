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
    unsigned short BOM = 0xFEFF;
    //куда считать, размер, сколько считать, откуда считать
    fread(&BOM, sizeof(unsigned short), 1, InputFile);
    if (BOM == 0xFEFF)
        BOM_FL = 1; //BOM есть в потоке, LE-порядок
    else if (BOM == 0xFFFE)
        BOM_FL = 0; //BOM есть в потоке, BE-порядок
    //BOM не нашли, порядок задаётся через командную строку
    else
    {
        BOM_FL = ByteOrder_On_CommandLine(argc, argv);
        if (BOM_FL == -1)
            return 1;    }
    /*----------------------Convert----------------------*/
    unsigned short utf16;
    if((BOM != 0xFEFF) && (BOM != 0xFFFE))
        fseek(InputFile, 0, SEEK_SET);
    //если считали < 1 объект, т.е. 0 => конец текста
    while (fread(&utf16, sizeof(unsigned short), 1, InputFile) == 1)
    {
        if (!BOM_FL) //если BE-порядок
            utf16 = (utf16 << 8) |(utf16 >> 8);
        //1 байт; диапазон 0..127, т.е. от U+0000 до U+007F
        if (utf16 <= 0x7F) {
            char utf8 = (char)utf16;
            fwrite(&utf8, sizeof(char), 1, OutputFile);
        }
        //2 байта; диапазон 128..2047, т.е. от U+0080 до U+07FF
        else if (utf16 <= 0x7FF) {
            char utf8[2];
            utf8[0] = 0xC0 | (char)((utf16 >> 6) & 0x1F);
            /*110 0000 = 0xC0, 0..011111 = 0x1F;
            10 000000 = 0x80, 0..0111111 = 0x3F*/
            utf8[1] = 0x80 | (char)(utf16 & 0x3F);
            fwrite(utf8, sizeof(char), 2, OutputFile);
        }
        //3 байта; диапазон 2048..65535, т.е. от U+0800 до U+FFFF
        else {
            char utf8[3];
            utf8[0] = 0xE0 | (char)((utf16 >> 12) & 0xF);
            /*1110 0000 = 0xE0, 0..01111 = 0xF*/
            utf8[1] = 0x80 | (char)((utf16 >> 6) & 0x3F);
            utf8[2] = 0x80 | (char)(utf16 & 0x3F);
            fwrite(&utf8, sizeof(char), 3, OutputFile);
        }
    }

    fclose(InputFile);
    fclose(OutputFile);
    return 0;
}
