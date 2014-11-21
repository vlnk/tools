gcc -lm -fopenmp merge-sort-simple.c -o merge-sort-simple
gcc -lm -fopenmp merge-sort-recursive.c -o merge-sort-recursive
gcc -lm -fopenmp merge-sort-parallel.c -o merge-sort-parallel

./merge-sort-simple 10 > merge-sort-simple.log
./merge-sort-simple 100 >> merge-sort-simple.log
./merge-sort-simple 1000 >> merge-sort-simple.log
./merge-sort-simple 10000 >> merge-sort-simple.log
./merge-sort-simple 100000 >> merge-sort-simple.log
./merge-sort-simple 1000000 >> merge-sort-simple.log
./merge-sort-simple 10000000 >> merge-sort-simple.log
./merge-sort-simple 100000000 >> merge-sort-simple.log
./merge-sort-simple 1000000000 >> merge-sort-simple.log

./merge-sort-recursive 10 >> merge-sort-recursive.log
./merge-sort-recursive 100 >> merge-sort-recursive.log
./merge-sort-recursive 1000 >> merge-sort-recursive.log
./merge-sort-recursive 10000 >> merge-sort-recursive.log
./merge-sort-recursive 100000 >> merge-sort-recursive.log
./merge-sort-recursive 1000000 >> merge-sort-recursive.log
./merge-sort-recursive 10000000 >> merge-sort-recursive.log
./merge-sort-recursive 100000000 >> merge-sort-recursive.log
./merge-sort-recursive 1000000000 >> merge-sort-recursive.log

./merge-sort-parallel 10 4 >> merge-sort-parallel.log
./merge-sort-parallel 100 4 >> merge-sort-parallel.log
./merge-sort-parallel 1000 4 >> merge-sort-parallel.log
./merge-sort-parallel 10000 4 >> merge-sort-parallel.log
./merge-sort-parallel 100000 4 >> merge-sort-parallel.log
./merge-sort-parallel 1000000 4 >> merge-sort-parallel.log
./merge-sort-parallel 10000000 4 >> merge-sort-parallel.log
./merge-sort-parallel 100000000 4 >> merge-sort-parallel.log
./merge-sort-parallel 1000000000 4 >> merge-sort-parallel.log
