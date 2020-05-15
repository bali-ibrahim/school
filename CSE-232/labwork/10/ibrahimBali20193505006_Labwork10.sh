# ibrahimBali20193505006_Labwork10

input_filename=input_file.txt
sep="\t"

echo -e "Banyan tree${sep}India${sep}21 m height" >${input_filename}
echo -e "Alder tree${sep}Uk${sep}28 m height" >>$input_filename
echo -e "Cypress tree${sep}Mexico${sep}42 m width" >>$input_filename
echo -e "Black pines${sep}Japan${sep}40 m height" >>$input_filename
echo -e "Scotch Pine${sep}Turkey${sep}30 m height" >>$input_filename

echo 1
cat $input_filename | grep '^B'
echo 2
cat $input_filename | awk ' $2 ~/e$/'
echo 3
cat $input_filename | grep -E 'e{2,}'
echo 4
cat $input_filename | grep -E 'es'
echo 5
cat $input_filename | awk "BEGIN {FS=\"${sep}\"}; /[pt]r/{print \$1, FS, \$3}"
