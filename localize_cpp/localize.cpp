#include <assert.h>
#include <time.h>
#include <string.h>

#include "keys2a.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include "fmatrix.h"
#include "vector.h"
#include <stdlib.h>

using namespace std;

/* Estimate an F-matrix from a given set of point matches */
std::vector<int> EstimateFMatrix(const std::vector<Keypoint> &k1,
                                 const std::vector<Keypoint> &k2,
                                 std::vector<KeypointMatch> matches,
                                 int num_trials, double threshold,
                                 double *F, bool essential)
{
    int num_pts = (int) matches.size();

    /* num_pts should be greater than a threshold */
    if (num_pts < 20) {
        std::vector<int> inliers;
        return inliers;
    }

    v3_t *k1_pts = new v3_t[num_pts];
    v3_t *k2_pts = new v3_t[num_pts];

    v3_t *k1_pts_in = new v3_t[num_pts];
    v3_t *k2_pts_in = new v3_t[num_pts];

    for (int i = 0; i < num_pts; i++) {
        int idx1 = matches[i].m_idx1;
        int idx2 = matches[i].m_idx2;

        assert(idx1 < (int) k1.size());
        assert(idx2 < (int) k2.size());

        k1_pts[i] = v3_new(k1[idx1].m_x, k1[idx1].m_y, 1.0);
        k2_pts[i] = v3_new(k2[idx2].m_x, k2[idx2].m_y, 1.0);
    }

    estimate_fmatrix_ransac_matches(num_pts, k2_pts, k1_pts,
        num_trials, threshold, 0.95,
        (essential ? 1 : 0), F);

    /* Find the inliers */
    std::vector<int> inliers;

    for (int i = 0; i < num_pts; i++) {
        double dist = fmatrix_compute_residual(F, k2_pts[i], k1_pts[i]);
        if (dist < threshold) {
            inliers.push_back(i);
        }
    }

    /* Re-estimate using inliers */
    int num_inliers = (int) inliers.size();

    for (int i = 0; i < num_inliers; i++) {
        k1_pts_in[i] = k1_pts[inliers[i]]; // v3_new(k1[idx1]->m_x, k1[idx1]->m_y, 1.0);
        k2_pts_in[i] = k2_pts[inliers[i]]; // v3_new(k2[idx2]->m_x, k2[idx2]->m_y, 1.0);
    }

    // printf("[1] num_inliers = %d\n", num_inliers);

#if 0
    double F0[9];
    double e1[3], e2[3];
    estimate_fmatrix_linear(num_inliers, k2_pts_in, k1_pts_in, F0, e1, e2);

    inliers.clear();
    for (int i = 0; i < num_pts; i++) {
        double dist = fmatrix_compute_residual(F0, k2_pts[i], k1_pts[i]);
        if (dist < threshold) {
            inliers.push_back(i);
        }
    }
    num_inliers = inliers.size();
    // printf("[2] num_inliers = %d\n", num_inliers);

    // matrix_print(3, 3, F0);
#else
    double F0[9];
    memcpy(F0, F, sizeof(double) * 9);
#endif

    if (!essential) {
        /* Refine using NLLS */
        for (int i = 0; i < num_inliers; i++) {
            k1_pts_in[i] = k1_pts[inliers[i]];
            k2_pts_in[i] = k2_pts[inliers[i]];
        }

        refine_fmatrix_nonlinear_matches(num_inliers, k2_pts_in, k1_pts_in,
            F0, F);
    } else {
        memcpy(F, F0, sizeof(double) * 9);
    }

#if 0
    if (essential) {
        /* Compute the SVD of F */
        double U[9], S[3], VT[9];
        dgesvd_driver(3, 3, F, U, S, VT);
        double E0[9] = { 1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 0.0 };

        double tmp[9];
        matrix_product(3, 3, 3, 3, U, E0, tmp);
        matrix_product(3, 3, 3, 3, tmp, VT, F);
    }
#endif

    inliers.clear();
    for (int i = 0; i < num_pts; i++) {
        double dist = fmatrix_compute_residual(F, k2_pts[i], k1_pts[i]);
        if (dist < threshold) {
            inliers.push_back(i);
        }
    }
    num_inliers = (int) inliers.size();

    delete [] k1_pts;
    delete [] k2_pts;
    delete [] k1_pts_in;
    delete [] k2_pts_in;

    return inliers;

}

