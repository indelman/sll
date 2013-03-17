dir=$1
binary_dir=$2

for i in `ls $dir/`
do
	bin_file=`echo $i | sed 's/\.key$/\.bin/'`
	echo $dir/$i
	echo $binary_dir/$bin_file
	./a.out $dir/$i $binary_dir/$bin_file
done

#mv $dir/*.bin $binary_dir/
