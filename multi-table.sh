#! /bin/sh

for((i=1; i<=$1; i++))
do    
    for((j=1; j<=$2; j++))
    do
        let mul=i*j
        printf "%d * %d=%d " "$i" "$j" "$mul"
    done

    printf "\n"
done
