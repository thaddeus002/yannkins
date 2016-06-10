#!/bin/bash
# this script is a task to be executed by yannkins
# you need put in parameters the name of the task and the command
# One line will be added in the log file ${LOGDIR}/${TASK} :
#     ${date};FAIL (in case of failure)
#     ${date};OK   (in case of success)
# The file ${LOGDIR}/${TASK}_console will content the last console output


IC="Yannkins"
DIR="${YANNKINS_HOME}"
LOGDIR="${DIR}/log"

function usage() {
    printf "Execute a %s task\n" ${IC}
    printf "Usage : %s <TASK_NAME> <COMMAND>\n" $0
    exit 1
}


if [ $# -ne 2 ]; then
    usage
fi

TACHE=$1
COMMANDE="$2"
DATE=$(date "+%d/%m/%Y %H:%M")
RESULTAT=0

if [ ! -d ${LOGDIR} ]; then
    mkdir -p ${LOGDIR}
    if [ $? -ne 0 ]; then
        printf "Could not create the directory %s\n" ${LOGDIR}
        printf "%s : task %s not completed\n" "${DATE}" ${TACHE}
        exit 1
    fi
fi

FICLOG="${LOGDIR}/${TACHE}"
touch ${FICLOG}
if [ $? -ne 0 ]; then
    printf "Could not create or modify the file %s\n" ${FICLOG}
    printf "%s : task %s not completed\n" "${DATE}" ${TACHE}
    exit 1
fi
[ $(wc -c ${FICLOG} | cut -f 1 -d \ ) -eq 0 ] && echo "date;result" > ${FICLOG}


FICCONSOLE="${LOGDIR}/${TACHE}_console"
touch ${FICCONSOLE}
if [ $? -ne 0 ]; then
    printf "Could not create or modify the file %s\n" ${FICCONSOLE}
    printf "%s : task %s not completed\n" "${DATE}" ${TACHE}
    exit 1
fi

${COMMANDE} &> ${FICCONSOLE}
RESULTAT=$?

if [ ${RESULTAT} -eq 0 ]; then
    RESULTAT="OK"
else
    RESULTAT="FAIL"
fi

printf "%s;%s\n" "${DATE}" ${RESULTAT} >> ${FICLOG}

