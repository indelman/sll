#include <iostream>
#include "matrix.h"
#include "stdio.h"
#include "compute_scores.h"
#include "math.h"
#include <vector>

inline double SIGN(double x) {return (x >= 0.0f) ? +1.0f : -1.0f;}
inline double NORM(double a, double b, double c, double d) {return sqrt(a * a + b * b + c * c + d * d);}
double Am[] = { 1.544, 12.5418, 1.4969, 12.1058 };

using namespace std;

bundle::bundle(char* filename)
{
	ReadBundle(filename);


	//	strcpy(fname, filename);

}

void Rot2Quat (double* R, double* q)
{

	double r11 = R[0], r12 = R[1], r13 = R[2],
	       r21 = R[3], r22 = R[4], r23 = R[5],
	       r31 = R[6], r32 = R[7], r33  =R[8];;
	q[0] = ( r11 + r22 + r33 + 1.0f) / 4.0f;
	q[1] = ( r11 - r22 - r33 + 1.0f) / 4.0f;
	q[2] = (-r11 + r22 - r33 + 1.0f) / 4.0f;
	q[3] = (-r11 - r22 + r33 + 1.0f) / 4.0f;
	if(q[0] < 0.0f) q[0] = 0.0f;
	if(q[1] < 0.0f) q[1] = 0.0f;
	if(q[2] < 0.0f) q[2] = 0.0f;
	if(q[3] < 0.0f) q[3] = 0.0f;
	q[0] = sqrt(q[0]);
	q[1] = sqrt(q[1]);
	q[2] = sqrt(q[2]);
	q[3] = sqrt(q[3]);
	if(q[0] >= q[1] && q[0] >= q[2] && q[0] >= q[3]) {
		q[0] *= +1.0f;
		q[1] *= SIGN(r32 - r23);
		q[2] *= SIGN(r13 - r31);
		q[3] *= SIGN(r21 - r12);
	} else if(q[1] >= q[0] && q[1] >= q[2] && q[1] >= q[3]) {
		q[0] *= SIGN(r32 - r23);
		q[1] *= +1.0f;
		q[2] *= SIGN(r21 + r12);
		q[3] *= SIGN(r13 + r31);
	} else if(q[2] >= q[0] && q[2] >= q[1] && q[2] >= q[3]) {
		q[0] *= SIGN(r13 - r31);
		q[1] *= SIGN(r21 + r12);
		q[2] *= +1.0f;
		q[3] *= SIGN(r32 + r23);
	} else if(q[3] >= q[0] && q[3] >= q[1] && q[3] >= q[2]) {
		q[0] *= SIGN(r21 - r12);
		q[1] *= SIGN(r31 + r13);
		q[2] *= SIGN(r32 + r23);
		q[3] *= +1.0f;
	} else {
		printf("coding error\n");
	}
	double r = NORM(q[0], q[1], q[2], q[3]);
	q[0] /= r;
	q[1] /= r;
	q[2] /= r;
	q[3] /= r;

}

void computeViewingDirection(double q0, double qx, double qy,
		double qz, std::vector<double>& dir) {
	double tmp_dir_x = 2.0 * (q0 * qy + qx * qz);
	double tmp_dir_y = 2.0 * (qy * qz - q0 * qx);
	double tmp_dir_z = 1.0 - 2.0 * (qx * qx + qy * qy);
	double norm =
		1.0
		/ (sqrt(
					tmp_dir_x * tmp_dir_x + tmp_dir_y * tmp_dir_y
					+ tmp_dir_z * tmp_dir_z));
	double dir_x = norm * tmp_dir_x;
	double dir_y = norm * tmp_dir_y;
	double dir_z = norm * tmp_dir_z;
	dir.push_back(dir_x);
	dir.push_back(dir_y);
	dir.push_back(dir_z);
}


double findAngleProd(std::vector<double> query_dir,
		std::vector<double>& db_dir) {
	double angle = query_dir[0] * db_dir[0] + query_dir[1] * db_dir[1]
		+ query_dir[2] * db_dir[2];
	return angle;
}

