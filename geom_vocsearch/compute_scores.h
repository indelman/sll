#ifndef COMPUTE_SCORES
#define COMPUTE_SCORES

struct camstr
{

        double t[3];
        double R[9];
//      double *

};


class bundle{

	public:

	bundle(char *);
	~bundle(void){};

	void ReadBundle(char* );	
	double getSimilarityDist(int a, int b);	
	void compute_scores_(int*, float*, float*);
//	struct camstr* GetCamset()	

	private:

	int num_images, num_points;	
	char fname[256];
	struct camstr* camset;
	


};







#endif
