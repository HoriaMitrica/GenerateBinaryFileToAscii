#pragma once
#pragma pack(1)
#define MAX_LENGTH 30
typedef struct
{
    char sStreet[MAX_LENGTH+1];
    char sCity[MAX_LENGTH+1];
    char sCountry[MAX_LENGTH+1];
} AddressTmpl;

typedef enum
{
    Male = 1,
    Female = 2
} Gender;

typedef struct 
{
    int nPersonID;
    char chRecordType;
#define REC_TYPE_PERSONAL 1
#define REC_TYPE_HOME 2
#define REC_TYPE_WORK 3

    union
    {
        struct /* record type 1 */
        {
            char sFirstName[MAX_LENGTH+1];
            char sLastName[MAX_LENGTH+1];
            char sBirthdate[MAX_LENGTH/3+1]; /* YYYY/MM/DD date format */
            Gender theGender;
        } PersonalInfo;

        struct /* record type 2 */
        {
            AddressTmpl homeAddress;
            char sPhone[MAX_LENGTH/2+1];
            char sEmail[MAX_LENGTH+1];
        } HomeDetails;

        struct /* record type 3 */
        {
            char sCompany[MAX_LENGTH+1];
            AddressTmpl workAddress;
            char sPhone[MAX_LENGTH/2+1];
            char sFax[MAX_LENGTH/2+1];
            char sEmail[MAX_LENGTH+1];
        } WorkDetails;
    } Details;
}PersonTmpl;
#pragma pack()
