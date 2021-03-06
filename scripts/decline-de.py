######################################################################
# Decline German substantives
# Requires .po containing nominative with definite articles as input
######################################################################
import sys
import re

#####################################
# Extract text from line
#####################################
def extract_text(line):
    text = re.sub('^[^"]*"', '', line)
    text = re.sub('"[ \t]*$', '', text)
    return text

#####################################
# Split out " of <whatever>"
# It takes no part in declensions
#####################################
def split_of(text):
    words = ['of', 'von', 'des', 'der']
    
    for word in words:
        patt = " " + word + " "
        if re.search("[^ ]" + patt, text):
            suffix = re.sub(".*" + patt, patt, text)
            text = re.sub(patt + ".*$", "", text)
            return text, suffix
    
    return text, ""

#####################################
# Determine gender from German nominative
# (Input must have definite article)
#####################################
def get_gender(text):
    if text.startswith("der "):
        return "masc"
    elif text.startswith("die "):
        return "fem"
    elif text.startswith("das "):
        return "neut"
    else:
        return "unk" # unknown

#####################################
# Check German text
#####################################
def check_german(text):
    if re.search("^[ \t]", text):
        print('WARNING: Blank space at start of: "' + text + '"')
        text = re.sub("^[ \t]+", "", text)
    
    gender = get_gender(text)
    
    # remove "of <whatever>"
    full_text = text
    text = re.sub(" (des|der) .*$", "", text)
    
    if re.search("(lein|chen|ment|tel)$", text):
        # should be neuter
        if gender != "neut":
            print("WARNING: Should be neuter: \"" + full_text + '"')
    elif re.search("(um|sel|ma)$", text):
        # should be neuter, apart from a few exceptions
        if re.search("(irrtum|konsum|reichtum|baum|streusel)$", text, re.IGNORECASE):
            if gender != "masc":
                print('WARNING: Should be masculine: "' + full_text + '"')
        elif re.search("firma$", text, re.IGNORECASE):
            if gender != "fem":
                print('WARNING: Should be feminine: "' + full_text + '"')
        else:
            if gender != "neut": 
                print('WARNING: Should be neuter: "' + full_text + '"')
    elif re.search("(ung|tion|heit|keit|scaft|enz|anz|ik|tät|itis|sis|ade|age|ere|ine|isse|ive|ei)$", text):
        # should be feminine
        if gender != "fem":
            print('WARNING: Should be feminine: "' + full_text + '"')
    elif text.endswith("ie"):
        # should be feminine apart from a few exceptions
        if re.search("zombie$", text, re.IGNORECASE):
            if gender != "masc":
                print('WARNING: Should be masculine: "' + full_text + '"')
        else:
            if gender != "fem": 
                print('WARNING: Should be feminine: "' + full_text + '"')
    elif re.search("(ant|ast|ich|ig|ling|ismus)$", text):
        # should be masculine
        if gender != "masc":
            print('WARNING: Should be masculine: "' + full_text + '"')
    elif text.endswith("ist"):
        # probably should be masculine
        if gender != "masc":
            print('WARNING: Probably should be masculine: "' + full_text + '"')
    elif text.endswith("or"):
        # should be masculine, except das Labor.
        if text.lower().endswith("labor"):
            if gender != "neut":
                print('WARNING: Should be neuter: "' + full_text + '"')
        else:
            if gender != "masc":
                print('WARNING: Should be masculine: "' + full_text + '"')
    elif text.endswith("us"):
        if text.lower().endswith("maus"):
            # die Maus, die Fledermaus
            if gender != "fem":
                print('WARNING: Should be feminine: "' + full_text + '"')
        elif text.lower().endswith("haus"):
            # das Haus
            if gender != "neut":
                print('WARNING: Should be neuter: "' + full_text + '"')
        else:
            # probably should be masculine
            if gender != "masc":
                print('WARNING: Probably should be masculine: "' + full_text + '"')

#####################################
# Decline English singular
#####################################
def decline_english_singular(text, article):
    if article == "definite":
        return text
    else:
        # replace "the" with "a" or "an"
        text = re.sub(r"^the ([aAeEiIoOuU])", r"an \1", text)
        text = re.sub(r"^the (.)", r"a \1", text)
        return text

