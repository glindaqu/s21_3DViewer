#!/bin/bash

read -p "Введите название ветки: " branch_name

read -p "Введите origin: " origin

current_branch=$(git rev-parse --abbrev-ref HEAD)
if [ "$current_branch" != "$branch_name" ]; then
    echo "Текущая ветка не соответствует введенному имени: $branch_name"
    exit 1
fi

git push $origin $branch_name