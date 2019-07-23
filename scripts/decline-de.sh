#!/bin/bash
######################################################################
# Decline German substantives
# Requires .po containing nominative with definite articles as input
######################################################################

#####################################
# Make English plural
#####################################
function make_english_plural {
	local singular="$1"
	plural=`echo "$singular" | sed -e 's/^msgid */msgid_plural /' -e 's/"the /"%d /' -e 's/"a /"%d /' -e 's/"an /"%d /'`

    if [[ $singular =~ gold\" ]] || [[ $singular =~ fish\" ]] || [[ $singular =~ folk\" ]] || [[ $singular =~ spwan\" ]]
	then
		: # plural same as singular
    elif [[ $singular =~ tengu\" ]] || [[ $singular =~ sheep\" ]] || [[ $singular =~ swine\" ]] || [[ $singular =~ efreet\" ]]
	then
		: # plural same as singular
    elif [[ $singular =~ jiangshi\" ]] || [[ $singular =~ raiju\" ]] || [[ $singular =~ meliai\" ]]
	then
		: # plural same as singular
	elif [[ $singular =~ ' of ' ]]
	then
		# orbs of eyes, etc.
		plural=`echo "$plural" | sed 's/ of /s of /'`
	elif [[ $singular =~ catoblepas\" ]]
	then
		# catoblepas -> catoblepae
		plural=`echo "$plural" | sed 's/s"$/e"/'`
	elif [[ $singular =~ cyclops\" ]]
	then
		# cyclops -> cyclopes
		plural=`echo "$plural" | sed 's/s"$/es"/'`
	elif [[ $singular =~ mage\" ]]
	then
		# mage -> magi
		plural=`echo "$plural" | sed 's/e"$/i"/'`
	elif [[ $singular =~ simulacrum\" ]]
	then
		# simulacrum -> simulacra
		plural=`echo "$plural" | sed 's/um"$/a"/'`
	elif [[ $singular =~ eidolon\" ]]
	then
		# eidolon -> eidola
		plural=`echo "$plural" | sed 's/on"$/a"/'`
	elif [[ $singular =~ djinni\" ]]
	then
		# djinni -> djinn
		plural=`echo "$plural" | sed 's/i"$/"/'`
	elif [[ $singular =~ foot\" ]]
	then
		# foot -> feet
		plural=`echo "$plural" | sed 's/foot"$/feet"/'`
	elif [[ $singular =~ ophan\" ]] || [[ $singular =~ cherub\" ]] || [[ $singular =~ seraph\" ]]
	then
		# add -im
		plural=`echo "$plural" | sed 's/"$/im"/'`
	elif [[ $singular =~ arachi\" ]]
	then
		# barachi -> barachim
		plural=`echo "$plural" | sed 's/i"$/im"/'`
	elif [[ $singular =~ ushabti\" ]]
	then
		# ushabti -> ushabtiu
		plural=`echo "$plural" | sed 's/"$/u"/'`
	elif [[ $singular =~ zitzimitl\" ]]
	then
		# Tzitzimitl -> Tzitzimimeh
		plural=`echo "$plural" | sed 's/tl"$/meh"/'`
	elif [[ $singular =~ mosquito\" ]]
	then
		# mosquito -> mosquitoes (but not gecko -> geckoes)
		plural=`echo "$plural" | sed 's/o"$/oes"/'`
	elif [[ $singular =~ larva\" ]] || [[ $singular =~ antenna\" ]] || [[ $singular =~ hypha\" ]]
	then
		# larva -> larvae, etc.
		plural=`echo "$plural" | sed 's/a"$/ae"/'`
	elif [[ $singular =~ staff\" ]]
	then
		# staff -> staves
		plural=`echo "$plural" | sed 's/ff"$/ves"/'`
	elif [[ $singular =~ [^f]f\" ]]
	then
		# elf -> elves, but not hippogriff -> hippogrives
		plural=`echo "$plural" | sed 's/f"$/ves"/'`
	elif [[ $singular =~ fe\" ]]
	then
		# knife -> knives
		plural=`echo "$plural" | sed 's/fe"$/ves"/'`
	elif [[ $singular =~ ex\" ]]
	then
		# vortex -> vortices
		plural=`echo "$plural" | sed 's/ex"$/ices"/'`
	elif [[ $singular =~ us\" ]] && ! [[ $singular =~ lotus\" ]] && ! [[ $singluar =~ status\" ]]
	then
		# fungus -> fungi, ufetubus -> ufetubi
		plural=`echo "$plural" | sed 's/us"$/i"/'`
	elif [[ $singular =~ [^aeiou]y\" ]]
	then
		# -y -> -ies
		plural=`echo "$plural" | sed 's/y"$/ies"/'`
	elif [[ $singular =~ (s|sh|ch|x|z)\" ]]
	then
		# add -es
		plural=`echo "$plural" | sed 's/"$/es"/'`
	else
		# add -s
		plural=`echo "$plural" | sed 's/"$/s"/'`
	fi

	echo "$plural"
}

#####################################
# Process for given article and case
#####################################
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
			# remove trailing spaces
			line=`echo "$line" | sed 's/"[[:space:]]+$/"/'`

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
				local sing_en=`echo "$line" | sed -r 's/"the ([aAeEiIoOuU])/"an \1/' | sed -e 's/"the /"a /' -e 's/^msgid */msgid /'`

				# generate English plural
				local plural_en=$(make_english_plural "$sing_en")

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
				local CHECK=0
				if [[ "$CONTEXT" == "dat" ]] || [[ $line =~ \"den ]]
				then
					# if there's a capitalized word apart from the final word then the result would be doubtful
					if [[ $line =~ [[:space:]][[:upper:]].*[[:space:]] ]]
					then
						CHECK=1
					else
						# adjective ending is -en
						line=`echo "$line" | sed -r 's/e /en /g'`
					fi
				fi
				echo "$line" >> $TMPFILE
				if [ $CHECK -eq 1 ]
				then
					echo "# check line above" >> $TMPFILE
				fi
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
					echo "$sing_de" >> $TMPFILE
					echo "$plural_de" >> $TMPFILE
					echo "# check both singular and plural above" >> $TMPFILE
				else
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

					echo "$sing_de" >> $TMPFILE
					echo "$plural_de" >> $TMPFILE
					echo "# check plural above" >> $TMPFILE
				fi

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

