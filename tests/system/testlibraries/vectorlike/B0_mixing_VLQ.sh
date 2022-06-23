mkdir -p bin obj scripts/obj # ensures directories are created
rm table.txt
bash generic/loopovermodels.sh testlib d s NoBoson # Loop over 28 VLQ models

../../bin/comparedata.x table.txt table_benchmark.txt 1e-5 # Compare the results with the benchmark
exit $?
