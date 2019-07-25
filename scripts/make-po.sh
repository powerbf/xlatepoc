#!/bin/bash

NAME=$1
if [ -z "$NAME" ]
then
	echo "Usage: make-po.sh <name>"
	exit 1
fi


python3 decline-de.py the-$NAME.po
cp ./$NAME-header.po ../po/de/$NAME.po
cat out.po >> ../po/de/$NAME.po

