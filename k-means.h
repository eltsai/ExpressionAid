#pragma once
#include <fstream>
#include <assert.h>

class KMeans
{
public:
	enum InitMode
	{
		InitRandom,
		InitManual,
		InitUniform,
	};

	KMeans(int dimNum = 1, int clusterNum = 1);
	//preferrably les than 20,000 * 68

	~KMeans();

	void SetMean(int i, const double* u)
	{ 
		memcpy(means_[i], u, sizeof(double) * dimNum_);
	}

	void SetInitMode(int i)
	{ 
		initMode_ = i; 
	}

	void SetMaxIterNum(int i)			
	{ 
		maxIterNum_ = i;
	}

	void SetEndError(double f)			
	{ 
		endError_ = f;
	}

	double* GetMean(int i)	
	{ 
		return means_[i];
	}

	int GetInitMode()		
	{ 
		return initMode_;
	}

	int GetMaxIterNum()	
	{
		return maxIterNum_;
	}
	double GetEndError()	
	{ 
		return endError_;
	}


	//	SampleFile: <size><dim><data>...
	//	LabelFile:	<size><label>...

	void Cluster(const char* sampleFileName, const char* labelFileName);
	void Init(std::ifstream& sampleFile);
	void Init(double *data, int N);
	void Cluster(double *data, int N, int *Label);
	friend std::ostream& operator<<(std::ostream& out, KMeans& kmeans);

private:
	int dimNum_;
	int clusterNum_;
	double** means_;

	int initMode_;
	int maxIterNum_;		// The stopping criterion regarding the number of iterations
	double endError_;		// The stopping criterion regarding the error

	double GetLabel(const double* x, int* label);
	double CalcDistance(const double* x, const double* u, int dimNum);
};
