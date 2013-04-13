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
#include "compute_scores.h"
#include "gtsam/geometry/SimpleCamera.h"
#include "ransac/estimateCamera.h"

#include "matrix.h"
//#include "gtsam

using namespace std;

int estimate_error =0;
vector < vector <double> > bow_index;
vector < vector <int> > db_index;
bundle* poses;
list< pair <gtsam::Point3, gtsam::Point2> > corr;
vector < pair <int, int> > dimensions;

vector < gtsam::Pose3 > actual_poses;

//void computeError(

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

int findNextImage(vector<double> bow_indices)
{
    double min = DBL_MAX;
    int index = -1;
//    cout << endl << "----------------------------" << endl;
    for(int i=0; i<bow_indices.size(); i++)
    {
    //    cout << bow_indices[i] << " ";
//        cout << bow_indices[i] << " ";
         if( bow_indices[i] !=-1 && bow_indices[i] < min)
         {
         
             min = bow_indices[i];
             index = i;
         }
    }

//    cout << endl << "----------------------------" << endl;

    cout << "Min: " << min << " Index: " << index << endl;
    if(index == -1)return -1;
    return index;
}


void computeError(int id, int index, vector<Keypoint> Keypoint_q, vector<Keypoint> Keypoint_r, vector<KeypointMatch> matches, vector<int> inliers)
{

    corr.clear(); // comment it if you want union

    for(int i=0; i<inliers.size(); i++)
    {
        int idx1=matches[i].m_idx1;
        int idx2=matches[i].m_idx2;

        
       float x0= Keypoint_q[idx1].m_x;
       float y0 = Keypoint_q[idx1].m_y;

       float x1 = Keypoint_r[idx2].m_x;
       float y1 = Keypoint_r[idx2].m_y;

       float W = dimensions[id].first;
       float H = dimensions[id].second;

        int point_index = poses->getPoint(index, idx2);

        if(point_index != -1)
        {
                x0 = x0 - W/2;
                y0 = H/2 - y0;
                float X = poses->vertexset[point_index].m_pos[0];
                float Y = poses->vertexset[point_index].m_pos[1];
                float Z = poses->vertexset[point_index].m_pos[2];

                corr.push_back(make_pair (gtsam::Point3(X,Y,Z), gtsam::Point2(x0,y0)));

//            corr.push_back
        }
    }

    if(corr.size() < 6) return;
    // estimate Pose

    gtsam::SimpleCamera cam = estimateCamera(corr);

    gtsam::Cal3_S2 K = cam.calibration();
    cout << "Calibration: " << endl;
    K.print();
    gtsam::Pose3 Pose = cam.pose();
    gtsam::Point3 point = Pose.translation();
    gtsam::Rot3 rot = Pose.rotation();

    

    // actual pose
    gtsam::Pose3 actual_pose = actual_poses[id];
    gtsam::Point3 actual_trans = actual_pose.translation();
    gtsam::Rot3 actual_rot = actual_pose.rotation();


   // estimate error
   double error_dist = actual_trans.dist(point);
    cout << "Translation Error " << error_dist << endl;  
    
     

    
}



