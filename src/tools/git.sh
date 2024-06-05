#!/bin/bash

read -p "Введите название ветки: " branch_name

read -p "Введите origin: " origin

git push $origin $branch_name