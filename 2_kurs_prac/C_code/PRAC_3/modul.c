#include "modul.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//BOM: 1 (LE), 0 (BE), -1 (error)
int ByteOrder_On_CommandLine(int argc, char** argv)
{
    int buff = 1; //для BOM
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-le") == 0) {
            for (int j = i+1; j < argc; j++) {
                if (strcmp(argv[j], "-be") == 0) {
                    printf("Error: -le and -be\n");
                    return -1;
                }
            }
            buff = 1;
        }
        else if (strcmp(argv[i], "-be") == 0) {
            for (int j = i+1; j < argc; j++) {
                if (strcmp(argv[j], "-le") == 0) {
                    printf("Error: -be and -le\n");
                    return -1;
                }
            }
            buff = 0;
        }
    }
    return buff;
}

void i_o_flags(int argc, char** argv, char** file_Inp, char** file_Out)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            if (i+1 < argc) {
                *(file_Inp) = argv[i+1];
            }
        }
        if (strcmp(argv[i], "-o") == 0) {
            if (i+1 < argc) {
                *(file_Out) = argv[i+1];
            }
        }
    }
    return;
}