#####################################
# Make English plural
#####################################
def make_english_plural(singular):
    
    plural = re.sub("^(the|a|an) ", "%d ", singular)
    
    if re.search("(gold|fish|folk|spawn|tengu|sheep|swine|efreet|jiangshi|raiju|meliai)$", singular):
        # plural same as singular
        pass
    elif singular.endswith("catoblepas"):
        # catoblepas -> catoblepae
        plural = re.sub("s$", "e", plural)
    elif singular.endswith("cyclops"):
        # cyclops -> cyclopes
        plural = re.sub("s$", "es", plural)
    elif singular.endswith("mage"):
        # mage -> magi
        plural = re.sub("e$", "i", plural)
    elif singular.endswith("simulacrum"):
        # simulacrum -> simulacra
        plural = re.sub("um$", "a", plural)
    elif singular.endswith("eidolon"):
        # eidolon -> eidola
        plural = re.sub("on$", "a", plural)
    elif singular.endswith("djinni"):
        # djinni -> djinn
        plural = re.sub("i$", "", plural)
    elif singular.endswith("foot"):
        # foot -> feet
        plural = re.sub("oot$", "eet", plural)
    elif re.search("(ophan|cherub|seraph)$", singular):
        # add -im
        plural += "im"
    elif singular.endswith("arachi"):
        # barachi -> barachim
        plural += "m"
    elif singular.endswith("ushabti"):
        # ushabti -> ushabtiu
        plural += "u"
    elif singular.endswith("zitzimitl"):
        # Tzitzimitl -> Tzitzimimeh
        plural = re.sub("tl$", "meh", plural)
    elif singular.endswith("mosquito"):
        # mosquito -> mosquitoes (but not gecko -> geckoes)
        plural += "es"
    elif re.search("(larva|antenna|hypha)$", singular):
        # larva -> larvae, etc.
        plural += "e"
    elif singular.endswith("staff"):
        # staff -> staves
        plural = re.sub("ff$", "ves", plural)
    elif re.search("[^f]f$", singular):
        # elf -> elves, but not hippogriff -> hippogrives
        plural = re.sub("f$", "ves", plural)
    elif singular.endswith("fe"):
        # knife -> knives
        plural = re.sub("fe$", "ves", plural)
    elif singular.endswith("ex"):
        # vortex -> vortices
        plural = re.sub("ex$", "ices", plural)
    elif singular.endswith("lotus") or singular.endswith("status"):
        plural += "es"
    elif singular.endswith("us"):
        # fungus -> fungi, ufetubus -> ufetubi
        plural = re.sub("us$", "i", plural)
    elif re.search("[^aeiou]y$", singular):
        # -y -> -ies
        plural = re.sub("y$", "ies", plural)
    elif re.search("(s|sh|ch|x|z)$", singular):
        # add -es
        plural += "es"
    else:
        # add -s
        plural += "s"
    
    return plural

#####################################
# Decline German singular
#####################################
def decline_german_singular(text, article, case):
    
    gender = get_gender(text)
    sing_de = text
    
    if article == "definite":
        if case == "akk":
            sing_de = re.sub("^der ", "den ", sing_de)
        elif case == "dat":
            sing_de = re.sub("^(der|das) ", "dem ", sing_de)
            sing_de = re.sub("^die ", "der ", sing_de)
            
        # adjust adjective ending
        if case == "dat" or (case == "akk" and gender == "masc"):
            # adjective ending is -en
            sing_de = sing_de.replace("e ", "en ")
        
    else:
        
        # adjust article
        if gender == "masc":
            if case == "dat":
                sing_de = re.sub("^der ", "einem ", sing_de)
            elif case == "akk":
                sing_de = re.sub("^der ", "einen ", sing_de)
            else:
                sing_de = re.sub("^der ", "ein ", sing_de)
        if gender == "fem":
            if case == "dat":
                sing_de = re.sub("^die ", "einer ", sing_de)
            else:
                sing_de = re.sub("^die ", "eine ", sing_de)
        if gender == "neut":
            if case == "dat":
                sing_de = re.sub("^das ", "einem ", sing_de)
            else:
                sing_de = re.sub("^das ", "ein ", sing_de)
        
        # adjust adjectives
        if case == "dat" or (case == "akk" and gender == "masc"):
            # adjective ending is -en
            sing_de = sing_de.replace("e ", "en ")
        elif gender == "masc":
            # adjective ending is -er
            sing_de = sing_de.replace("e ", "er ")
        elif gender == "neut":
            # adjective ending is -es
            sing_de = sing_de.replace("e ", "es ")
        
    # apply n-declension to noun
    if gender == "masc" and (case == "akk" or case == "dat"):
        if sing_de.lower().endswith("herr") or sing_de.lower().endswith("bauer"):
            # irregular
            sing_de += "n"
        elif sing_de.endswith("e"):
            if not re.search("(see|käse|deutsche|zombie|gargoyle)$", sing_de, re.IGNORECASE):
                sing_de += "n"
        elif re.search("(at|ant|ent|ist|oph|bär|mensch|prinz|narr)$", sing_de, re.IGNORECASE):
            sing_de += "en"
    elif case == "dat" and gender == "neut" and sing_de.lower().endswith("herz"):
        # a rare neuter noun that declines (in dative only)
        sing_de += "en"
    
    return sing_de

