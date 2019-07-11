Init or update pot:
xgettext --language=C++ --add-comments --sort-output -o po/messages.pot main.cc

Init po file:
msginit --input=po/messages.pot --locale=de --output=po/de/messages.po

Update po file:
msgmerge --update po/de/messages.po po/messages.pot

Generate mo file:
msgfmt --output-file=./locale/de/LC_MESSAGES/messages.mo po/de/messages.po
