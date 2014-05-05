

#ifndef RSPROD_STRING_UTILS
#define RSPROD_STRING_UTILS

int rsprod_string_trim_allNulls(const char *string_orig,char **string_trans);
int rsprod_string_trim_allNullsExceptLast(const char *string_orig,char **string_trans);
void rsprod_string_split(char *string, char sep, char ***tokens,size_t *nbTokens);

int rsprod_string_toLower(char *string);
int rsprod_string_toUpper(char *string);

int rsprod_string_fromHDF4_2_NC(char *old, char **new);

#endif
