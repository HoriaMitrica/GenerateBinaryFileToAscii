#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#include "person.h"

#define INI_FILE "testc.ini"
#define SECTION_NAME "[testc]"
#define RANDOM_VALUES_FILE "RandomValues.txt"
#define MAX_BUFFER_SIZE 256
#define MAX_CHOICES 9

int parseIniFile(char *inPath);

void clearInputBuffer();

int isValidChoice(int choice, int const validChoices[], int length);

void askForInput(int *choice, int validChoices[], int length);

int validEmail(const char *input);

int validDate(const char *input);

void getRandomValueFromSection(FILE *file, const char *section, char *output);

void handleInput(char *input, int size);

void addPersonalDetails(PersonTmpl *person, bool isRandom);

void addHomeDetails(PersonTmpl *person,bool isRandom);

void addWorkDetails(PersonTmpl *person,bool isRandom);

void addPersonToFile(FILE *file, const PersonTmpl *person);

void addPerson(FILE *file, PersonTmpl *person, int id, bool isRandom);

int main() {
    char inPath[MAX_BUFFER_SIZE] = {0};
    if (parseIniFile(inPath) != 0) {
        fprintf(stderr, "Failed to parse ini file\n");
        return -1;
    }

    FILE *binFile = fopen(inPath, "wb");
    if (!binFile) {
        perror("Failed to open file");
        return -1;
    }
    PersonTmpl *person = malloc(sizeof(PersonTmpl));

    int id = 1;
    int choice = 0;
    int validChoices[] = {1, 2, 3};
    while (choice != 3) {
        printf(
            "Choose how you want to write to the in file\n1.Randomly Generated\n2.Introduce persons yourself\n3.Quit program\n");
        askForInput(&choice, validChoices, sizeof(validChoices) / sizeof(int));
        printf("\x1b[2J");
        printf("\x1b[H");

        switch (choice) {
            case 1: {
                srand(time(NULL));
                int amount = 0;
                printf("How many persons do you want to generate? (you can type 0 to go back)\n");
                scanf("%d", &amount);
                clearInputBuffer();
                for (int i = 0; i < amount; i++) {
                    addPerson(binFile, person, id,true);
                    id++;
                }
                break;
            }
            case 2: {
                addPerson(binFile, person, id,false);
                id++;
                break;
            }
            default: break;
        }
    }

    fclose(binFile);
    free(person);
    person = NULL;
    binFile = NULL;
    return 0;
}

int parseIniFile(char *inPath) {
    FILE *file = fopen(INI_FILE, "r");
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
        }
    }
    fclose(file);
    file = NULL;
    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int isValidChoice(const int choice, int const validChoices[], int length) {
    for (int i = 0; i < length; i++) {
        if (choice == validChoices[i]) {
            return 1;
        }
    }
    return 0;
}

void askForInput(int *const choice, int validChoices[], const int length) {
    while (1) {
        char c = getchar();
        if (c <= '9' && c >= '0' && isValidChoice(c - '0', validChoices, length)) {
            *choice = c - '0';
            clearInputBuffer();
            break;
        }
        clearInputBuffer();
        printf("Choice is not among valid options.\n");
    }
}

void getRandomValueFromSection(FILE *file, const char *section, char *output) {
    char line[MAX_BUFFER_SIZE];
    int count = 0;
    char *values[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, section, strlen(section)) == 0) {
            while (fgets(line, sizeof(line), file) && line[0] != '[') {
                line[strcspn(line, "\n")] = 0;
                if (count < MAX_LENGTH) {
                    values[count] = strdup(line);
                    count++;
                }
            }
            break;
        }
    }
    if (count > 0) {
        int randomIndex = rand() % count;
        strcpy(output, values[randomIndex]);
    }
    for (int i = 0; i < count; i++) {
        free(values[i]);
    }
}

void handleInput(char *const input, const int size) {
    fgets(input, size,stdin);
    if (strlen(input) > 0 && input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }
}

