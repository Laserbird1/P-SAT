# use hyperfine for benchmarks
# use timeout for 10mn max execution
# if [ ! which hyperfine > /dev/null ]; then {brew,apt..} install hyperfine; fi
# if [ ! which timeout > /dev/null ]; then {brew,apt..} install coreutils; fi

mkdir results 2> /dev/null

nwarmup=2
minruns=10
destdir=results

for path in ./instances/* ;
do
    name=$(basename $path)
    jsonname=$(echo $name | cut -f1 -d'.').json
    jsonloc=${destdir}/${jsonname}

    if [ ! -f ${jsonloc} ]; then
    # hyperfine 'cat {path} | timeout -s INT 10m ../bin/{ex}' \
    # --export-json ${destdir}/s_$jsonname \
    # --min-runs $minruns \
    # --time-unit millisecond \
    # --warmup $nwarmup \
    # --parameter-list ex 'xcc-with-sparse-sets' \
    # --parameter-list path $path \
    # --parameter-list file $name

    # hyperfine 'cat {file} | timeout -s INT 10m ../bin/{ex}' \
    # --export-json ${destdir}/k_$jsonname \
    # --min-runs $minruns \
    # --time-unit millisecond \
    # --warmup $nwarmup \
    # --prepare 'cat {path} | ../bin/converter s2k > {file}' \
    # --cleanup 'rm -f {file}' \
    # --parameter-list ex 'dlx2' \
    # --parameter-list path $path \
    # --parameter-list file $name 

    hyperfine 'cat {ex}_{file} | timeout -s INT 10m ../bin/{ex}' \
    --export-json $jsonloc \
    --min-runs $minruns \
    --time-unit millisecond \
    --warmup $nwarmup \
    --cleanup 'rm -f {ex}_{file}' \
    --prepare "if [ {ex} = dlx2 ]; then cat {path} | ../bin/converter s2k > {ex}_{file} ; else cp {path} {ex}_{file}; fi" \
    --parameter-list ex 'dlx2','xcc-with-sparse-sets' \
    --parameter-list path $path \
    --parameter-list file $name ;
    fi
done;

