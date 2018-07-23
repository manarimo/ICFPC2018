#!/bin/zsh

# yuusti
#URL="http://kadingel.osak.jp/job/yuusti%E3%81%AEAI%E3%81%B6%E3%82%93%E5%9B%9E%E3%81%99%E3%81%9E/buildWithParameters"

# kawatea
#URL="http://kadingel.osak.jp/job/kawatea%E3%81%AEAI%E3%81%B6%E3%82%93%E5%9B%9E%E3%81%99%E3%81%9E/buildWithParameters"

# mkut
#URL="http://kadingel.osak.jp/job/mkut%E3%81%AEAI%E3%81%B6%E3%82%93%E5%9B%9E%E3%81%99%E3%81%9E/buildWithParameters"

URL="http://kadingel.osak.jp/job/%E8%AA%B0%E3%81%8B%E3%81%AEAI%E3%81%B6%E3%82%93%E5%9B%9E%E3%81%99%E3%81%9E"

#for i in {001..186}; do
for i in {187..199}
    echo $i
    curl "${URL}?PROBLEM_ID=FD${i}&AUTHOR=kawatea&COMMENT=shiva&token=a&RUN_SCRIPT=ai/kawatea/run_shiva.sh"
done
