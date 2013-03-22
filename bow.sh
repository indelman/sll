

#Dubrovnik data
DUBROVNIK_DIR=../../data/Dubrovnik6K/
DUBROVNIK_NAME=dubrovnik

#Rome data
ROME_DIR=../../data/Rome16K/
ROME_NAME=rome

#Vienna data
VIENNA_DIR=../../data/Vienna/
VIENNA_NAME=vienna

#Aachen data
AACHEN_DIR=../../data/Aachen/
AACHEN_NAME=aachen

if [ $1 -eq 1 ]
then
echo 'Testing on dubrovnik'
TEST_DIR=$DUBROVNIK_DIR
TEST_NAME=$DUBROVNIK_NAME
elif [ $1 -eq 2 ]
then 
echo 'Testing on rome'
TEST_DIR=$ROME_DIR
TEST_NAME=$ROME_NAME
elif [ $1 -eq 3 ]
then 
echo 'Testing on vienna'
TEST_DIR=$VIENNA_DIR
TEST_NAME=$VIENNA_NAME
elif [ $1 -eq 4 ]
then 
echo 'Testing on aachen'
TEST_DIR=$AACHEN_DIR
TEST_NAME=$AACHEN_NAME
fi


TEST_NUM=$2

echo 'Using ' $TEST_NUM 'for retrieval'

cd vocsearch
bash run.sh
./a.out $TEST_DIR/list.db.binary.txt $TEST_DIR/list.query.binary.txt $TEST_DIR $TEST_NUM > ../../results/$TEST_NAME/$TEST_NAME.bow.$TEST_NUM 

cd ../generate_bow/
python convert_result.py ../../results/$TEST_NAME/$TEST_NAME.bow.$TEST_NUM $TEST_DIR/list.db.images.txt $TEST_DIR/list.query.images.txt $TEST_DIR/bundle/list.orig.txt > ../../results/$TEST_NAME/$TEST_NAME.bow.$TEST_NUM.orig $TEST_NUM

#cd ../localize_cpp
#bash compile.sh

#rm ../../results/$TEST_NAME/$TEST_NAME.match.$TEST_NUM.ransac
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bundler/lib/
#./localize $TEST_DIR/bundle/key.orig.txt ../../results/$TEST_NAME/$TEST_NAME.bow.$TEST_NUM.orig $TEST_DIR/ ../../results/$TEST_NAME/$TEST_NAME.match.$TEST_NUM.ransac
cd ..

