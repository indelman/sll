export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bundler/lib/:/usr/local/lib/

BUNDLER_SRC=bundler/src/
ANN_HEADER=bundler/lib/ann_1.1_char/include
CLUSTER_SRC=../cluster/cluster-1.50/src/
BUNDLER_LIBS=bundler/lib/
BUNDLER_SRC_LIBS=bundler/src/


CURR_DIR=/home/siddharth/Research/visprob/gdb-v1
GETFEM_DIR=$CURR_DIR/lib/getfem-4.1.1/ 
VOCSEARCH_DIR=$CURR_DIR/lib/vocsearch/
ACG_LOCALIZER=$CURR_DIR/lib/ACG_Localizer_v_1_2_1
OPENMESH_DIR=$CURR_DIR/lib/OpenMesh-2.3.1

LIBS_ACG=$ACG_LOCALIZER/build/src/CMakeFiles/acg_localizer.dir/



g++ localize.cpp $BUNDLER_SRC_LIBS/keys2a.o ../geom_vocsearch/compute_scores.o $LIBS_ACG/RANSAC.cc.o $LIBS_ACG/timer.cc.o $LIBS_ACG/math/math.cc.o $LIBS_ACG/math/pseudorandomnrgen.cc.o $LIBS_ACG/solver/solverbase.cc.o $LIBS_ACG/solver/solverproj.cc.o $LIBS_ACG/math/matrix3x3.cc.o $LIBS_ACG/math/matrix4x4.cc.o $LIBS_ACG/math/matrixbase.cc.o $LIBS_ACG/math/projmatrix.cc.o $LIBS_ACG/math/SFMT_src/SFMT.cc.o -I $BUNDLER_SRC -I $ANN_HEADER -I $BUNDLER_LIBS/imagelib/ -I $BUNDLER_LIBS/matrix -I$ACG_LOCALIZER/src/ -I$GETFEM_DIR/src/ -I$OPENMESH_DIR/src/ -I ../geom_vocsearch -I $BUNDLER_LIBS/matrix -L ../geom_vocsearch -L $BUNDLER_LIBS -L $BUNDLER_SRC_LIBS -lz -lANN_char -limage -lmatrix -lblas -lcblas -llapack -lminpack -lboost_system -lgtsam -lransac -o localize 

