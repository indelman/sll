1. Run binarize with db image directory and query image directory to form the corresponding directories with binary descriptors

2. Create ground truth data if given (in case of Dubrovnik and Rome it is present)

./a.out bundle.orig.out (in utils)
bash generate_gt.sh list.orig.txt cam_array (in generate_gt)

3. Run Vocsearch (for BOW results)

./a.out list.db.binary.txt list.query.binary.txt data_directory
bash convert_result.sh BOW_results list.db.images.txt list.query.images.txt list.orig.txt 


4. Match and RANSAC
./localize key.orig.txt BOW_or_GT data_directory output_file
