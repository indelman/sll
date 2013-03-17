import sys;

# bow_result list.db.images.txt list.query.images.txt

result_file = sys.argv[1];

list_db=[];
list_query=[];

f=open(sys.argv[2]);

for i in f.readlines():
	list_db.append(i);

f.close();

f=open(sys.argv[3]);

for i in f.readlines():
	list_query.append(i);

f.close();

f = open(result_file);

count = 0;
for i in f.readlines():
        out= "<img src="+list_query[count]+" width=100 height=100/> &nbsp; &nbsp; "
	res = i.split(' ');
	for j in range(0,10):
	        out= out+ "<img src="+list_db[int(res[j])]+" width=100 height=100/> &nbsp; &nbsp; "
	out = out + '<br>'
	print out
	count = count + 1


	
	

