/**
 * @file utils.h
 * @brief Utilitaries functions to manipule char strings. 
 */

#ifndef UTILS_H_
#define UTILS_H_



/**
 * @brief Copy a String without the doble quotes
 * @param string the string to copy
 * @return a new string without the doble quotes
 */
char *suppress_quotes_new_string(char *string);


/**
 * @brief Delete the doble quotes of a string.
 * @param chaine the string to modify
 */
void suppress_quotes(char *chaine);


/**
 * @brief Convert à String to uppercase.
 * @param chaine the string to modify
 */
void to_upper_case(char *chaine);


/**
 * @brief Allocate memory and copy the content of a given string
 * @param object the string to cop
 * @return the new string
 */
char *copyString(char *object);




#endif
