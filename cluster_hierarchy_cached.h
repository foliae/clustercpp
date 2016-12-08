#ifndef _CLUSTER_HIERARCHY_CACHED_H_
#define _CLUSTER_HIERARCHY_CACHED_H_

#include "QA.h"
#include "center.h"

#include "assert.h"
#include <string>
#include <set>
#include <list>
#include <iterator>

enum Merge{Fast=0,Accu};//Merge type
enum Norm{MAX=0,COMM, Text};

class ClustersCached:public std::vector<Center*>{
public:
	ClustersCached():_k(0),_thresh(0.0),findMergePair(&ClustersCached::findMergePairFast),distance(&ClustersCached::distanceComm){
		this->distCache=NULL;
	}
	ClustersCached(float thresh, int merge_t=1, int norm_t=1, size_t k=0, const char* conf = "./qa.conf");
	~ClustersCached(){
		this->drop();
		QAFactory::destroy();
	}
	void drop(){
		for(size_t i=0;i<this->size();i++){
			delete (*this)[i];
		}
		this->clear();
		this->nnan=0;
		if(distCache){
			delete [] distCache;
			distCache=NULL;
		}
	}

	size_t	add(Center*);
	void	clustering();
	void	clusteringStep();

private:
	void	updateClusters(size_t &idx1, size_t &idx2);

	float	(ClustersCached::*distance)(const size_t &idx1, const size_t &idx2);
	float	distanceComm(const size_t &idx1, const size_t &idx2);
	float	distanceMax(const size_t &idx1, const size_t &idx2);
	float 	distanceText(const size_t &idx1, const size_t &idx2);

	float	(ClustersCached::*findMergePair)(size_t &idx1, size_t &idx2);
	float	findMergePairFast(size_t &idx1, size_t &idx2);
	float	findMergePairAccu(size_t &idx1, size_t &idx2);

	void initDist();

	//std::map<size_t,map<size_t,float> > Cost;
	float * distCache;
	size_t nnan;
	const size_t	_k;
	const float	_thresh;
	QA*	_pQA;

};

#endif
