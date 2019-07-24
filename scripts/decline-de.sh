#!/bin/bash
######################################################################
# Decline German substantives
# Requires .po containing nominative with definite articles as input
######################################################################

#####################################
# Extract text from line
#####################################
function extract_text {
	local line="$1"
	local text=`echo "$line" | sed -e 's/^[^"]*"//' -e 's/"[[:blank:]]*$//'`
	echo "$text"
}

#####################################
# Check line
#####################################
function check_line {
	local line=$1

	if [[ $line =~ ^[[:space:]]*# ]] || [[ $line =~ ^[[:blank:]]*$ ]]
	then
		: # skip
	elif [[ $line =~ msgid ]]
	then
		if ! [[ $line =~ ^msgid[[:space:]]+\"[^\"]*\"[[:blank:]]*$ ]]
		then
			echo "ERROR: bad line: $line"
		fi
	elif [[ $line =~ msgstr ]]
	then
		if ! [[ $line =~ ^msgstr[[:space:]]+\"[^\"]*\"[[:blank:]]*$ ]]
		then
			echo "ERROR: bad line: $line"
		fi

		local text=$(extract_text "$line")

		if [[ $text =~ ^[[:blank:]] ]]
		then
			echo "WARNING: Blank space at start of \"$text\""
			text=`echo "$text" | sed -e 's/^[[:blank:]]+//'`		
		fi

		local gender='unk' # unknown
		if [[ $text =~ ^der\  ]]
		then
			gender=masc
		elif [[ $text =~ ^die\  ]]
		then
			gender=fem
		elif [[ $text =~ ^das\  ]]
		then
			gender=neut
		fi

		# remove "of <whatever>"
		local full_text="$text"
		text=`echo "$text" | sed -e 's/ des .*$//' -e 's/ der .*$//'`		

		if [[ $text =~ lein$ ]] || [[ $text =~ chen$ ]] || [[ $text =~ ment$ ]] || [[ $text =~ tel$ ]]
		then
			# should be neuter
			if [[ $gender != neut  ]]
			then
				echo "WARNING: Should be neuter: \"$full_text\""
			fi
		elif [[ $text =~ um$ ]] || [[ $text =~ sel$ ]] || [[ $text =~ ma$ ]]
		then
			# should be neuter, apart from a few exceptions
			if [[ $text =~ [Ii]rrtum$ ]] || [[ $text =~ [Rr]eichtum$ ]] || [[ $text =~ [Kk]onsum$ ]] || [[ $text =~ [Bb]aum$ ]] || [[ $text =~ [Ss]treusel$ ]]
			then
				if [[ $gender != masc ]]
				then
					echo "WARNING: Should be masculine: \"$full_text\""
				fi
			elif [[ $text =~ [Ff]irma$ ]]
			then
				if [[ $gender != fem ]]
				then
					echo "WARNING: Should be feminine: \"$full_text\""
				fi
			else
				if [[ $gender != neut  ]]
				then
					echo "WARNING: Should be neuter: \"$full_text\""
				fi
			fi
		elif [[ $text =~ ung$ ]] || [[ $text =~ tion$ ]] || [[ $text =~ heit$ ]] || [[ $text =~ keit$ ]] || [[ $text =~ schaft$ ]] || \
			 [[ $text =~ enz$ ]] || [[ $text =~ anz$ ]] || [[ $text =~ ik$ ]] || [[ $text =~ tät$ ]] || [[ $text =~ itis$ ]] || \
			 [[ $text =~ sis$ ]] || [[ $text =~ ade$ ]] || [[ $text =~ age$ ]] || [[ $text =~ ere$ ]] || [[ $text =~ ine$ ]] || \
			 [[ $text =~ isse$ ]] || [[ $text =~ ive$ ]] || [[ $text =~ ei$ ]]
		then
			# should be feminine
			if [[ $gender != fem  ]]
			then
				echo "WARNING: Should be feminine: \"$full_text\""
			fi
		elif [[ $text =~ ie$ ]]
		then
			# should be feminine apart from a few exceptions
			if [[ $text =~ [Zz]ombie$ ]]
			then
				if [[ $gender != masc ]]
				then
					echo "WARNING: Should be masculine: \"$full_text\""
				fi
			else
				if [[ $gender != fem  ]]
				then
					echo "WARNING: Should be feminine: \"$full_text\""
				fi
			fi
		elif [[ $text =~ ant$ ]] || [[ $text =~ ast$ ]] || [[ $text =~ ich$ ]] || [[ $text =~ ig$ ]] || [[ $text =~ ling$ ]] || [[ $text =~ ismus$ ]]
		then
			# should be masculine
			if [[ $gender != masc  ]]
			then
				echo "WARNING: Should be masculine: \"$full_text\""
			fi
		elif [[ $text =~ ist$ ]]
		then
			# probably should be masculine
			if [[ $gender != masc  ]]
			then
				echo "WARNING: Probably should be masculine: \"$full_text\""
			fi
		elif [[ $text =~ or$ ]]
		then
			# should be masculine, except das Labor.
			if [[ $text =~ [Ll]abor$ ]]
			then
				if [[ $gender != neut ]]
				then
					echo "WARNING: Should be neuter: \"$full_text\""
				fi
			else
				if [[ $gender != masc  ]]
				then
					echo "WARNING: Should be masculine: \"$full_text\""
				fi
			fi
		elif [[ $text =~ us$ ]]
		then
			if [[ $text =~ [Mm]aus$ ]]
			then
				# die Maus, die Fledermaus
				if [[ $gender != fem ]]
				then
					echo "WARNING: Should be feminine: \"$full_text\""
				fi
			elif [[ $text =~ [Hh]aus$ ]]
			then
				# das Haus
				if [[ $gender != neut ]]
				then
					echo "WARNING: Should be neuter: \"$full_text\""
				fi
			else
				# probably should be masculine
				if [[ $gender != masc  ]]
				then
					echo "WARNING: Probably should be masculine: \"$full_text\""
				fi
			fi
		fi
	fi
}

function check_file {
	echo "Checking $INFILE..."
	while IFS= read -r line
	do
		check_line "$line"
	done < "$INFILE"
}

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
# Make German plural
#
# References:
# - https://www.vistawide.com/german/grammar/german_nouns02.htm
# - https://www.germanveryeasy.com/plural
#
#####################################
function make_german_plural {
	local line="$1"
	local gender="$2"
	local context="$3" # because case is a reserved word

	local singular=$(extract_text "$line")
	local plural=`echo "$singular" | sed -e 's/^der /%d /' -e 's/^die /%d /' -e 's/^das /%d /'`

	local certain=0
	local warning=''

	if [[ $singular =~ [Zz]ombie$ ]] || [[ $singular =~ [Gg]oblin$ ]] || [[ $singular =~ [Ww]yvern$ ]] || 
	   [[ $singular =~ [Ss]priggan$ ]] || [[ $singular =~ [Kk]lown$ ]] || [[ $singular =~ Lich$ ]] || 
       [[ $singular =~ [Gg]argoyle$ ]] 
	then
		# borrowed English words take -s
		plural="$plural"s
		certain=1
	elif [[ $singular =~ [Bb]aum$ ]]
	then
		plural=`echo "$plural" | sed 's/aum$/äume/'`
		certain=1
	elif [[ $singular =~ [Kk]olben$ ]] || [[ $singular =~ [Ss]chatten$ ]]
	then
		# no change
		certain=1
	elif [[ $singular =~ [Gg]eist$ ]]
	then
		# add -er
		plural="$plural"er
		certain=1
	elif [[ $singular =~ e$ ]]
	then
		# nouns ending in -e normally add -n
		plural="$plural"n
		if [[ $gender == fem ]]
		then
			# if it's feminine, then it's certain
			certain=1
		fi
	elif [[ $singular =~ [aoiuy]$ ]]
	then
		# nouns ending in other vowels normally add -s, apart from a few exceptions
		if [[ $singular =~ [Hh]ydra$ ]] || [[ $singular =~ [Pp]risma$ ]] || [[ $singular =~ [Tt]arantella$ ]]
		then
			# -a -> -en
			plural=`echo "$plural" | sed 's/a$/en/'`
		else
			# add -s
			plural="$plural"s
		fi
	elif [[ $singular =~ chen$ ]] || [[ $singular =~ lein$ ]]
	then
		# no change - guaranteed
		certain=1
	elif [[ $singular =~ el$ ]] || [[ $singular =~ er$ ]]
	then
		# feminine nouns add -n (e.g. Kugeln), masc/neut usually don't change, but there are some exceptions
		if [[ $gender == fem ]]
		then
			plural="$plural"n
		elif [[ $singular =~ [Tt]ier$ ]]
		then
			# Tier -> Tiere
			plural="$plural"e
		fi
	elif [[ $singular =~ us$ ]]
	then
		if [[ $singular =~ aus$ ]]
		then
			# Fledermäuse, etc.
			plural=`echo "$plural" | sed 's/aus$/äuse/'`
			certain=1
		elif [[ $singluar =~ Ufetubus$ ]]
		then
			plural=`echo "$plural" | sed 's/us$/i/'`
			certain=1
		else
			plural=`echo "$plural" | sed 's/us$/en/'`
		fi
	elif [[ $singular =~ um$ ]]
	then
		plural=`echo "$plural" | sed 's/um$/en/'`
	elif [[ $singlar =~ nis$ ]]
	then
		# nis -> nisse
		plural="$plural"se
		certain=1
	elif [[ $singular =~ eur\" ]] || [[ $singular =~ ich\" ]] || [[ $singular =~ ig\" ]] || [[ $singular =~ ling\" ]]
	then
		# always add -e
		plural="$plural"e
		certain=1
	elif [[ $singular =~ ör\" ]] || [[ $singular =~ är\" ]] || [[ $singular =~ ar\" ]]
	then
		# probably add -e
		plural="$plural"e
	elif [[ $singular =~ at\" ]] || [[ $singular =~ ant\" ]] || [[ $singular =~ ent\" ]] | [[ $singular =~ ist\" ]]
	then
		# always add -en
		plural="$plural"en
		certain=1
	elif [[ $gender == fem ]]
	then
		if [[ $singular =~ in$ ]]
		then
			# Königinnen, etc
			plural="$plural"nen
			certain=1
		elif [[ $singular =~ itis$ ]]
		then
			# itis -> iden
			plural=`echo "$plural" | sed 's/itis$/iden/'`
			certain=1
		elif [[ $singular =~ xis$ ]]
		then
			# xis -> xien
			plural=`echo "$plural" | sed 's/xis$/xien/'`
			certain=1
		elif [[ $singular =~ sis$ ]]
		then
			# sis -> sen
			plural=`echo "$plural" | sed 's/sis$/sen/'`
			certain=1
		elif [[ $text =~ ung$ ]] || [[ $text =~ tion$ ]] || [[ $text =~ heit$ ]] || [[ $text =~ keit$ ]] || [[ $text =~ schaft$ ]] || \
			 [[ $text =~ tät$ ]] || [[ $text =~ ei$ ]]
		then
			# definitely add -en
			plural="$plural"en
			certain=1
		else
			# probably add -en
			plural="$plural"en
		fi
	else
		# other words normally add -e
		plural="$plural"e
	fi

	if [[ $context == dat ]]
	then
		# in dative, must add extra -n if plural does not already end in -n or -s
		if ! [[ $plural =~ n$ ]] && ! [[ $plural =~ s$ ]]
		then
			plural="$plural"n
		fi

		# adjective ending is -en
		plural=`echo "$plural" | sed 's/e /en /g'`
	fi

	echo "msgstr[1] \"$plural\""
	if [[ $certain == 0 ]]
	then
		echo "# check plural above"
	fi
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
			# remove trailing spaces
			line=`echo "$line" | sed 's/"[[:blank:]]+$/"/'`

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
				local gender='unk' # unknown
				if [[ $line =~ \"der\  ]]
				then
					gender=masc
				elif [[ $line =~ \"die\  ]]
				then
					gender=fem
				elif [[ $line =~ \"das\  ]]
				then
					gender=neut
				fi

				local sing_de=`echo "$line" | sed 's/^msgstr/msgstr[0]/'`

				if [[ $gender == masc ]]
				then
					if [[ $CONTEXT == nom ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"der/"ein/'`
					elif [[ $CONTEXT == akk ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"der/"einen/'`
					else
						sing_de=`echo "$sing_de" | sed 's/"der/"einem/'`
					fi
				elif [[ $gender == fem ]]
				then
					if [[ $CONTEXT == dat ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"die/"einer/'`
					fi
				elif [[ $gender == neut ]]
				then
					if [[ $CONTEXT == dat ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"das/"einem/'`
					else
						sing_de=`echo "$sing_de" | sed 's/"das/"ein/'`
					fi
				fi						
				
				if [[ $gender == masc ]]
				then
					if [[ $CONTEXT == nom ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"der /"ein /'`
					elif [[ $CONTEXT == akk ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"der /"einen /'`
					else
						sing_de=`echo "$sing_de" | sed 's/"der /"einem /'`
					fi
				elif [[ $gender == fem ]]
				then
					if [[ $CONTEXT == dat ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"die /"einer /'`
					else
						sing_de=`echo "$sing_de" | sed 's/"die /"eine /'`
					fi
				elif [[ $gender == neut ]]
				then
					if [[ $CONTEXT == dat ]]
					then
						sing_de=`echo "$sing_de" | sed 's/"das /"einem /'`
					else
						sing_de=`echo "$sing_de" | sed 's/"das /"ein /'`
					fi
				fi

				# if there's a capitalized word apart from the final word then the result would be doubtful
				if [[ $line =~ [[:space:]][[:upper:]].*[[:space:]] ]]
				then
					local plural_de=`echo "$line" | sed -e 's/^msgstr[^"]*"/msgstr[1] "/' -e 's/"der /"%d /' -e 's/"die /"%d /' -e 's/"das /"%d /'`
					echo "$sing_de" >> $TMPFILE
					echo "$plural_de" >> $TMPFILE
					echo "# check both singular and plural above (multiple nouns)" >> $TMPFILE
				else
					# adjust adjectives
					if [[ $CONTEXT == dat ]]
					then
						# adjective ending is -en for all genders
						sing_de=`echo "$sing_de" | sed 's/e /en /g'`
					elif [[ $gender == neut ]]
					then
						# adjective ending is -es
						sing_de=`echo "$sing_de" | sed 's/e /es /g'`
					elif [[ $gender == masc ]]
					then
						if [[ $CONTEXT == nom ]]
						then
							# adjective ending is -er
							sing_de=`echo "$sing_de" | sed 's/e /er /g'`
						elif [[ $CONTEXT == akk ]]
						then
							# adjective ending is -en
							sing_de=`echo "$sing_de" | sed 's/e /en /g'`
						fi
					fi

					# generate German plural
					local plural_de=$(make_german_plural "$line" "$gender" "$CONTEXT")

					echo "$sing_de" >> $TMPFILE
					echo "$plural_de" >> $TMPFILE
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

check_file

echo "Writing output to $OUTFILE..."
echo > $OUTFILE

process "nominative" "definite"
process "accusative" "definite"
process "dative"     "definite"
process "nominative" "indefinite"
process "accusative" "indefinite"
process "dative"     "indefinite"

