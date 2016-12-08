#include "cluster_hierarchy_cached.h"

#define CACHE_ELEM(idx1,idx2) distCache[ (idx1)*this->size() \
								-((idx1)*(idx1)+3*(idx1))/2	\
								+(idx2)-1 ]

ClustersCached::ClustersCached(float thresh, int merge_t, int norm_t,size_t k, const char* conf):_k(k),_thresh(thresh)
{
	switch(merge_t){
		case Fast:
			findMergePair=&ClustersCached::findMergePairFast;
			break;
		case Accu:
			findMergePair=&ClustersCached::findMergePairAccu;
			break;
		default:
			findMergePair=&ClustersCached::findMergePairAccu;
	}

	switch(norm_t){
		case MAX:
			distance=&ClustersCached::distanceMax;
			break;
		case COMM:
			distance=&ClustersCached::distanceComm;
			break;
		case Text:
			_pQA = QAFactory::getInstance(conf);
			distance=&ClustersCached::distanceText;
			break;
		default:
			distance=&ClustersCached::distanceComm;
	}

	distCache=NULL;

}

size_t ClustersCached::add(Center *c){
	const Feature& f=c->getFeature();
	if(f.size()>=this->_thresh){
		this->push_back(c);
		return 1;
	}
	return 0;
}

float ClustersCached::findMergePairAccu(size_t &idx1, size_t &idx2)
{
#ifdef _DEBUG_
	std::cout<<"begin to find merge pair in current list...."<<std::endl;
#endif
	float nb=0.0;
	size_t i,j;
	for(size_t i=0;i<this->size();i++)
	{
		if((*this)[i]==NULL)continue;
/*
#ifdef _DEBUG_
		std::cout<<"cluster size=="<<this->size()<<std::endl;
#endif
*/
		float nb_t;
		j=i;
		for(j++;j<this->size();j++)
		{
			if((*this)[j]==NULL)continue;
			nb_t=(this->*distance)(i,j);

/*
#ifdef _DEBUG_
			std::cout<<"\tdistance(i, j)="<<nb_t<<std::endl;
#endif
*/
			//nb_t=Cost[i][j];
			if(nb_t>nb){
				nb=nb_t;
				idx1=i;
				idx2=j;
			}
		}
	}
	return nb;
}

float ClustersCached::findMergePairFast(size_t &idx1, size_t &idx2)
{
	float nb=0.0;
	size_t i,j;
	for(i=0;i<this->size();i++)
	{
		if((*this)[i]==NULL)continue;
		float nb_t;
		j=i;
		for(j++;j<this->size();j++)
		{
			if((*this)[j]==NULL)continue;
			nb_t=(this->*distance)(i,j);
			//nb_t=Cost[i][j];
			if(nb_t>this->_thresh){
				nb=nb_t;
				idx1=i;
				idx2=j;
				return nb;
			}else if(nb_t>nb){
				nb=nb_t;
				idx1=i;
				idx2=j;
			}
		}
	}
	return nb;
}

float ClustersCached::distanceComm(const size_t &idx1, const size_t &idx2){
	if(CACHE_ELEM(idx1,idx2)==-1){
		float v = (*this)[idx1]->distanceComm((*this)[idx2]);
		CACHE_ELEM(idx1,idx2)=v;
		return v;
	}else{
		return CACHE_ELEM(idx1,idx2);
	}
}

float ClustersCached::distanceMax(const size_t &idx1, const size_t &idx2){
	if(CACHE_ELEM(idx1,idx2)==-1){
		float v = (*this)[idx1]->distanceMax((*this)[idx2]);
		CACHE_ELEM(idx1,idx2)=v;
		return v;
	}else{
		return CACHE_ELEM(idx1,idx2);
	}
}

float cosSimilarity(Center* c1, Center* c2, QA* pQA){
	const Feature& f1=c1->getFeature();
	const Feature& f2=c2->getFeature();
	float cn=0.0;
	SET::iterator itr1=f1.begin();
	SET::iterator itr2=f2.begin();
	for(; itr1!=f1.end(); itr1++ )
		for(; itr2!=f2.end(); itr2++ ){
			float simi = pQA->match((*itr1).c_str(), (*itr2).c_str());
			if (simi > cn){
				cn = simi;
			}
		}
	return cn;

}

float ClustersCached::distanceText(const size_t &idx1, const size_t &idx2){
	if(CACHE_ELEM(idx1,idx2)==-1 && CACHE_ELEM(idx2,idx1)==-1 ){
		Center* c1 = (*this)[idx1];
		Center* c2 = (*this)[idx2];
		float v = cosSimilarity(c1, c2, _pQA);
printf("*******Similarity == idx1 = %d, idx2 = %d , %f \n", idx1, idx2, v);
		CACHE_ELEM(idx1,idx2)=v;
		return v;
	}else{
		return ( CACHE_ELEM(idx1,idx2)==-1 ) ? CACHE_ELEM(idx2,idx1) : CACHE_ELEM(idx1,idx2);
	}
}

void ClustersCached::updateClusters(size_t &idx1,size_t &idx2)
{
	(*this)[idx1]->merge((*this)[idx2]);
	//this->erase(itr_c1);
	delete (*this)[idx2];
	(*this)[idx2]=NULL;
	nnan++;
	//updateCache
	for(size_t i=idx1+1; i<this->size(); i++){
			CACHE_ELEM(idx1,i)=-1;
	}
	for(size_t i=idx2+1; i<this->size(); i++){
			CACHE_ELEM(idx2,i)=-1;
	}
	for(size_t i=0; i<idx1; i++){
			CACHE_ELEM(i,idx1)=-1;
	}
	for(size_t i=0; i<idx2; i++){
			CACHE_ELEM(i,idx2)=-1;
	}

}

void ClustersCached::clustering()
{
	initDist();
#ifdef _DEBUG_
	std::cout<<"begin clustering...."<<std::endl;
#endif
	if(_k==0 && _thresh==0.0)
		return;
	//initCost();
	this->nnan=0;
	while( this->size() > this->nnan )
	{
		size_t idx1,idx2;
		float diff=(this->*findMergePair)(idx1,idx2);
#ifdef _DEBUG_
		std::cout<<"thresh:"<<_thresh<<"\tK value:"<<_k<<std::endl;
		std::cout<<"merge w:"<<diff<<"\tc size"<<this->size()<<std::endl;
#endif
		if(diff<_thresh||_k>this->size())
			break;
		updateClusters(idx1, idx2);
	}
	if(distCache){
		delete [] distCache;
		distCache=NULL;
	}
}

void ClustersCached::clusteringStep()
{
	initDist();
#ifdef _DEBUG_
	std::cout<<"begin clustering...."<<std::endl;
#endif
	if(_k==0 && _thresh==0.0)
		return;
	this->nnan=0;
	//initCost();
	if( this->size() > this->nnan )
	{
		size_t idx1,idx2;
		float diff=(this->*findMergePair)(idx1,idx2);
#ifdef _DEBUG_
		std::cout<<"thresh:"<<_thresh<<"\tK value:"<<_k<<std::endl;
		std::cout<<"merge w:"<<diff<<"\tc size"<<this->size()<<std::endl;
#endif
		if(diff<_thresh||_k>this->size())
			return;
		updateClusters(idx1, idx2);
	}
	if(distCache){
		delete [] distCache;
		distCache=NULL;
	}
}

void ClustersCached::initDist(){
	size_t csize = this->size()*(this->size()-1)/2;
	distCache = new float[csize];
	assert(distCache);
	for(size_t i=0; i<csize; i++){
		distCache[i]=-1;
	}
}
