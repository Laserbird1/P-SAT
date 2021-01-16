# use time for ram benchmarks on unix

nwarmup=1
minruns=2
destdir=results_cars_memory
mkdir ${destdir} 2> /dev/null
execs=( "dlx2" "xcc-with-sparse-set" )


for path in ../src/cars/instances/ok/*.txt;
do
    name=$(basename $path)
    jsonname=$(echo $name | cut -f1 -d'.').json
    jsonloc=${destdir}/${jsonname}
    printf '{\n "results" : [\n' > "${jsonloc}"
    for ex in "${execs[@]}";
    do
        instancepath=${path}
        printf '    {\n      "max_resident_set_size_kb" : [\n        ' >> "${jsonloc}"
        if [ ${ex} = 'dlx2' ]; then
            instancepath=./tmp_${name} 
            ../bin/converter_v2 s2k < ${path} > ./tmp_${name} 
            wait;
        fi;
        average=0;
        for i in {1..5};
        do
            /usr/bin/time -v -o ./tmp_output.txt ../bin/${ex} $(if [ ${ex} = 'dlx2' ]; then echo 't1'; else echo '-f'; fi; ) < ${instancepath}
            wait
            memtime=$(cat ./tmp_output.txt | grep "Maximum resident set size " | cut  -d: -f2  | cut -d' ' -f2 | tr -d $'\n')
            average=$(( average + memtime ))
            printf ${memtime} >> ${jsonloc}
            if [ ${i} != 5 ]; then printf ',\n        ' >> ${jsonloc}; fi;
        done
        printf '\n      ],\n' >> ${jsonloc}
        average=$(( average / 5 ))
        printf '      "average_max_resident_set_size_kb" : '${average}',\n' >> ${jsonloc}
        printf '      "parameters" : {\n        "ex" : "'${ex}'",\n        "file" : "'${name}'"\n' >> ${jsonloc}
        printf '      }\n' >> ${jsonloc}
        printf '    }' >> ${jsonloc}
        if [ ${ex} = 'dlx2' ]; then
            printf ',\n' >> ${jsonloc}
            rm -f ${instancepath}
        fi;   
        echo "${jsonname}";
    done;
    printf '\n  ]\n}\n' >> ${jsonloc}
done;
rm -f tmp_output.txt