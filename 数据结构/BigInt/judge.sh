python gen.py >in.txt
g++ -O2 BigInt.cpp 

start_tm=`date +%s%N`;
./a.out <in.txt >cpp.txt 
end_tm=`date +%s%N`;
use_tm=`echo $end_tm $start_tm | awk '{ print ($1 - $2) / 1000000000}'`
echo $use_tm

start_tm=`date +%s%N`;
python std.py <in.txt >py.txt 
end_tm=`date +%s%N`;
use_tm=`echo $end_tm $start_tm | awk '{ print ($1 - $2) / 1000000000}'`
echo $use_tm 

cmp cpp.txt py.txt -c
