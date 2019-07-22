#!/bin/bash
######################################################################
# Decline German substantives
# Requires .po containing nominative with definite articles as input
######################################################################

function process {
	local CASE=$1
	local ARTICLE=$2
	local CONTEXT=${CASE:0:3}
	CONTEXT=${CONTEXT/acc/akk}
	local TMPFILE=$CONTEXT-$ARTICLE.tmp

	echo "Processing $ARTICLE $CASE..."

	echo "################################" > $TMPFILE
	echo "# $ARTICLE - $CASE" >> $TMPFILE
	echo "################################" >> $TMPFILE

	while IFS= read -r line
	do
		if [[ $line =~ msgctxt ]]
		then
			continue
		elif [[ $line =~ ^msgid ]]
		then
			# msgctxt line
			if [ $CONTEXT == nom ]
			then
				# nominative is default
				echo "#msgctxt \"$CONTEXT\"" >> $TMPFILE
			else
				echo "msgctxt \"$CONTEXT\"" >> $TMPFILE
			fi

			# msgid line (English)
			if [ $ARTICLE == definite ]
			then
				echo "$line" >> $TMPFILE
			else
				# replace "the" with "a" or "an"
				local sing_en=`echo "$line" | sed -r 's/"the ([aAeEiIoOuU])/"an \1/' | sed 's/"the /"a /' | sed 's/^msgid */msgid /'`

				# generate English plural
				local plural_en=`echo "$line" | sed 's/^msgid */msgid_plural /' | sed 's/"the /"%d /' | sed 's/"[[:space:]]*$/s"/'`


				echo "$sing_en" >> $TMPFILE
				echo "$plural_en" >> $TMPFILE
			fi
		elif [[ $line =~ ^msgstr ]]
		then
			# msgstr line (German)
			if [ $ARTICLE == definite ]
			then
				if [ $CONTEXT == akk ]
				then
					line=`echo "$line" | sed 's/"der /"den /'`
				elif [ $CONTEXT == dat ]
				then
					line=`echo "$line" | sed 's/"der /"dem /' | sed 's/"das /"dem /' | sed 's/"die /"der /'`
				fi

				# adjust adjective ending
				if [[ "$CONTEXT" == "dat" ]] || [[ $line =~ \"den ]]
				then
					# if there's a capitalized word apart from the final word then the result would be doubtful
					if [[ $line =~ [[:space:]][[:upper:]].*[[:space:]] ]]
					then
						echo "# check this" >> $TMPFILE
					else
						# adjective ending is -en
						line=`echo "$line" | sed -r 's/e /en /g'`
					fi
				fi
				echo "$line" >> $TMPFILE
			else
				local sing_de=`echo "$line" | sed 's/^msgstr/msgstr[0]/'`
				local plural_de=`echo "$line" | sed 's/^msgstr/msgstr[1]/'`

				if [ $CONTEXT == nom ]
				then
					sing_de=`echo "$sing_de" | sed 's/"der /"ein /' | sed 's/"das /"ein /' | sed 's/"die /"eine /'`
				elif [ $CONTEXT == akk ]
				then
					sing_de=`echo "$sing_de" | sed 's/"der /"einen /' | sed 's/"das /"ein /' | sed 's/"die /"eine /'`
				elif [ $CONTEXT == dat ]
				then
					sing_de=`echo "$sing_de" | sed 's/"der /"einem /' | sed 's/"das /"einem /' | sed 's/"die /"einer /'`
				fi

				plural_de=`echo "$plural_de" | sed 's/"der /"%d /' | sed 's/"das /"%d /' | sed 's/"die /"%d /'`

				# adjust adjectives

				# if there's a capitalized word apart from the final word then the result would be doubtful
				if [[ $line =~ [[:space:]][[:upper:]].*[[:space:]] ]]
				then
					echo "# check both singular and plural" >> $TMPFILE
				else
					echo "# check plural" >> $TMPFILE

					# try to guess German plural noun form
					plural_de=`echo "$plural_de" | sed 's/"[[:space:]]+$/"/'`

					plural_de=`echo "$plural_de" | sed 's/e"$/en"/'`
					plural_de=`echo "$plural_de" | sed 's/in"$/innen"/'`
					plural_de=`echo "$plural_de" | sed 's/ist"$/isten"/'`
					plural_de=`echo "$plural_de" | sed -r 's/([^nr ])"$/\1e"/'`

					plural_de=`echo "$plural_de" | sed 's/mause"$/mäuse"/'`

					if [ $CONTEXT == dat ]
					then
						plural_de=`echo "$plural_de" | sed 's/r"$/rn"/'`
						plural_de=`echo "$plural_de" | sed 's/e"$/en"/'`
						plural_de=`echo "$plural_de" | sed -r 's/([^n ])"$/\1en"/'`
					fi	

					if [ $CONTEXT == dat ]
					then
						# adjective ending is -en for both singular and plural
						sing_de=`echo "$sing_de" | sed 's/e /en /g'`
						plural_de=`echo "$plural_de" | sed 's/e /en /g'`
					elif [ $CONTEXT == akk ] && [[ "$sing_de" =~ "\"einen " ]]
					then
						# adjective ending is -en for singular only
						sing_de=`echo "$sing_de" | sed 's/e /en /g'`
					elif [ $CONTEXT == nom ] && [[ "$line" =~ "\der " ]]
					then
						# adjective ending is -er for singular
						sing_de=`echo "$sing_de" | sed 's/e /er /g'`
					elif [[ "$line" =~ "\"das " ]]
					then
						# adjective ending is -es for singular
						sing_de=`echo "$sing_de" | sed 's/e /es /g'`
					fi
				fi

				echo "$sing_de" >> $TMPFILE
				echo "$plural_de" >> $TMPFILE
			fi
		else
			# some other type of line (comment, blank line)
			echo "$line" >> $TMPFILE
		fi
	done < "$INFILE"

	cat $TMPFILE >> $OUTFILE
}

INFILE=$1

if [ -z "$INFILE" ]
then
	echo "Usage: decline-de.sh <input file>"
	echo "    where <input file> is a .po file containing German translations with nominative singular definite articles (der/die/das)"
	exit 1
fi

OUTFILE=de-out.po

echo "Writing output to $OUTFILE..."
echo > $OUTFILE

process "nominative" "definite"
process "accusative" "definite"
process "dative"     "definite"
process "nominative" "indefinite"
process "accusative" "indefinite"
process "dative"     "indefinite"

