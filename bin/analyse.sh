#!/bin/bash
# Process the periodic analysis of declared projects


#YANNKINS_HOME="/var/yannkins"
#YANNKINS_HOME="."
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
        #if [ ${COMPIL}_ != _ ]; then
        if [ ${COMP} -gt 0 ]; then 
            cd ${SVN_HOME}/${PROJECT_NAME}
            tache.sh COMPILATION_${PROJECT_NAME} "${COMPIL}"
            cd -
        fi

        # tests
        printf "Tests\n"
        TESTS=$(echo -n ${TESTS_UNI} | wc -c)
        #if [ ${TESTS_UNI}_ != _ ]; then
        if [ ${TESTS} -gt 0 ]; then 
            cd ${SVN_HOME}/${PROJECT_NAME}
            tache.sh TESTS_${PROJECT_NAME} "${TESTS_UNI}"
            cd -
        fi

        # Logs SVN
        printf "Checking SVN logs\n"
        #printf "#;auteur;date;nb lignes;commentaires\n" > svnlogl10.csv
        printf "#;author;date;number of lines;commentaries\n" > svnlogl10.csv
        svn log -l 10 ${SVN_DEPOT}/trunk | tr "|" ";" | tr "\n" "$" | sed -e 's/\$\$/; /g' | sed -e 's/\$--/\n--/g' | sed -re 's/-+\$//g' | sed -e 's/\$/\<br\/\>/g' >> svnlogl10.csv

        printf "Creating project's page\n"
        mv svnlogl10.csv ${YANNKINS_HOME}/log/SVNLOG_${PROJECT_NAME}
        cree_page
        #dillo table.html
    fi
done