/* Match the keys */
vector < pair<int, pair<int,int> > > matchKeys( vector<int> indices, vector<double> bow_indices, vector<int> db_indices, const vector<string>& key_files, int ret_cnt, int id )
{

    float ratio = 0.7;

    double F[9];
    
    int query=indices[0]; // query image index

    
        unsigned char **query_keys = new unsigned char*[1];
        int query_num_keys;
        keypt_t** query_info = new keypt_t*[1];

        query_num_keys = ReadKeyFile(key_files[indices[0]].c_str(), query_keys, query_info); // Read Key files

     //   cout << query_num_keys << endl;

       // cout << key_files[indices[0]].c_str() << endl;

       double* dist_arr = new double[db_indices.size()];
        // create keypoint vector
        vector < Keypoint > Keypoint_q;

        keypt_t* query_info_t = query_info[0];
                        unsigned char* query_key_t = query_keys[0];

        for(int j=0;j< query_num_keys; j++)
        {

            Keypoint_q.push_back( Keypoint(query_info_t[j].x,
                        query_info_t[j].y,
                        query_info_t[j].scale,
                        query_info_t[j].orient,
                        (short int*) query_key_t + 128*j
                        ));

        }


    // Create the search tree of the query image
    //ANNkd_tree *tree = CreateSearchTree(num_keys[0],keys[0]);	

    // Create Result vector
    vector < pair < int, pair<int,int> > > result;

    // Match query image with the rest of the images
    for(int i=1;i<ret_cnt; i++)
    {

        cout << "COUNT " << i << endl;
        int index = findNextImage(bow_indices);
        if(index == -1)break;
        unsigned char **keys = new unsigned char*[1];
        int num_keys;
        keypt_t** info = new keypt_t*[1]; 

        num_keys = ReadKeyFile(key_files[indices[index+1]].c_str(), keys, info); // Read Key files

 //       cout << key_files[indices[index+1]].c_str() << endl;
   //     cout << num_keys << endl;
        keypt_t* info_t = info[0];
        unsigned char* key_t = keys[0];
        // create keypoint vector
        vector < Keypoint > Keypoint_r;

        for(int j=0;j< num_keys; j++)
        {
//            cout << info_t[j].x << " " << info_t[j].y << endl;
            Keypoint_r.push_back( Keypoint(info_t[j].x,
                        info_t[j].y,
                        info_t[j].scale,
                        info_t[j].orient,
                        (short int*) key_t + 128*j
                        ));

        }
    


        
        // create the tree
        ANNkd_tree *nn_tree = CreateSearchTree(num_keys,keys[0]);

        std::vector<KeypointMatch> matches =
           MatchKeys(query_num_keys, query_keys[0], nn_tree, ratio);

        
        bow_indices[index]=-1;

        if( (int)matches.size() >= 16)
        {


            // run ransac

            // normalize x,y locations according to image dimensions

            // TODO: normalize it later, right now run according un normalized constraints

            const vector<int>& inliers = EstimateFMatrix(Keypoint_q, Keypoint_r, matches, 2048, 9.0f, F, false);		

            // push into result vector
            result.push_back(make_pair(db_indices[index], make_pair(matches.size(), inliers.size())));


            if(inliers.size() >=12 && estimate_error ==1)
            {
                // estimate error
                computeError(id, db_indices[index], Keypoint_q, Keypoint_r, matches, inliers);

            }

            
            //            if(inliers.size() <12 )

            //          {

            double max_dist = 0;
            double max_bow = 0;
            for(int j=0; j<db_indices.size(); j++)
            {
                   double dist = poses->getSimilarityDist(db_indices[index], db_indices[j]);
                   dist_arr[j] = log(1+dist); 
                   if(dist > max_dist)max_dist = dist;

                   if(bow_indices[j]>max_bow)max_bow = bow_indices[j];
            }

//            cout << "MAX BOW " << max_bow << " MAX DIST " << max_dist << endl;

               
            double sum_dist = 0;
            for(int j=0; j< db_indices.size(); j++)
            {

                if(bow_indices[j] !=-1)
                {
                    double dist = dist_arr[j]/max_dist;
                    bow_indices[j] = bow_indices[j]/max_bow;

                    if(inliers.size() < 12)
                        bow_indices[j] = bow_indices[j]*1.0f/dist;
                    else
                        bow_indices[j] = bow_indices[j]*dist;
                }
            }

            bow_indices[index] = -1;

            // rerank the rest of the images
            //        }
            cout << "Found " << matches.size() << " " << inliers.size() << " inliers between 0 and " << index+1 << endl;

        }
        else
        {
            // bow score: bow_indices[i-1];
            // db_indices: db_indices[i-1];

            // if not matched rerank rest of the images
            result.push_back(make_pair(db_indices[index], make_pair(matches.size(),-1)));

            double max_dist = 0;
            double max_bow = 0;
            for(int j=0; j<db_indices.size(); j++)
            {
                double dist = poses->getSimilarityDist(db_indices[index], db_indices[j]);
                dist_arr[j] = log(1+dist);
                if(dist > max_dist)max_dist = dist;

                if(bow_indices[j]>max_bow)max_bow = bow_indices[j];
            }



//            cout << "MAX BOW " << max_bow << " MAX DIST " << max_dist << endl;
            // cout << endl << "----------------------------" << endl;
            for(int j=0; j< db_indices.size(); j++)
            {
                if(bow_indices[j] != -1)
                {
                double dist = dist_arr[j]/max_dist;

  //              cout << bow_indices[j] << " (" << dist << " " << dist_arr[j] << " " << max_dist << ") ";
                bow_indices[j] = bow_indices[j]/max_bow;

                    bow_indices[j] = bow_indices[j]*1.0f/dist;
                }

            }
            //cout << endl << "----------------------------" << endl;


            bow_indices[index] = -1;


        }


        annDeallocPts(nn_tree->pts);
        delete nn_tree;
        delete [] keys[0];
        delete [] info[0];
        delete [] keys;
        delete [] info;
    }


    delete [] query_keys[0];
    delete [] query_info[0];
        delete [] query_keys;
        delete [] query_info;

    //delete tree;
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


void readGT(char* list_in)
{
	ifstream infile;
	infile.open (list_in);
    double R[9], t[3], R_temp[9], t_final[3];
    double number;
	while(!infile.eof()) // To get you all the lines.
	{

        
        for(int i=0; i<9; i++)
            infile >> R[i];

         for(int i=0;i<3;i++)
         {
             infile >> t[i];
             t[i] = -1*t[i];
         }

        
        matrix_invert(3, R, R_temp);
        matrix_product(3,3,3,1,R_temp, t, t_final); 
    
        gtsam::Rot3 rot = gtsam::Rot3(R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7], R[8]);
        gtsam::Point3 point = gtsam::Point3(t_final[0], t_final[1], t_final[2]);    
        actual_poses.push_back( gtsam::Pose3(rot, point));

       
    }

    //return dimensions;
}


