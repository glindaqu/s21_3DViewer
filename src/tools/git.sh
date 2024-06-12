#!/bin/bash

usage() {
    echo "Использование: $0 [-a <файл>] [-r <файл>] [-c <сообщение>] [-p <origin> <ветка>] [-i <сообщение>] [-y <сообщение>] [-h]"
    echo "  -a <файл>     Добавить файл в индекс Git"
    echo "  -r <файл>     Удалить файл из индекса Git"
    echo "  -c <сообщение>    Создать коммит с сообщением"
    echo "  -i <сообщение>    Создать коммит с issue"
    echo "  -y <сообщение>    Создать коммит с yandex tracker"
    echo "  -p <origin> Отправить изменения в удаленный репозиторий"
    echo "  -h, --help     Показать эту справку и выйти"
    exit 1
}

if [ "$#" -eq 0 ]; then
    git status
fi

issue_message=""
yandex_message=""
add_file=""
rm_file=""
push_flag=false
issue_flag=false
yandex_flag=false
commit_message=""
origin=""
branch_name=""

while getopts ":a:r:c:p:i:y:h:" opt; do
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
        i) commit_message="$OPTARG" 
           issue_flag=true ;;
        y) commit_message="$OPTARG" 
           yandex_flag=true ;;
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

if [ $push_flag = true ]; then
    read -p "Branch name: " branch_name
    if [ ! -z "$origin" ] && [ ! -z "$branch_name" ]; then
        echo "Pushing to $origin $branch_name"
        git push $origin $branch_name
    fi
fi

if [ ! -z "$add_file" ]; then
    git add "$add_file"
fi

if [ ! -z "$rm_file" ]; then
    git rm "$rm_file"
fi

if [ ! -z "$commit_message" ]; then
    if [ "$issue_flag" = true ] && [ "$yandex_flag" = true ]; then
        read -p "Issue num: " issue_message
        read -p "Yandex num: " yandex_message
        commit_message="#[$issue_message][$yandex_message] $commit_message"
    elif [ "$issue_flag" = true ]; then
        read -p "Issue num: " issue_message
        commit_message="#${issue_message} $commit_message"
    elif [ "$yandex_flag" = true ]; then
        read -p "Yandex num: " yandex_message
        commit_message="$VIEWER-${yandex_message} $commit_message"
    fi
    git commit -m "$commit_message"
fi