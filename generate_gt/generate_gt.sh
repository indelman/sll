
#list.orig.txt cam_array
python parse.py $1 > db_or_not
python generate_list.py db_or_not $2 > groundtruth_result
python generate_page.py $1 groundtruth_result > gt.html
