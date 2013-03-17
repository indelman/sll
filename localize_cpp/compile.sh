export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bundler/lib/

BUNDLER_SRC=bundler/src/
ANN_HEADER=bundler/lib/ann_1.1_char/include

BUNDLER_LIBS=bundler/lib/
BUNDLER_SRC_LIBS=bundler/src/
g++ localize.cpp $BUNDLER_SRC_LIBS/keys2a.o -I $BUNDLER_SRC -I $ANN_HEADER -I $BUNDLER_LIBS/imagelib/ -I $BUNDLER_LIBS/matrix -L $BUNDLER_LIBS -L $BUNDLER_SRC_LIBS -lz -lANN_char -limage -lmatrix -lblas -lcblas -llapack -lminpack -o localize 

