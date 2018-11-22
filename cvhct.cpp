#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <stdlib.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;
using namespace cv;

typedef struct _VALUE_WITH_ID{
	float v;
	int id;
} VALUE_WITH_ID;


int operator < (VALUE_WITH_ID &m1, VALUE_WITH_ID m2)
{
	return m1.v < m2.v;
}

int* randperm(int n) {
	
	srand(0);
	int *perm = new int[n];
	VALUE_WITH_ID *v = new VALUE_WITH_ID[n];
	for (int i = 0; i < n; i++){
		v[i].id = i;
		v[i].v = rand();
	}
	sort(v, v + n);
	for (int i = 0; i < n; i++){
		perm[i] = v[i].id;
	}
	delete[] v;
	return perm;
}

int main() {
	
	cout << "start" << endl;
	int data_len = 1000000;
	int query_len = 10000;
	int total_len = data_len + query_len;
	
	
	// read data
	cout << "read data ... " << endl;
	ifstream iptf("TENMILLIONdata.txt");
	int *data = new int[total_len * 64]();
	int *ori_data = new int[total_len * 64]();
	string sss;
	int aaa;
	for (int i = 0; i < total_len * 64; i++) {
		getline(iptf, sss);
		aaa = atoi(sss.c_str());
		ori_data[i] = aaa;
	}
	
	// random select query
	int* perm = randperm(total_len);
	for (int i = 0; i < total_len; i++){
		for (int j = 0; j < 64; j++){
			data[i * 64 + j] = ori_data[perm[i] * 64 + j];
		}
	}
	
	//make data
	cout << "make Mat data ... " << endl;
	Mat data_mat(data_len, 64, CV_8U);
	Mat query_mat(query_len, 64, CV_8U);
	
	for (int i = 0; i < data_len; i++) {
		for (int j = 0; j < 64; j++) {
			data_mat.at<uchar>(i,j) = data[i * 64 + j];
		}
	}
	
	for (int i = 0; i < query_len; i++) {
		for (int j = 0; j < 64; j++) {
			query_mat.at<uchar>(i,j) = data[i * 64 + data_len * 64 + j];
		}
	}
	
	// linear search
	cout << "do linear search ... " << endl;
	int *ground_truth = new int[query_len * 2]();
	
	/*
	//get linear results
	ifstream mgt("SCOREBASE_MILLION_GROUND.txt");
	int msi = 0;
	string mst;
	for (int i = 0; i < query_len * 2; i++) {
		getline(mgt, mst);
		msi = atoi(mst.c_str());
		ground_truth[i] = msi;
	}
	*/
	
	Mat linear_indices, linear_dists;
	flann::Index flann_linear_index(data_mat, flann::LinearIndexParams(), cvflann::FLANN_DIST_HAMMING);
	clock_t linear_start = clock();
	flann_linear_index.knnSearch(query_mat, linear_indices, linear_dists, 2, flann::SearchParams(1000, 0, true));
	double linear_time = (double) (clock() - linear_start) / CLOCKS_PER_SEC;
	for (int i = 0; i < query_len; i++) {
		ground_truth[i * 2] = (int)linear_dists.at<int>(i,0);
		ground_truth[i * 2 + 1] = (int)linear_dists.at<int>(i,1);
	}
	cout << linear_time << endl;
	
	//write to txt file
	ofstream mgt("SCOREBASE_MILLION_GROUND.txt");
	for (int i = 0; i < query_len * 2; i++) {
		mgt << ground_truth[i] << endl;
	}
	
	//do match
	cout << "do flann match" << endl;
	Mat indices, dists;
	flann::Index flann_index(data_mat, flann::HierarchicalClusteringIndexParams(16, cvflann::FLANN_CENTERS_RANDOM, 1, 300), cvflann::FLANN_DIST_HAMMING);
	
	ofstream rp("SEARCH_RESULT.txt");
	int linear_job[] = {1000,2000,3000,4000,5000,6000,7000,8000,9000,10000,15000,20000,25000,30000};
	int n_jobs = 14;
	for (int iter = 0; iter < n_jobs; iter++) {
		clock_t flann_start = clock();
		flann_index.knnSearch(query_mat, indices, dists, 2, flann::SearchParams(linear_job[iter], 0, true));
		double flann_time = (double) (clock() - flann_start) / CLOCKS_PER_SEC;
		cout << flann_time << "  1nn score  ";
	
		int score_a = 0;
		int score_b = 0;
		for (int i = 0; i < query_len; i++) {
			if ((int)dists.at<int>(i,0) == ground_truth[i * 2]) {
				score_a++;
				if ((int)dists.at<int>(i,1) == ground_truth[i * 2 + 1]) {
					score_b++;
				}
			}
		}
		cout << score_a << "  2nn score  ";
		cout << score_b << endl;
		rp << linear_job[iter] << "," << score_a / double(query_len) << "," << score_b / double(query_len) << "," << flann_time << "," << linear_time << endl;
	}
	return 0;
}
