#!/bin/bash

NAME=$1
if [ -z "$NAME" ]
then
	echo "Usage: make-po.sh <name>"
	exit 1
fi


./decline-de.sh the-$NAME.po
cp ./$NAME-header.po ../po/de/$NAME.po
cat de-out.po >> ../po/de/$NAME.po

