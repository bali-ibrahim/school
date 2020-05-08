TAB=$'\t'

while IFS= read -r line; do
    id="$(echo $line | sed -E 's/(.+?)\t.+/\1/g')"
    name="$(echo $line | sed -E 's/(.+)\t(.+)\t(.+)\t(.+)\t(.+)/\2/g')"
    shares="$(echo $line | sed -E 's/(.+)\t(.+)\t(.+)\t(.+)\t(.+)/\3/g')"
    price2010="$(echo $line | sed -E 's/(.+)\t(.+)\t(.+)\t(.+)\t(.+)/\4/g')"
    price2020="$(echo $line | sed -E 's/(.+)\t(.+)\t(.+)\t(.+)\t(.+)/\5/g')"
    # echo $id $name $shares $price2010 $price2020
    echo $id
    # currency=${line##* }
done <technology_markets_shares.txt
