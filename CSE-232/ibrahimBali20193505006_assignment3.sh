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

compare_the_files_and_add_lines() {
    local filenames=($1 $2)

    for f in ${filenames[@]}; do
        add_newline_if_missing $f
    done

    ((number_of_lines_1_has_more_than_2 = $(wc -l <${filenames[0]}) - $(wc -l <${filenames[1]})))

    # echo $number_of_lines_1_has_more_than_2

    local file_that_misses_lines_index=1
    if [ $number_of_lines_1_has_more_than_2 -lt 0 ]; then
        ((number_of_lines_1_has_more_than_2 = -$number_of_lines_1_has_more_than_2))
        file_that_misses_lines_index=0
    fi

    local file_that_misses_lines=${filenames[${file_that_misses_lines_index}]}

    # TODO write to file use function
    echo "File $file_that_misses_lines missing $number_of_lines_1_has_more_than_2 entries." >compare.txt

    # (you can write any product name with its price)
    random_line=$(head -n 1 $file_that_misses_lines)

    until [ $number_of_lines_1_has_more_than_2 -eq 0 ]; do
        echo $random_line >>$file_that_misses_lines
        let number_of_lines_1_has_more_than_2-=1
    done

    return 0
}

is_sum_odd_or_even() {
    local sum=0

    local currency
    while IFS= read -r line; do
        ((sum += "$(echo $line | sed -E 's/.+\s([0-9]+)\s.+/\1/g')"))
        currency=${line##* }
    done <$1

    local market_name="$(echo $1 | sed -E 's/(.*)\.[^.]+/\1/g')"

    local is_even_or_odd
    if (($sum % 2)); then
        is_even_or_odd="odd"
    else
        is_even_or_odd="even"
    fi

    # echo $is_even_or_odd

    echo "$market_name market products sum is $sum $currency and it's $is_even_or_odd." >>$2
    # echo $market_name
    # echo $sum
    return 0
}

main() {

    local filenames=($1 $2)

    if does_not_exist_or_empty ${filenames[0]} || does_not_exist_or_empty ${filenames[1]}; then
        return 2
    fi

    compare_the_files_and_add_lines ${filenames[0]} ${filenames[1]}

    local odd_even_filename="odd_even.txt"
    rm $odd_even_filename

    for f in ${filenames[@]}; do
        is_sum_odd_or_even $f $odd_even_filename
    done

    return 0
}

main $1 $2
if [ $? -eq 2 ]; then
    echo "Error: At least one of $filename1, $filename2 does not exist or is empty."
fi
