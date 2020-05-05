#!/bin/bash

does_not_exist_or_empty() {
    if [ -s $1 ]; then
        return 1
    else
        return 0
    fi
}

newline_at_eof() {
    if [ -z "$(tail -c 1 "$1")" ]; then
        return 0
    else
        return 1
    fi
}

add_newline_if_missing() {
    if newline_at_eof $1; then
        echo
    else
        printf "\n" >>$1
    fi
}

main() {

    local filenames=($1 $2)

    if does_not_exist_or_empty ${filenames[0]} || does_not_exist_or_empty ${filenames[1]}; then
        return 2
    fi

    # wc -l <${filenames[0]}
    # wc -l <${filenames[1]}

    for f in ${filenames[@]}; do
        add_newline_if_missing $f
    done

    ((number_of_lines_1_has_more_than_2 = $(wc -l <${filenames[0]}) - $(wc -l <${filenames[1]})))

    echo $number_of_lines_1_has_more_than_2

    local file_that_misses_lines_index=1

    if [ $number_of_lines_1_has_more_than_2 -lt 0 ]; then
        ((number_of_lines_1_has_more_than_2 = -$number_of_lines_1_has_more_than_2))
        file_that_misses_lines_index=0
    fi

    local file_that_misses_lines=${filenames[${file_that_misses_lines_index}]}

    # TODO write to file use function
    echo "File $file_that_misses_lines missing $number_of_lines_1_has_more_than_2 entries." > compare.txt

    random_line=$(head -n 1 $file_that_misses_lines)

    until [ $number_of_lines_1_has_more_than_2 -eq 0 ]; do
        echo $random_line >>$file_that_misses_lines
        let number_of_lines_1_has_more_than_2-=1
    done

    return 0
}

main $1 $2
if [ $? -eq 2 ]; then
    echo "Error: At least one of $filename1, $filename2 does not exist or is empty."
fi