#####################################
# Make German plural
#
# References:
# - https://www.vistawide.com/german/grammar/german_nouns02.htm
# - https://www.germanveryeasy.com/plural
#
#####################################
def make_german_plural(singular, case):
    certain = False
    gender = get_gender(singular)

    plural = re.sub("^(der|die|das) ", "%d ", singular)
    
    if (re.search("(zombie|goblin|wyvern|spriggan|klown|gargoyle)$", singular, re.IGNORECASE) \
       or singular.endswith("Lich")):
        # borrowed English words take -s
        plural += "s"
        certain = True
    elif re.search("[Bb]aum$", singular):
        plural = re.sub("aum$", "äume", plural)
        certain = True
    elif re.search("(kolben|schatten)$", singular, re.IGNORECASE):
        # no change
        certain = True
    elif re.search("geist$", singular, re.IGNORECASE):
        # add -er
        plural += "er"
        certain = True
    elif re.search("bär$", singular, re.IGNORECASE):
        # add -en
        plural += "en"
        certain = True
    elif re.search("segment$", singular, re.IGNORECASE):
        # add -e
        plural += "e"
        certain = True
    elif singular.endswith("e"):
        # nouns ending in -e normally add -n
        plural += "n"
        if  gender == "fem":
            # if it's feminine, then it's certain
            certain = True
    elif re.search("[aoiuy]$", singular):
        # nouns ending in other vowels normally add -s, apart from a few exceptions
        if re.search("(hydra|prisma|tarantella)$", singular, re.IGNORECASE):
            # -a -> -en
            plural = re.sub("a$", "en", plural)
        else:
            # add -s
            plural += "s"
    elif  singular.endswith("chen") or singular.endswith("lein"):
        # no change - guaranteed
        certain = True
    elif  singular.endswith("el") or singular.endswith("er"):
        # feminine nouns add -n (e.g. Kugeln), masc/neut usually don't change, but there are some exceptions
        if  gender == "fem": 
            plural += "n"
        elif re.search("[Tt]ier$", singular): 
            # Tier -> Tiere
            plural += "e"
    elif  singular.endswith("us"):
        if gender == "fem" and singular.endswith("aus"):
            # Fledermäuse, etc.
            plural = re.sub("aus$", "äuse", plural)
            certain = True
        elif singular.endswith("Ufetubus"):
            plural = re.sub("us$", "i", plural)
            certain = True
        else:
            plural = re.sub("us$", "en", plural)
    elif singular.endswith("um"):
            plural = re.sub("um$", "en", plural)
    elif singular.endswith("nis"):
        # nis -> nisse
        plural += "se"
        certain = True
    elif singular.endswith("eur") or singular.endswith("ich") or singular.endswith("ig") or singular.endswith("ling"):
        # always add -e
        plural += "e"
        certain = True
    elif singular.endswith("ör") or singular.endswith("är") or singular.endswith("ar"):
        # probably add -e
        plural += "e"
    elif singular.endswith("at") or singular.endswith("ant") or singular.endswith("ent") or singular.endswith("ist"):
        # always add -en
        plural += "en"
        certain = True
    elif gender == "fem":
        if singular.endswith("in"):
            # Königinnen, etc
            plural += "nen"
            certain = True
        elif singular.endswith("itis"):
            # itis -> iden
            plural = re.sub("itis$", "iden", plural)
            certain = True
        elif singular.endswith("xis"):
            # xis -> xien
            plural = re.sub("xis$", "xien", plural)
            certain = True
        elif singular.endswith("sis"):
            # sis -> sen
            plural = re.sub("sis$", "sen", plural)
            certain = True
        elif re.search("(ung|tion|heit|keit|schaft|tät|ei)$", singular):
            # definitely add -en
            plural += "en"
            certain = True
        else:
            # probably add -en
            plural += "en"
    else:
        # other words normally add -e
        plural += "e"
    
    if case == "dat":
        # in dative, must add extra -n if plural does not already end in -n or -s or -i
        if not re.search("[nsi]$", plural):
            plural += "n"

        # adjective ending is -en
        plural = plural.replace("e ", "en ")
    
    return plural, certain

