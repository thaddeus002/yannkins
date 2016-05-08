#!/bin/bash
# Process the periodic analysis of declared projects

# Default Yannkins' working directory
DEFAULT_DIR="/var/yannkins"

if [ ${YANNKINS_HOME}_ == "_" ]; then
	printf "Warning : Environment variable YANNKINS_HOME not found. Using %s.\n" ${DEFAULT_DIR}
	export YANNKINS_HOME=${DEFAULT_DIR}
fi 

PROJECTS_HOME="${YANNKINS_HOME}/projets"
SVN_HOME="${YANNKINS_HOME}/svn"


#parcourt des projets
for p in ${PROJECTS_HOME}/*; do
    if [ $p != "${PROJECTS_HOME}/*" ]; then

        . $p

        if [ ${PROJECT_NAME}_ == _ ]; then
            printf "File %s : Project name missing\n" "$p"
            exit 1
        fi

        printf "Analysis of project %s\n" "${PROJECT_NAME}"

        # Checkout of the project
        printf "Checkout of repository\n"
        if [ ${SVN_DEPOT}_ == _ ]; then
            printf "Repository not specified\n"
            exit 1
        fi
        cd ${SVN_HOME}
        rm -rf ${PROJECT_NAME}
        COMMANDE="svn co ${SVN_DEPOT}/trunk ${PROJECT_NAME}"
        printf "%s\n" "${COMMANDE}"
        tache.sh "SVN_CHECKOUT_${PROJECT_NAME}" "${COMMANDE}"
        cd ${YANNKINS_HOME}

        # Compiling project
        printf "Compilation\n"
        COMP=$(echo -n ${COMPIL} | wc -c)
        if [ ${COMP} -gt 0 ]; then 
            cd ${SVN_HOME}/${PROJECT_NAME}
            tache.sh COMPILATION_${PROJECT_NAME} "${COMPIL}"
            cd -
        fi

        # tests
        printf "Tests\n"
        TESTS=$(echo -n ${TESTS_UNI} | wc -c)
        if [ ${TESTS} -gt 0 ]; then 
            cd ${SVN_HOME}/${PROJECT_NAME}
            tache.sh TESTS_${PROJECT_NAME} "${TESTS_UNI}"
            cd -
        fi

        # Logs SVN
        printf "Checking SVN logs\n"
        printf "#;author;date;number of lines;commentaries\n" > svnlogl10.csv
        svn log -l 10 ${SVN_DEPOT}/trunk | tr "|" ";" | tr "\n" "$" | sed -e 's/\$\$/; /g' | sed -e 's/\$--/\n--/g' | sed -re 's/-+\$//g' | sed -e 's/\$/\<br\/\>/g' >> svnlogl10.csv

        printf "Creating project's page\n"
        mv svnlogl10.csv ${YANNKINS_HOME}/log/SVNLOG_${PROJECT_NAME}
        cree_page
    fi
done
