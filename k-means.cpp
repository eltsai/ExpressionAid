#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include "k-means.h"
using namespace std;


KMeans::KMeans(int dimNum, int clusterNum)
{
	dimNum_= dimNum;
	clusterNum_ = clusterNum;

	means_ = new double*[clusterNum_];
	for(int i = 0; i < clusterNum_; i++)
	{
		means_[i] = new double[dimNum_];
		memset(means_[i], 0, sizeof(double) * dimNum_);
	}

	initMode_ = InitRandom;
	maxIterNum_ = 100;
	endError_ = 0.001;
}

KMeans::~KMeans()
{
	for(int i = 0; i < clusterNum_; i++)
	{
		delete[] means_[i];
	}
	delete[] means_;
}

void KMeans::Cluster(const char* sampleFileName, const char* labelFileName)
{
	// Check the sample file
	ifstream sampleFile(sampleFileName, ios_base::binary);
	assert(sampleFile);

	int size = 0;
	int dim = 0;
	sampleFile.read((char*)&size, sizeof(int));
	sampleFile.read((char*)&dim, sizeof(int));
	assert(size >= clusterNum_);
	assert(dim == dimNum_);

	// Initialize model

	Init(sampleFile);

	// Recursion
	double* x = new double[dimNum_];	// Sample data
	int label = -1;		// Class index
	double iterNum = 0;
	double lastCost = 0;
	double currCost = 0;
	int unchanged = 0;
	bool loop = true;
	int* counts = new int[clusterNum_];
	double** next_means = new double*[clusterNum_];	// New model for reestimation
	for(int i = 0; i < clusterNum_; i++)
	{
		next_means[i] = new double[dimNum_];
	}

	while(loop)
	{
	    //clean buffer for classification
		memset(counts, 0, sizeof(int) * clusterNum_);
		for(int i = 0; i < clusterNum_; i++)
		{
			memset(next_means[i], 0, sizeof(double) * dimNum_);
		}

		lastCost = currCost;
		currCost = 0;

		sampleFile.clear();
		sampleFile.seekg(sizeof(int) * 2, ios_base::beg);

		// Classification
		for(int i = 0; i < size; i++)
		{
			sampleFile.read((char*)x, sizeof(double) * dimNum_);
			currCost += GetLabel(x, &label);

			counts[label]++;
			for(int d = 0; d < dimNum_; d++)
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Re-estimation
		for(int i = 0; i < clusterNum_; i++)
		{
			if(counts[i] > 0)
			{
				for(int d = 0; d < dimNum_; d++)
				{
					next_means[i][d] /= counts[i];
				}
				memcpy(means_[i], next_means[i], sizeof(double) * dimNum_);
			}
		}

		// Terminal conditions
		iterNum++;
		if(fabs(lastCost - currCost) < endError_ * lastCost)
		{
			unchanged++;
		}
		if(iterNum >= maxIterNum_ || unchanged >= 3)
		{
			loop = false;
		}
		//DEBUG
		//cout << "Iter: " << iterNum << ", Average Cost: " << currCost << endl;
	}

	// Output the label file
	ofstream labelFile(labelFileName, ios_base::binary);
	assert(labelFile);

	labelFile.write((char*)&size, sizeof(int));
	sampleFile.clear();
	sampleFile.seekg(sizeof(int) * 2, ios_base::beg);

	for(int i = 0; i < size; i++)
	{
		sampleFile.read((char*)x, sizeof(double) * dimNum_);
		GetLabel(x, &label);
		labelFile.write((char*)&label, sizeof(int));
	}

	sampleFile.close();
	labelFile.close();

	delete[] counts;
	delete[] x;
	for(int i = 0; i < clusterNum_; i++)
	{
		delete[] next_means[i];
	}
	delete[] next_means;
}

//N
void KMeans::Cluster(double *data, int N, int *Label)
{
	int size = 0;
	size = N;

	assert(size >= clusterNum_);

	// Initialize model
	Init(data, N);

	// Recursion
	double* x = new double[dimNum_];	// x[n]
	int label = -1;		// Class index
	double iterNum = 0;
	double lastCost = 0;
	double currCost = 0;
	int unchanged = 0;
	bool loop = true;
	int* counts = new int[clusterNum_];
	double** next_means = new double*[clusterNum_];	// New model for reestimation
	for(int i = 0; i < clusterNum_; i++)
	{
		next_means[i] = new double[dimNum_];
	}

	while(loop)
	{
	    //clean buffer for classification
		memset(counts, 0, sizeof(int) * clusterNum_);
		for(int i = 0; i < clusterNum_; i++)
		{
			memset(next_means[i], 0, sizeof(double) * dimNum_);
		}

		lastCost = currCost;
		currCost = 0;

		// Classification
		for(int i = 0; i < size; i++)
		{
			for(int j = 0; j < dimNum_; j++)
				x[j] = data[i * dimNum_ + j];

			currCost += GetLabel(x, &label);

			counts[label]++;
			for(int d = 0; d < dimNum_; d++)
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Reestimation
		for(int i = 0; i < clusterNum_; i++)
		{
			if(counts[i] > 0)
			{
				for(int d = 0; d < dimNum_; d++)
				{
					next_means[i][d] /= counts[i];
				}
				memcpy(means_[i], next_means[i], sizeof(double) * dimNum_);
			}
		}

		// Terminal conditions
		iterNum++;

		if(fabs(lastCost - currCost) < endError_ * lastCost)
		{
			unchanged++;
		}
		if(iterNum >= maxIterNum_ || unchanged >= 3)
		{
			loop = false;
		}

		//DEBUG
		//cout << "Iter: " << iterNum << ", Average Cost: " << currCost << endl;
	}

	// Output the label file
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < dimNum_; j++)
            x[j] = data[i * dimNum_ + j];
		GetLabel(x,&label);
		Label[i] = label;
	}
	delete[] counts;
	delete[] x;
	for(int i = 0; i < clusterNum_ ; i++)
	{
		delete[] next_means[i];
	}
	delete[] next_means;
}

