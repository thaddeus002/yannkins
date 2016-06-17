/**
 * @file project.h
 * @brief define a project
 */

#ifndef YK_PROJECT_H
#define YK_PROJECT_H 1


/**
 * @brief The type of versioning system
 */
typedef enum {
    NONE, /**< @brief type is undefined */
    GIT, /**< @brief Git repository */
    SVN /**< @brief Subversion repository */
} repository_t;


/**
 * @brief Data of a project
 */
typedef struct {
	char *project_name; /**< @brief name of this project */
    repository_t versioning_type; /**< @brief which versionning system is used */
	char *repository; /**< @brief repository's url */
	char *repos_user; /**< @brief login for svn authentification */
	char *repos_password; /**< @brief password for svn authentification */
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
