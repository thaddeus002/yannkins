/**
 * @file project.h
 * @brief define a project
 */

#ifndef YK_PROJECT_H
#define YK_PROJECT_H 1

/**
 * @brief Data of a project
 */
typedef struct {
	char *project_name; /**< @brief name of this project */
	char *svn_depot; /**< @brief svn repository */
	char *svn_user; /**< @brief login for svn authentification */
	char *svn_password; /**< @brief password for svn authentification */
	char *compil_cmd; /**< @brief compilation command*/
	char *tests_cmd; /**< @brief unit tests command */
} yk_project;

/**
 * @brief Read the description of a project in a file.
 * @param filename the of the file to read
 * @return a pointer on a project structure
 */
yk_project *yk_read_project_file(char *filename);


/**
 * @brief Free the memory.
 * @param a struct to free
 */
void yk_destroy_project(yk_project *project);

#endif
