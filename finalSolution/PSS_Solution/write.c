#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "person.h"

#define INI_FILENAME "testc.ini"
#define SECTION_NAME "[testc]"
#define MAX_BUFFER_SIZE 256

int parseIniFile(char *inPath, char *outPath);

void writeBinaryToAscii(FILE *binFile, FILE *asciiFile);

int main() {
    char inPath[MAX_BUFFER_SIZE] = {0};
    char outPath[MAX_BUFFER_SIZE] = {0};

    if (parseIniFile(inPath, outPath) != 0) {
        fprintf(stderr, "Failed to parse ini file\n");
        return -1;
    }

    FILE *binFile = fopen(inPath, "rb");
    FILE *asciiFile = fopen(outPath, "w");

    if (!binFile || !asciiFile) {
        perror("Failed to open file");
        return -1;
    }
    writeBinaryToAscii(binFile, asciiFile);
    printf("Finished writing data\nPress any key to exit the program....");

    fclose(asciiFile);
    fclose(binFile);
    asciiFile = NULL;
    binFile = NULL;
    getchar();
    return 0;
}

int parseIniFile(char *inPath, char *outPath) {
    FILE *file = fopen(INI_FILENAME, "r");
    if (!file) {
        perror("error opening .ini file");
        return -1;
    }
    char line[MAX_BUFFER_SIZE];
    bool inSection = false;
    while (fgets(line, MAX_BUFFER_SIZE, file)) {
        if (strncmp(line, SECTION_NAME, 7) == 0) {
            inSection = true;
            continue;
        }

        if (inSection) {
            if (strncmp(line, "in", 2) == 0) {
                char *value = strchr(line, '"');
                if (value) {
                    value++;
                    strncpy(inPath, value, strlen(value) - 2);
                    inPath[strlen(inPath) + 1] = '\0';
                }
            }
            if (strncmp(line, "out", 3) == 0) {
                char *value = strchr(line, '"');
                if (value) {
                    value++;
                    strncpy(outPath, value, strlen(value) - 1);
                    outPath[strlen(outPath) + 1] = '\0';
                }
            }
        }
    }
    fclose(file);
    file = NULL;
    return 0;
}

void writeBinaryToAscii(FILE *binFile, FILE *asciiFile) {
    PersonTmpl person_buffer;
    while (fread(&person_buffer, sizeof(person_buffer), 1, binFile) == 1) {
        switch (person_buffer.chRecordType - '0') {
            case 1:
                fprintf(
                    asciiFile,
                    "%4d%1c%30s%30s%10s%1c\n",
                    person_buffer.nPersonID,
                    person_buffer.chRecordType,
                    person_buffer.Details.PersonalInfo.sFirstName,
                    person_buffer.Details.PersonalInfo.sLastName,
                    person_buffer.Details.PersonalInfo.sBirthdate,
                    person_buffer.Details.PersonalInfo.theGender == 1 ? 'M' : 'F'
                );
                break;
            case 2: fprintf(
                    asciiFile,
                    "%4d%1c%30s%30s%30s%15s%30s\n",
                    person_buffer.nPersonID, person_buffer.chRecordType,
                    person_buffer.Details.HomeDetails.homeAddress.sCountry,
                    person_buffer.Details.HomeDetails.homeAddress.sCity,
                    person_buffer.Details.HomeDetails.homeAddress.sStreet, person_buffer.Details.HomeDetails.sPhone,
                    person_buffer.Details.HomeDetails.sEmail);
                break;
            case 3: fprintf(
                    asciiFile,
                    "%4d%1c%30s%30s%30s%30s%15s%15s%30s\n",
                    person_buffer.nPersonID, person_buffer.chRecordType, person_buffer.Details.WorkDetails.sCompany,
                    person_buffer.Details.WorkDetails.workAddress.sCountry,
                    person_buffer.Details.WorkDetails.workAddress.sCity,
                    person_buffer.Details.WorkDetails.workAddress.sStreet, person_buffer.Details.WorkDetails.sPhone,
                    person_buffer.Details.WorkDetails.sFax, person_buffer.Details.WorkDetails.sEmail);
                break;
            default: break;
        }
    }
}