void addPersonalDetails(PersonTmpl *person, const bool isRandom) {
    if (isRandom) {
        FILE *file = fopen(RANDOM_VALUES_FILE, "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }
        char section[MAX_LENGTH] = "[Name]";

        getRandomValueFromSection(file, section, person->Details.PersonalInfo.sFirstName);
        rewind(file);
        getRandomValueFromSection(file, section, person->Details.PersonalInfo.sLastName);
        rewind(file);
        strcpy(section, "[BirthDate]");
        getRandomValueFromSection(file, section, person->Details.PersonalInfo.sBirthdate);
        rewind(file);
        person->Details.PersonalInfo.theGender = rand() % 2 + 1;
        fclose(file);
        file = NULL;
        return;
    }
    char input[31];
    printf("Enter first name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.PersonalInfo.sFirstName, input, sizeof(person->Details.PersonalInfo.sFirstName) - 1);
    person->Details.PersonalInfo.sFirstName[sizeof(person->Details.PersonalInfo.sFirstName) - 1] = '\0';
    printf("Enter last name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.PersonalInfo.sLastName, input, sizeof(person->Details.PersonalInfo.sLastName) - 1);
    person->Details.PersonalInfo.sLastName[sizeof(person->Details.PersonalInfo.sLastName) - 1] = '\0';
    printf("Enter birthdate (DD-MM-YYYY)\n");
    handleInput(input, sizeof(input) / 3 + 1);

    while (!validDate(input)) {
        clearInputBuffer();
        printf("Invalid input\n");
        handleInput(input, sizeof(input) / 3 + 1);
    }
    clearInputBuffer();
    strncpy(person->Details.PersonalInfo.sBirthdate, input, sizeof(person->Details.PersonalInfo.sBirthdate) - 1);
    person->Details.PersonalInfo.sBirthdate[sizeof(person->Details.PersonalInfo.sBirthdate) - 1] = '\0';
    int choice = 0;
    int validChoices[] = {1, 2};
    printf("Choose Gender:\n1.Male\n2.Female\n");

    askForInput(&choice, validChoices, sizeof(validChoices) / sizeof(int));
    person->Details.PersonalInfo.theGender = choice;
}

void addHomeDetails(PersonTmpl *person,bool isRandom) {
    if (isRandom) {
        FILE *file = fopen(RANDOM_VALUES_FILE, "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }
        char section[MAX_LENGTH] = "[Country]";
        getRandomValueFromSection(file, section, person->Details.HomeDetails.homeAddress.sCountry);
        rewind(file);
        strcpy(section, "[City]");
        getRandomValueFromSection(file, section, person->Details.HomeDetails.homeAddress.sCity);
        rewind(file);
        strcpy(section, "[Street]");
        getRandomValueFromSection(file, section, person->Details.HomeDetails.homeAddress.sStreet);
        rewind(file);
        strcpy(section, "[Phone]");
        getRandomValueFromSection(file, section, person->Details.HomeDetails.sPhone);
        rewind(file);
        strcpy(section, "[Email]");
        getRandomValueFromSection(file, section, person->Details.HomeDetails.sEmail);
        fclose(file);
        file = NULL;
        return;
    }
    char input[31];
    printf("Enter home address country name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.HomeDetails.homeAddress.sCountry, input,
            sizeof(person->Details.HomeDetails.homeAddress.sCountry) - 1);
    person->Details.HomeDetails.homeAddress.sCountry[sizeof(person->Details.HomeDetails.homeAddress.sCountry) - 1] =
            '\0';
    printf("Enter home address city name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.HomeDetails.homeAddress.sCity, input,
            sizeof(person->Details.HomeDetails.homeAddress.sCity) - 1);
    person->Details.HomeDetails.homeAddress.sCity[sizeof(person->Details.HomeDetails.homeAddress.sCity) - 1] = '\0';
    printf("Enter home address street name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.HomeDetails.homeAddress.sStreet, input,
            sizeof(person->Details.HomeDetails.homeAddress.sStreet) - 1);
    person->Details.HomeDetails.homeAddress.sStreet[sizeof(person->Details.HomeDetails.homeAddress.sStreet) - 1] = '\0';
    printf("Enter home phone number (max 15 characters)\n");
    handleInput(input, sizeof(input) / 2 + 1);
    strncpy(person->Details.HomeDetails.sPhone, input, sizeof(person->Details.HomeDetails.sPhone) - 1);
    person->Details.HomeDetails.sPhone[sizeof(person->Details.HomeDetails.sPhone) - 1] = '\0';
    printf("Enter home email (max 30 characters)\n");
    handleInput(input, sizeof(input));
    while (!validEmail(input)) {
        printf("Invalid input\n");
        handleInput(input, sizeof(input));
    }
    strncpy(person->Details.HomeDetails.sEmail, input, sizeof(person->Details.HomeDetails.sEmail) - 1);
    person->Details.HomeDetails.sEmail[sizeof(person->Details.HomeDetails.sEmail) - 1] = '\0';
}

void addWorkDetails(PersonTmpl *person,bool isRandom) {
    if (isRandom) {
        FILE *file = fopen(RANDOM_VALUES_FILE, "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }
        char section[MAX_LENGTH] = "[Company]";
        getRandomValueFromSection(file, section, person->Details.WorkDetails.sCompany);
        rewind(file);
        strcpy(section, "[Country]");
        getRandomValueFromSection(file, section, person->Details.WorkDetails.workAddress.sCountry);
        rewind(file);
        strcpy(section, "[City]");
        getRandomValueFromSection(file, section, person->Details.WorkDetails.workAddress.sCity);
        rewind(file);
        strcpy(section, "[Street]");
        getRandomValueFromSection(file, section, person->Details.WorkDetails.workAddress.sStreet);
        rewind(file);
        strcpy(section, "[Phone]");
        getRandomValueFromSection(file, section, person->Details.WorkDetails.sPhone);
        rewind(file);
        strcpy(section, "[Fax]");
        getRandomValueFromSection(file, section, person->Details.WorkDetails.sFax);
        rewind(file);
        strcpy(section, "[Email]");
        getRandomValueFromSection(file, section, person->Details.WorkDetails.sEmail);
        rewind(file);
        fclose(file);
        file = NULL;
        return;
    }
    char input[51];
    printf("Enter company name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.WorkDetails.sCompany, input,
            sizeof(person->Details.WorkDetails.sCompany) - 1);
    person->Details.WorkDetails.sCompany[sizeof(person->Details.WorkDetails.sCompany) - 1] =
            '\0';
    printf("Enter work address country name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.WorkDetails.workAddress.sCountry, input,
            sizeof(person->Details.WorkDetails.workAddress.sCountry) - 1);
    person->Details.WorkDetails.workAddress.sCountry[sizeof(person->Details.WorkDetails.workAddress.sCountry) - 1] =
            '\0';
    printf("Enter work address city name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.WorkDetails.workAddress.sCity, input,
            sizeof(person->Details.WorkDetails.workAddress.sCity) - 1);
    person->Details.WorkDetails.workAddress.sCity[sizeof(person->Details.WorkDetails.workAddress.sCity) - 1] = '\0';
    printf("Enter work address street name (max 30 characters)\n");
    handleInput(input, sizeof(input));
    strncpy(person->Details.WorkDetails.workAddress.sStreet, input,
            sizeof(person->Details.WorkDetails.workAddress.sStreet) - 1);
    person->Details.WorkDetails.workAddress.sStreet[sizeof(person->Details.WorkDetails.workAddress.sStreet) - 1] = '\0';
    printf("Enter work phone number (max 15 characters)\n");
    handleInput(input, sizeof(input) / 2 + 1);
    strncpy(person->Details.WorkDetails.sPhone, input, sizeof(person->Details.WorkDetails.sPhone) - 1);
    person->Details.WorkDetails.sPhone[sizeof(person->Details.WorkDetails.sPhone) - 1] = '\0';
    printf("Enter work fax (max 15 characters)\n");
    handleInput(input, sizeof(input) / 2 + 1);
    strncpy(person->Details.WorkDetails.sFax, input, sizeof(person->Details.WorkDetails.sFax) - 1);
    person->Details.WorkDetails.sFax[sizeof(person->Details.WorkDetails.sFax) - 1] = '\0';
    printf("Enter work email (max 30 characters)\n");
    handleInput(input, sizeof(input));
    while (!validEmail(input)) {
        printf("Invalid input\n");
        handleInput(input, sizeof(input));
    }
    strncpy(person->Details.WorkDetails.sEmail, input, sizeof(person->Details.WorkDetails.sEmail) - 1);
    person->Details.WorkDetails.sEmail[sizeof(person->Details.WorkDetails.sEmail) - 1] = '\0';
}

void addPersonToFile(FILE *file, const PersonTmpl *const person) {
    if (fwrite(person, sizeof(*person), 1, file) != 1) {
        perror("Failed to write to file");
    } else {
        printf("Person added successfully!\n");
    }
}

void addPerson(FILE *file, PersonTmpl *person, const int id, const bool isRandom) {
    int choice = 0;
    int validChoices[] = {1, 2, 3, 4};
    person->nPersonID = id;
    if (!isRandom) {
        printf(
            "Choose the type of details you will store for this person\n1.Personal Details\n2.Home Details\n3.Work Details\n4.Go Back\n");
        askForInput(&choice, validChoices, sizeof(validChoices) / sizeof(int));
    } else {
        choice = rand() % 3 + 1;
    }
    const char chChoice = '0' + choice;
    person->chRecordType = chChoice;
    switch (choice) {
        case 1: addPersonalDetails(person, isRandom);
            break;
        case 2: addHomeDetails(person, isRandom);
            break;
        case 3: addWorkDetails(person, isRandom);
            break;
        case 4: {
            printf("\x1b[2J");
            printf("\x1b[H");
        }
            return;
        default:
            return;
    }
    addPersonToFile(file, person);
}

int validEmail(const char *input) {
    char *at_ch = strchr(input, '@');
    if (at_ch == NULL || strchr(input, ' ') != NULL || !isalnum(input[0]))
        return 0;
    at_ch++;
    if (strchr(at_ch, '@') != NULL)
        return 0;
    if (at_ch[0] == '.')
        return 0;
    return 1;
}

int validDate(const char *input) {
    if (strlen(input) != 10) {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if ((i == 2 || i == 5)) {
            if (input[i] != '-') {
                return 0;
            }
        } else {
            if (!isdigit(input[i])) {
                return 0;
            }
        }
    }

    return 1;
}
