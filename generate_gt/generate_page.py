import sys

# list.orig.txt groundtruth_result
name_list = [];
f=open(sys.argv[1]);

for i in f.readlines():
	name_list.append(i.split('/')[1].strip('\n'));
f.close();

f=open(sys.argv[2]);

for i in f.readlines():
	res = i.split(' ');
	out= "<img src=query_images/"+name_list[int(res[0])]+" width=100 height=100/> &nbsp; &nbsp; "
	for j in range(1,10):
		out= out+"<img src=db_images/"+name_list[int(res[j])]+" width=100 height=100/>"
		
	out = out + '<br>'
	print out


		


	
