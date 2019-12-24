python3 checkFont.py $1 | grep -c -E "\([0-9]+," > charlist.txt
python3 checkFont.py $1 | grep -o -E "\([0-9]+," >> charlist.txt