/* Match the keys */
vector < pair<int, int> > matchKeys( vector<int> indices, const vector<string>& key_files, int ret_cnt)
{

    float ratio = 0.7;

    double F[9];
    int query=indices[0]; // query image index

    unsigned char **keys = new unsigned char *[indices.size()];
    int *num_keys = new int[indices.size()];
    keypt_t** info = new keypt_t*[indices.size()];

    vector < vector < Keypoint > > KeypointVec;
    for(int i=0; i < ret_cnt; i++)
    {
        num_keys[i] = ReadKeyFile(key_files[indices[i]].c_str(), keys+i, info+i); // Read Key files

        // create keypoint vector
        vector < Keypoint > keypoint_temp;
        keypt_t* info_t = info[i];
        unsigned char* key_t = keys[i];

        for(int j=0;j< num_keys[i]; j++)
        {
            keypoint_temp.push_back( Keypoint(info_t[j].x, 
                        info_t[j].y,
                        info_t[j].scale,
                        info_t[j].orient,
                        (short int*) key_t + 128*j
                        ));

        }
        KeypointVec.push_back(keypoint_temp);

    }
    // Create the search tree of the query image
    //ANNkd_tree *tree = CreateSearchTree(num_keys[0],keys[0]);	

    // Create Result vector
    vector < pair < int, int > > result;

    // Match query image with the rest of the images
    for(int i=1;i<ret_cnt; i++)
    {
        // create the tree
        ANNkd_tree *nn_tree = CreateSearchTree(num_keys[i],keys[i]);

        std::vector<KeypointMatch> matches =
           MatchKeys(num_keys[0], keys[0], nn_tree, ratio);


        if( (int)matches.size() >= 16)
        {


            // run ransac

            // normalize x,y locations according to image dimensions

            // TODO: normalize it later, right now run according un normalized constraints
            const vector<int>& inliers = EstimateFMatrix(KeypointVec[0], KeypointVec[i], matches, 2048, 9.0f, F, false);		
            // push into result vector
            result.push_back(make_pair(matches.size(), inliers.size()));

        }
        else
        {
            result.push_back(make_pair(matches.size(), -1));
         }


         annDeallocPts(nn_tree->pts);
        delete nn_tree;
    }


    for(int i=0; i < ret_cnt; i++)
    {
        delete [] keys[i];
        delete [] info[i];
    }
    delete [] keys;
    delete [] info;
    //delete tree;
    delete [] num_keys;
    KeypointVec.clear();
    return result;
}


/* Read the image list */

const vector<string> readList(char* list_in, char* dir)
{
	
    std::vector<std::string> key_files;

    FILE *f = fopen(list_in, "r");
    if (f == NULL) {
        printf("Error opening file %s for reading\n", list_in);
        return key_files;
    }

    char buf[512];
    char temp[512];
    while (fgets(buf, 512, f)) {
        /* Remove trailing newline */
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

	sprintf(temp,"%s/%s",dir,buf);
        key_files.push_back(std::string(temp));
    }
	return key_files;

}

/* Read the index list */

const vector< vector<int> > readIndex(char* list_in)
{

	vector < vector <int> > list_index;

	string str;
	ifstream infile;
	infile.open (list_in);
	while(!infile.eof()) // To get you all the lines.
	{
		vector < int > temp;
		getline(infile,str); // Saves the line in STRING.
		std::stringstream ss;
		ss << str;
		while(!ss.eof())
		{
			int number;
			ss >> number;
			temp.push_back(number);
		}
		temp.erase(temp.end()-1);
		list_index.push_back(temp);
	}
	infile.close();

	return list_index;

}



int main(int argc, char* argv[])
{
	// ./localize <list fiile> <rank file> <directory> <output file>

	// Read the list of key files
	char* dir = argv[3]; //directory of the dataset
	const vector<string>& key_files = readList(argv[1],dir);

	// Read the indices
	const vector< vector<int> >& index_file = readIndex(argv[2]);

	// Opens the file to write results
	ofstream f_out(argv[4]);
	
    int ret_cnt= strtol(argv[5],NULL,10); // retrieve argv[5] images

	// Match Keys
	for(int i=0;i<index_file.size()-1; i++)
	{	
		for(int j=0;j<ret_cnt;j++)
			cout << index_file[i][j] << " ";
		cout << endl;
		const vector < pair <int, int> >& result = matchKeys(index_file[i],key_files,ret_cnt);

		for(int j=0;j<result.size(); j++)
			f_out << result[j].first << " " << result[j].second << " ";
		f_out << endl;
	}
	f_out.close();

	

}