double bundle::getSimilarityDist(int a, int b) {

	/// Computes the distance between the two cameras
	double* t1 = camset[a].t;
	double* t2 = camset[b].t;
	double trans_dist = sqrt((t1[1]-t2[1])*(t1[1]-t2[1]) + (t1[2]-t2[2])*(t1[2]-t2[2]) + (t1[3]-t2[3])*(t1[3]-t2[3]));

	/// Compute the dot product between the two viewing directions
	double* R1 = camset[a].R;
	double* R2 = camset[b].R;

	// Convert rotation matrix to quaternion
	double Q1[4], Q2[4];
	Rot2Quat(R1,Q1);
	Rot2Quat(R2,Q2);

	std::vector<double> dir1;
	std::vector<double> dir2;
	computeViewingDirection(Q1[0],Q1[1],Q1[2],Q1[3],dir1);
	computeViewingDirection(Q2[0],Q2[1],Q2[2],Q2[3],dir2);
	double angle_dist = 1 - findAngleProd(dir1, dir2);
	double val1 = Am[0] * trans_dist + Am[1] * angle_dist;
	double val2 = Am[2] * trans_dist + Am[3] * angle_dist;
	double value = sqrt(val1 * val1 + val2 * val2);
	double dist = exp(-value);
	return dist;

}

void bundle::compute_scores_(int* docs, float* scores, float* wt_scores)
{

float sum=0;
for(int i=0; i<10; i++)
{
	
	sum=0;
	for(int j=0;j<10;j++)
	{
	       
		sum = sum + getSimilarityDist(docs[i],docs[j])*1.0f/scores[j];
	}		
	
	wt_scores[i] = scores[i]*1.0f/sum;

}


}
void bundle::ReadBundle(char* filename)
{
	double version;

	printf("Reading Bundle File %s\n",filename);

	printf("[ReadBundleFile] Reading file...\n");

	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		printf("Error opening file %s for reading\n", filename);
		return;
	}


	char first_line[256];
	fgets(first_line, 256, f);
	if (first_line[0] == '#') {
		sscanf(first_line, "# Bundle file v%lf", &version);

		printf("[ReadBundleFile] Bundle version: %0.3f\n", version);

		fscanf(f, "%d %d\n", &num_images, &num_points);
	} else if (first_line[0] == 'v') {
		sscanf(first_line, "v%lf", &version);
		printf("[ReadBundleFile] Bundle version: %0.3f\n", version);

		fscanf(f, "%d %d\n", &num_images, &num_points);
	} else {
		version = 0.1;
		sscanf(first_line, "%d %d\n", &num_images, &num_points);
	}

	printf("[SifterApp::ReadBundleFile] Reading %d images and %d points...\n",
			num_images, num_points);

	camset = new struct camstr[num_images];

	for (int i = 0; i < num_images; i++) {
		//      printf("Camera Number %d\n",i);
		double focal_length;
		double R[9];
		double t[3];
		double k[2] = { 0.0, 0.0 };

		if (version >= 0.4) {
			char name[512];
			int w, h;
			fscanf(f, "%s %d %d\n", name, &w, &h);
		}

		/* Focal length */
		if (version > 0.1) {
			fscanf(f, "%lf %lf %lf\n", &focal_length, k+0, k+1);
		} else {
			fscanf(f, "%lf\n", &focal_length);
		}

		/* Rotation */
		fscanf(f, "%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
				&camset[i].R[0], &camset[i].R[1], &camset[i].R[2], &camset[i].R[3], &camset[i].R[4], &camset[i].R[5], &camset[i].R[6], &camset[i].R[7], &camset[i].R[8]);
		/* Translation */

		// printf("camset: %lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
		//   camset[i].R[0], camset[i].R[1], camset[i].R[2], camset[i].R[3], camset[i].R[4], camset[i].R[5], camset[i].R[6], camset[i].R[7], camset[i].R[8]);
		//      double t[3];
		fscanf(f, "%lf %lf %lf\n", &t[0],&t[1],&t[2]);
		t[0] = -1*t[0];
		t[1] = -1*t[1];
		t[2] = -1*t[2];


		double R1[9];
		matrix_invert(3,camset[i].R,R1);
		matrix_product(3,3,3,1,R1,t,camset[i].t);
		//      printf("%lf %lf %lf\n",camset[i].t[0],camset[i].t[1],camset[i].t[2]);

	}



