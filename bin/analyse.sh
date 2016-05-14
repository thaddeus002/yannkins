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

        # Versionning System : NONE, GIT, or SVN
        VS="NONE"
        if [ ${SVN_DEPOT}_ != _ ]; then
            VS="SVN"
        else
            if [ ${GIT_DEPOT}_ != _ ]; then
                VS="GIT"
            fi
        fi

        # Checkout/clone of the project

        if [ ${VS} == "NONE" ]; then
            printf "Repository not specified\n"
            exit 1
        fi

        if [ ${VS} == "SVN" ]; then
            MSG="Checkout of repository\n"
            COMMANDE="svn co ${SVN_DEPOT}/trunk ${PROJECT_NAME}"
        fi

        if [ ${VS} == "GIT" ]; then
            MSG="Clone of repository\n"
            COMMANDE="git clone ${GIT_DEPOT} ${PROJECT_NAME}"
        fi

        printf "${MSG}"
        cd ${SVN_HOME}
        rm -rf ${PROJECT_NAME}
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

        if [ ${VS} == "SVN" ]; then
            svn log -l 10 --xml ${SVN_DEPOT}/trunk > svnlogl10.xml
            convert_log -i svnlogl10.xml -o ${YANNKINS_HOME}/log/SVNLOG_${PROJECT_NAME}
        fi

        if [ ${VS} == "GIT" ]; then
            cd ${SVN_HOME}/${PROJECT_NAME}
            printf "#;author;date;commentaries\n" > ${YANNKINS_HOME}/log/SVNLOG_${PROJECT_NAME}
            git log -n 10 --pretty=format:"%h;%an;%ci;%s" >> ${YANNKINS_HOME}/log/SVNLOG_${PROJECT_NAME}
            cd -
        fi
    fi
done

# make report
printf "Creating projects' pages\n"
cree_page
