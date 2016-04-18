/** @file project.h données de définition d'un projet */

#ifndef YK_PROJECT_H
#define YK_PROJECT_H 1

/**
 * Data of a project
 */
typedef struct {
	char *project_name; /**< name of this project */
	char *svn_depot; /**< svn repository */
	char *svn_user;
	char *svn_password; 
	char *compil_cmd; /**< compilation command*/
	char *tests_cmd; /**< unit tests command */
} yk_project;

/**
 * Read the description of a project in a file.
 *
 * @param filename the of the file to read
 * @return a pointer on a project structure
 */
yk_project *yk_read_project_file(char *filename);


/**
 * Free the memory.
 * @param a struct to free
 */
void yk_destroy_project(yk_project *project);

#endif
