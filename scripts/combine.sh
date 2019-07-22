#!/bin/bash

while IFS=$'\t' read -r f1 f2
do
  if [ ! -z "$3" ]
  then
    printf 'msgctxt "%s"\n' "$3"
  fi
  printf 'msgid "%s"\n' "$f1"
  printf 'msgstr "%s"\n\n' "$f2"
done < <(paste $1 $2)
