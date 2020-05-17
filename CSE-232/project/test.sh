./main.exe 1.txt
./main.exe 2.txt
./main.exe 3.txt

actual=mytext.txt
expected=mytext.txt.expected
# https://stackoverflow.com/a/32877921/7032856
cmp --silent $actual $expected && echo '### TEST: SUCCESS - Files Are Identical! ###' || echo '### TEST: FAIL - Files Are Different! ###'
