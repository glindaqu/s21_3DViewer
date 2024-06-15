#!/bin/bash

usage() {
    echo "Использование: $0 [-a <файл>] [-r <файл>] [-c <сообщение>] [-p <origin> <ветка>] [-i <сообщение>] [-h]"
    echo "  -a <файл>     Добавить файл в индекс Git"
    echo "  -r <файл>     Удалить файл из индекса Git"
    echo "  -c <сообщение>    Создать коммит с сообщением"
    echo "  -i <сообщение>    Добавить к сообщению issue, yandex_tracker"
    echo "  -p <origin> <ветка>    Отправить изменения в удаленный репозиторий"
    echo "  -h, --help     Показать эту справку и выйти"
    exit 1
}

if [ "$#" -eq 0 ]; then
    git status
fi

add_file=""
rm_file=""
push_flag=false
commit_message=""
origin=""
issue_flag=false
branch_name=""
yandex_tracker=""
issue=""

while getopts ":a:r:c:p:i:h-" opt; do
    case $opt in
        a) add_file="$OPTARG" ;;
        r) rm_file="$OPTARG" ;;
        c) commit_message="$OPTARG" ;;
        p)
            push_flag=true
            if [ -z "$origin" ]; then
                origin="$OPTARG"
            else
                branch_name="$OPTARG"
            fi
            ;;
        h) usage ;;
        i)  
            issue_flag=true 
            commit_message="${commit_message}${OPTARG} " ;;
        -)
            case "$OPTARG" in
                help) usage ;;
                *) echo "Неизвестный аргумент: --$OPTARG" >&2
                   usage ;;
            esac
            ;;
        \?)
            echo "Неизвестный флаг: -$OPTARG" >&2
            usage
            ;;
        :)
            echo "Опция -$OPTARG требует аргумента." >&2
            usage
            ;;
    esac
done

if [ "$push_flag" = true ] && [ -z "$origin" ]; then
    echo "Ошибка: после опции -p необходимо указать имя удаленного репозитория."
    usage
fi

if [ "$push_flag" = true ] && [ -z "$branch_name" ]; then
    read -p "Branch name: " branch_name
fi

if [ ! -z "$add_file" ]; then
    git add "$add_file"
fi

if [ ! -z "$rm_file" ]; then
    git rm "$rm_file"
fi

if [ ! -z "$commit_message" ]; then
    if [ "$issue_flag" = true ]; then
        read -p "Issue: " issue
        read -p "Yandex tracker: " yandex_tracker
        if [ "$yandex_tracker" -eq 0 ]; then
            commit_message="#${issue} ${commit_message}"
        else
            commit_message="#${issue} ${commit_message}VIEWER-${yandex_tracker}"
        fi
    fi
    git commit -m "$commit_message"
fi

if [ "$push_flag" = true ]; then
    if [ ! -z "$origin" ] && [ ! -z "$branch_name" ]; then
        echo "Pushing to $origin $branch_name"
        git push $origin $branch_name
    fi
fi