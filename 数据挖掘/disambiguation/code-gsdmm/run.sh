datetime=`date "+%m%d_%H%M%S"`
mkdir log/$datetime

runscript() {
    echo `date "+%m %d %H:%M:%S"`":begin running script $1" 
    python $1.py > log/$datetime/$1.log 2>&1
    echo `date "+%m %d %H:%M:%S"`":finish running script $1"
}

scripts=(
    # "download"
    # "data_process" 
    # "gsdmm"
    # "make_cluster" 
    "merge"
    "f1")

for script in ${scripts[@]}
do
    runscript $script
done