#####################################
# Process for given article and case
#####################################
def process(infile_name, case_long, article):

    print("Processing " + article + " " + case_long + "...")

    outfile.write("\n")
    outfile.write("################################\n")
    outfile.write("# " + article + " - " + case_long + "\n")
    if case_long == "nominative":
        outfile.write("# (+ accusative for fem./neut.)\n")
    elif case_long == "accusative":
        outfile.write("# (for masc. only)\n")
    outfile.write("################################\n")

    case = case_long[0: 3]
    if case == "acc":
        case = "akk"
    
    english = ""
    deutsch = ""

    with open(infile_name) as infile:
        line = infile.readline()
        
        while line:
            line = line.strip()
            
            if "msgctxt" in line:
                pass
            
            elif line.startswith("msgid"):
                english = extract_text(line)
            
            elif line.startswith("msgstr"):
                deutsch = extract_text(line)
                if english == "":
                    print("ERROR: German with no English: " + deustch)
                
                check_german(deutsch)
                gender = get_gender(deutsch)
                
                if case == "akk" and gender != "masc":
                    # no need to generate accusative for fem./neut. - it's the same as nominative
                    pass
                else:
                    # separate " of <whatever>" suffixes - we will restore later
                    english, suffix_en = split_of(english)
                    deutsch, suffix_de = split_of(deutsch)
                     
                    # msgctxt line
                    outfile.write("\n")
                    if case != "nom": 
                        outfile.write('msgctxt "' + case + '"\n')
                    
                    # msgid line (English)
                    sing_en = decline_english_singular(english, article)
                    
                    if article == "definite":
                        outfile.write('msgid  "' + sing_en + suffix_en + '"\n')
                    else:
                        outfile.write('msgid "' + sing_en + suffix_en + '"\n')
                        plural_en = make_english_plural(english)
                        outfile.write('msgid_plural "' + plural_en + suffix_en + '"\n')
                    
                    # German translations
                    sing_de = decline_german_singular(deutsch, article, case)
                    
                    if article == "definite":
                        outfile.write('msgstr "' + sing_de + suffix_de + '"\n')
                    else:
                        (plural_de, certain) = make_german_plural(deutsch, case)
                        outfile.write('msgstr[0] "' + sing_de + suffix_de + '"\n')    
                        outfile.write('msgstr[1] "' + plural_de + suffix_de + '"\n')    
                        if not certain:
                            outfile.write("# check plural above\n")
                
                english = ""
                deutsch = ""
            
            elif line == "" or line.startswith("#"):
                # comment, blank line
                outfile.write(line)
            else:
                print("ERROR: Invalid line: " + line)
            
            line = infile.readline()
    
    infile.close()

#####################################
# Main
#####################################
if len(sys.argv) != 2:
    print("Usage: decline-de.py <input file>")
    sys.exit(1)

infile_name = sys.argv[1]

outfile_name = "out.po"
#os.remove(outfile_name)
outfile = open(outfile_name,"w+")

print("Writing output to " + outfile_name + "...")

process(infile_name, "nominative","definite")
process(infile_name, "accusative","definite")
process(infile_name, "dative", "definite")
process(infile_name, "nominative", "indefinite")
process(infile_name, "accusative", "indefinite")
process(infile_name, "dative", "indefinite")

outfile.close()
