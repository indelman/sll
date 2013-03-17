import sys;

# bow_result list.db.images.txt list.query.images.txt list.orig.images.txt

result_file = sys.argv[1];

list_db=[];
list_query=[];
list_orig=[];


# reading list.db.images.txt
f=open(sys.argv[2]);

for i in f.readlines():
	list_db.append(i.split('/')[1].strip('\n'));
	

f.close();
# reading list.query.images.txt
f=open(sys.argv[3]);

for i in f.readlines():
	list_query.append(i.split('/')[1].strip('\n'));

f.close();

# reading list.orig.images.txt
f=open(sys.argv[4]);
for i in f.readlines():
	list_orig.append(i.split('/')[1].strip('\n'));
f.close();

range_val=int(sys.argv[5]);

# reading bow result file
f = open(result_file);

count = 0;
for i in f.readlines():
        #out= "<img src="+list_query[count]+" width=100 height=100/> &nbsp; &nbsp; "
	print list_orig.index(list_query[count]),
	res = i.split(' ');
	for j in range(0,range_val):
		print list_orig.index(list_db[int(res[j])]),
	print '\n',
	#        out= out+ "<img src="+list_db[int(res[j])]+" width=100 height=100/> &nbsp; &nbsp; "
	#out = out + '<br>'
	#print out
	count = count + 1


	
	

