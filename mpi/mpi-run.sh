NODES=6

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ]; then
    echo "USE: $0 <app> <number_of_node> <args>"
elif [ $2 -gt $NODES ] && [ $2 -lt 0 ]; then
    echo "ERROR: number of nodes invalid"
else
    if mpicc -o $1 $1.c; then
        while [ $NODES -gt 1 ]; do
            scp -r "$1" "server$NODES:$1"
            let NODES=NODES-1
        done

        clear
        mpirun -np $2 $1 $3
    fi
fi
