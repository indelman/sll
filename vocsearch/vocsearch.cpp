/* Copyright (c) 2008, Eidgenössische Technische Hochschule Zürich, ETHZ
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Eidgenössische Technische Hochschule Zürich, ETHZ "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL the Eidgenössische Technische Hochschule Zürich, ETHZ BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Author: Friedrich Fraundorfer, fraundorfer@inf.ethz.ch

#include "stdio.h"
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "ff_voctree.h"
#include "ff_database.h"
#include "ff_sort.h"

#include <vector>
#include <string>
#include <fstream>
#include "math.h"
using namespace std;


vector<string> read (string file)
{

string line;
vector <string> list;
ifstream file_in(file.c_str(), ios::in);

if(file_in.is_open())
{
	while(file_in.good())
	{
		getline(file_in,line);	
		list.push_back(line);
//		cout << line << endl;
	}

}
file_in.close();

return list;

}

int main(int argc, char **argv) {

	// read list

	if(argc < 4)
	{
		std::cerr << "./a.out db.list query.list directory" << std::endl;
		return 0;
	}
	
	string arg1(argv[1]); // convert to a string 
	vector<string> db_list = read(arg1);
//	cout << "Size of DB Images " << db_list.size() << endl;
	
	string arg2(argv[2]); // convert to a string
	vector<string> query_list = read(arg2);
//	cout << "Size of Query Images " << query_list.size() << endl;

	int ret_cnt= strtol(argv[4],NULL,10); // retrieve argv[4] images
	

	int imgnr = 1000;
	int maxnrvw = 2000;

	FILE *fin;
	unsigned char *sift = new unsigned char[maxnrvw*128];

	// load voctree
	ff_voctree voctree;
	voctree.init("voctrees/voctree.bin");
//	cout << "Voctree loaded." << endl;
//	cout << "Levels: " << voctree.nrlevels() << endl;
//	cout << "Branch factor: " << voctree.nrsplits() << endl;
//	cout << "VW's: " << voctree.nrvisualwords() << endl;

	ff_database db;
	db.init(imgnr, voctree.nrvisualwords(), maxnrvw);
//	cout << "Database initialized." << endl;

	unsigned int *vwread = new unsigned int[maxnrvw];

	int nr = imgnr;
	char strin[256];
	int size;


//	cout << "Reading SIFT-features from files." << endl;
	for (int i = 0; i < db_list.size() - 1; i++) {
		sprintf(strin, "%s/%s", argv[3],db_list[i].c_str()); 
		fin = fopen(strin, "rb");
		size = (int)fread(sift, sizeof(unsigned char), maxnrvw*128, fin)/128;
		fclose(fin);
		for (int k = 0; k < size; k++) {
			voctree.quantize(&vwread[k], &sift[k*128]);
		}

		//ff_heapsort sorter;
		//sorter.sortarray(vwread,size);  // VW need to be sorted and unique if matches are to be retrieved from the database
		//unique(vwread, (unsigned int*)&size);

		db.insertdoc(vwread, size, i);
	}
//	cout << "Done." << endl;

	// compute idf score of the images in the database
	db.computeidf();
	// update normalization in database according to changed idf-weights
	db.normalize();


	int qdocnames[100];
	float qscores[100]; 
	double recval = 0;

	// scoring with invfile class
//	cout << "Reading SIFT-features and querying database." << endl;
	for (int i = 0; i < query_list.size() - 1; i++) {
			sprintf(strin, "%s/%s", argv[3],query_list[i].c_str()); 
			fin = fopen(strin, "rb");
			size = (int)fread(sift, sizeof(unsigned char), maxnrvw*128, fin)/128;
			fclose(fin);
			for (int k = 0; k < size; k++) {
				voctree.quantize(&vwread[k], &sift[k*128]);
			}

			db.querytopn(vwread, size, ret_cnt, qdocnames, qscores);   

			for(int j=0; j<ret_cnt;j++)
			cout << qdocnames[j] << " ";
			cout << endl;	
			// The database is containing 4 instances of each object. Ideally for
			// every query the 4 top ranked images will be of the same object. This
			// will be computed by recval here.
			// Max. possible recval for this experiment will be 4.
			
	}
	
//	cout << "recval: " << recval/(double)(nr) << endl; 

return 0;
}