void KMeans::Init(double *data, int N)
{
	int size = N;

	if(initMode_ ==  InitRandom)
	{
		int inteval = size / clusterNum_;
		double* sample = new double[dimNum_];

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for(int i = 0; i < clusterNum_; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
			for(int j = 0; j < dimNum_; j++)
				sample[j] = data[select*dimNum_+j];
			memcpy(means_[i], sample, sizeof(double) * dimNum_);
		}

		delete[] sample;
	}
	else if(initMode_ == InitUniform)
	{
		double* sample = new double[dimNum_];

		for(int i = 0; i < clusterNum_; i++)
		{
			int select = i * size / clusterNum_;
			for(int j = 0; j < dimNum_; j++)
				sample[j] = data[select*dimNum_+j];
			memcpy(means_[i], sample, sizeof(double) * dimNum_);
		}

		delete[] sample;
	}
	else if(initMode_ == InitManual)
	{
		// Do nothing
	}
}

void KMeans::Init(ifstream& sampleFile)
{
	int size = 0;
	sampleFile.seekg(0, ios_base::beg);
	sampleFile.read((char*)&size, sizeof(int));

	if(initMode_ ==  InitRandom)
	{
		int inteval = size / clusterNum_;
		double* sample = new double[dimNum_];

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for(int i = 0; i < clusterNum_; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
			int offset = sizeof(int) * 2 + select * sizeof(double) * dimNum_;

			sampleFile.seekg(offset, ios_base::beg);
			sampleFile.read((char*)sample, sizeof(double) * dimNum_);
			memcpy(means_[i], sample, sizeof(double) * dimNum_);
		}

		delete[] sample;
	}
	else if(initMode_ == InitUniform)
	{
		double* sample = new double[dimNum_];

		for (int i = 0; i < clusterNum_; i++)
		{
			int select = i * size / clusterNum_;
			int offset = sizeof(int) * 2 + select * sizeof(double) * dimNum_;

			sampleFile.seekg(offset, ios_base::beg);
			sampleFile.read((char*)sample, sizeof(double) * dimNum_);
			memcpy(means_[i], sample, sizeof(double) * dimNum_);
		}

		delete[] sample;
	}
	else if(initMode_ == InitManual)
	{
		// Do nothing
	}
}

double KMeans::GetLabel(const double* sample, int* label)
{
	double dist = -1;
	for(int i = 0; i < clusterNum_; i++)
	{
		double temp = CalcDistance(sample, means_[i], dimNum_);
		if(temp < dist || dist == -1)
		{
			dist = temp;
			*label = i;
		}
	}
	return dist;
}

double KMeans::CalcDistance(const double* x, const double* u, int dimNum)
{
	double temp = 0;
	for(int d = 0; d < dimNum; d++)
	{
		temp += (x[d] - u[d]) * (x[d] - u[d]);
	}
	return sqrt(temp);
}

ostream& operator<<(ostream& out, KMeans& kmeans)
{
	out << "<KMeans>" << endl;
	out << "<DimNum> " << kmeans.dimNum_ << " </DimNum>" << endl;
	out << "<ClusterNum> " << kmeans.clusterNum_ << " </CluterNum>" << endl;

	out << "<Mean>" << endl;
	for(int i = 0; i < kmeans.clusterNum_; i++)
	{
		for(int d = 0; d < kmeans.dimNum_; d++)
		{
			out << kmeans.means_[i][d] << " ";
		}
		out << endl;
	}
	out << "</Mean>" << endl;

	out << "</KMeans>" << endl;
	return out;
}
