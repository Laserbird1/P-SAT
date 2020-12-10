mkdir results 2> /dev/null
for file in ../instances/*;
do
    echo $file;
    cat $file
    (cat $file | time ../bin/xcc-with-sparse-sets) > ./results/sparse-$(basename $file) 2>&1
    cat $file | ../bin/converter s2k
    (cat $file | ../bin/converter s2k | time ../bin/dlx2) > ./results/knuth-$(basename $file) 2>&1

done;
