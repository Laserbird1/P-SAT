# use hyperfine for benchmarks
# use timeout for 10mn max execution
# if [ ! which hyperfine > /dev/null ]; then {brew,apt..} install hyperfine; fi
# if [ ! which timeout > /dev/null ]; then {brew,apt..} install coreutils; fi

nwarmup=1
minruns=2
destdir=results_car2
mkdir ${destdir} 2> /dev/null

for path in ../src/cars/instances/tooheavy/*.txt ;
do
    name=$(basename $path)
    jsonname=$(echo $name | cut -f1 -d'.').json
    jsonloc=${destdir}/${jsonname}

    if [ ! -f ${jsonloc} ]; then
        hyperfine 'cat {ex}_{file} | timeout 300s ../bin/{ex} `if [ {ex} = dlx2 ]; then echo t1; else echo -f; fi;`|| [ $? = 124 ]' \
        --export-json $jsonloc \
        --min-runs $minruns \
        --time-unit millisecond \
        --warmup $nwarmup \
        --cleanup 'rm -f {ex}_{file}' \
        --prepare "if [ {ex} = dlx2 ]; then cat `echo ${path}` | ../bin/converter_v2 s2k > {ex}_{file} ; else cp `echo ${path}` {ex}_{file}; fi" \
        --parameter-list ex 'dlx2','xcc-with-sparse-sets' \
        --parameter-list file $name ;
    fi
done;