#if 0
	vertexset = new struct vertex[num_points+3];
	for (int i = 0; i < num_points; i++)
	{

		// printf("Point Number %d\n",i);
		/* Position */
		fscanf(f, "%f %f %f\n",
				&vertexset[i].m_pos[0], &vertexset[i].m_pos[1], &vertexset[i].m_pos[2]);

		// if (m_bundle_version < 0.3)
		//     vertexset.m_pos[2] = -vertexset.m_pos12];

		/* Color */
		fscanf(f, "%f %f %f\n",
				&vertexset[i].m_color[0], &vertexset[i].m_color[1], &vertexset[i].m_color[2]);

		vertexset[i].m_color[0]/=255.0;
		vertexset[i].m_color[1]/=255.0;
		vertexset[i].m_color[2]/=255.0;

		//      printf("%f %f %f\n",vertexset[i].m_color[0],vertexset[i].m_color[1],vertexset[i].m_color[2]);
		int num_visible;
		fscanf(f, "%d", &num_visible);
		vertexset[i].m_num_vis=num_visible;

#if 0
		if(num_visible>200)
		{
			printf("num_visible %d\n",num_visible);
			vertexset[i].m_color[0]=1;
			vertexset[i].m_color[1]=1;
			vertexset[i].m_color[2]=1;

		}

#endif
		//      if (num_visible >=3)
		//              num_min_views_points++;

		// vertexset.m_views.resize(num_visible);
		//  printf("num_visible for 3d point %d = %d\n",i,num_visible);

#if 0
		vertexset[i].sift = new int[128];
		int* meanSift = vertexset[i].sift;

		for(int k=0;k<128;k++)
		{
			meanSift[k]=0;
		}

#endif

		vector < int > temp_v;
		for (int j = 0; j < num_visible; j++) {
			int view, key;
			fscanf(f, "%d %d", &view, &key);

			temp_v.push_back(view);

#if 0
			unsigned char* sift = imageset[view].sift + 128*key;


			for(int k=0;k<128;k++)
			{
				meanSift[k] += (int)sift[k];

				//          printf("%hhu ",sift[k]);

			}

			//  printf("\n");



			//                  printf("%d %d\n",view,key);




			if (!m_image_data[view].m_camera.m_adjusted) {
				// printf("[SifterApp::ReadBundleFile] "
				//        "Removing view %d from point %d\n", view, i);
			} else {
				/* Check cheirality */
				bool val = (m_bundle_version >= 0.3);

				double proj_test[2];
				if (m_image_data[view].m_camera.
						Project(pt.m_pos, proj_test) == val) {

					pt.m_views.push_back(ImageKey(view, key));
				} else {
					printf("[SifterApp::ReadBundleFile] "
							"Removing view %d from point %d [cheirality]\n",
							view, i);
					// pt.m_views.push_back(ImageKey(view, key));
				}
			}
			// pt.m_views.push_back(ImageKey(view, key));

#endif

			double x, y;
			fscanf(f, "%lf %lf", &x, &y);
		}
		point_vec.push_back(temp_v);


		//  printf("Meansift\n");
#if 0
		for(int k=0;k<128;k++)
		{
			meanSift[k] =       (meanSift[k]*1.0)/(num_visible*1.0);    

			//                  printf("%d ",meanSift[k]);

		}

#endif
		//vertexset[i].sift = meanSift;
		//  printf("\n");                    
		// #define CROP_POINT_CLOUD
	}
	printf("End Point Iteration\n");

	//  printf("Debug\n");

#if 0
#if VERBOSE == 1
	for(int i=0;i<num_points;i++)
	{


		for(int j=0;j<128;j++)
			printf("%d ",vertexset[i].sift[j]);

		printf("\n");
	}

#endif

#if DUMP_MODEL_SIFT == 1


	FILE* out = fopen("modelsift.txt","w");

	fprintf(out,"%d\n",num_points);

	for(int i=0;i<num_points;i++)
	{


		for(int j=0;j<128;j++)
			fprintf(out,"%d ",vertexset[i].sift[j]);

		fprintf(out,"\n");
	}
	fclose(out);

#endif

#endif

#if 0    
	/* Read outliers */
	int num_outliers;
	fscanf(f, "%d", &num_outliers);

	for (int i = 0; i < num_outliers; i++) {
		ImageKey ik;
		fscanf(f, "%d %d", &(ik.first), &(ik.second));
		m_outliers.push_back(ik);
	}
#endif

#endif

	fclose(f);




}