/* Read the dimension */

void readDim(char* list_in)
{

	ifstream infile;
	infile.open (list_in);
    int number1, number2;
	while(!infile.eof()) // To get you all the lines.
	{

           infile >> number1;
           infile >> number2;

           dimensions.push_back(make_pair(number1, number2));


    }

    //return dimensions;
}

/* Read the index list */

const vector< vector<int> > readIndex(char* list_in)
{

	vector < vector <int> > list_index;
    printf("%s\n",list_in);
	string str;
	ifstream infile;
	infile.open (list_in);
	while(!infile.eof()) // To get you all the lines.
	{
		vector < int > temp;
		vector < double > bow_temp;
		vector < int > db_temp;
		getline(infile,str); // Saves the line in STRING.
		std::stringstream ss;
		ss << str;

        int flag = 0;
		while(!ss.eof())
		{
			int number;
            double numb_t;
			ss >> number;

			temp.push_back(number);
            if(flag == 1)
            {

               ss >> number;
               db_temp.push_back(number);      

               ss >> numb_t;
               bow_temp.push_back(numb_t);      
             
            }

           flag = 1;
                
		}
		temp.erase(temp.end()-1);

        if(bow_temp.size()>=1)
		bow_temp.erase(bow_temp.end()-1);

        if(db_temp.size()>=1)
            db_temp.erase(db_temp.end()-1);
  //      list_index.erase(list_index.end() - 1);
    //    db_index.erase(db_index.end() - 1);
        
		list_index.push_back(temp);
		bow_index.push_back(bow_temp);
		db_index.push_back(db_temp);
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

    if(strcmp(dir, "../../data/Dubrovnik6K//")==0 || strcmp(dir,"../../data/Rome16K//")==0)
    {
        estimate_error=1;
        readDim(argv[7]);

        char bundle_file[256];
        sprintf(bundle_file,"%s/bundle/bundle.query.out",dir);
        readGT(bundle_file);
    }
	// Read the indices
	const vector< vector<int> >& index_file = readIndex(argv[2]);
    cout << "size of list_index " << db_index.size() << endl;
    // Read the bundle file
    poses = new bundle(argv[4]);

	// Opens the file to write results
	ofstream f_out(argv[5]);
	
    int ret_cnt= strtol(argv[6],NULL,10); // retrieve argv[5] images

	// Match Keys
	for(int i=0;i<index_file.size()-1; i++)
	{	
		for(int j=0;j<ret_cnt;j++)
			cout << index_file[i][j] << " ";
		cout << endl;
		const vector < pair <int, pair<int,int> > >& result = matchKeys(index_file[i], bow_index[i], db_index[i],  key_files,ret_cnt, i);

		for(int j=0;j<result.size(); j++)
			f_out << result[j].first << " " << result[j].second.first  << " " << result[j].second.second << " ";
		f_out << endl;
//        break;
	}
	f_out.close();

	

}
