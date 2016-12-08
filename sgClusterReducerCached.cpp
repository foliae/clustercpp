#include "utility.h"
#include "TextCenter.h"
#include "cluster_hierarchy_cached.h"

#include "stdlib.h"

#include <iostream>
#include <fstream>
using namespace std;


string mode="";
string conf="";
int norm_t;
int merge_t;
float Thresh=0.0;

static const Option long_options[] = {
	      {"thresh",'t', "0.9", "float",
			       "threshold for clustering: default=4" },
	      {"merge", 'm', "1", "INT",
				       "method to find merge pair : Accu=1/Fast=0" },
	      {"norm", 'n', "2", "INT",
				       "method to find distance measure between two centers : Text=2/COMM=1/MAX=0" },
	      {"help", 'h', 0, 0,       
					"show this help and exit" },
	      {0, 0, 0, 0, 0}
};

Param param;

size_t addToCluster(ClustersCached *,const string &, const string &);
void (*process)(ClustersCached *, string &);
void calClusters(ClustersCached *, string &);
void calClustersStep(ClustersCached *, string &);

int main(int argc, char ** argv)
{
	if(!param.open(argc,argv,long_options)||param.get<bool>("help")){
		std::cout<<param.help();
		param.dump_config(&cout);
		return 0;
	}
	Thresh=param.get<float>("thresh");
	norm_t=param.get<int>("norm");
	merge_t=param.get<int>("merge");

	const vector<string> &rest=param.rest_args();
	if(rest.size()>=2){
		cerr<<"param error"<<endl;
		return -1;
	}else if(rest.size()==1){
		conf = rest[0];
	}else
		conf = "./qa.conf";

	mode = "All";
	if(mode=="Step"){
		process=calClustersStep;
	}else if(mode=="All"){
		process=calClusters;
	}else{
		cerr<<"param error"<<endl;
		return -1;
	}
	//param.dump_config(&std::cout);

	ClustersCached *cs = new ClustersCached(Thresh,merge_t,norm_t, 0, conf.c_str());

	string line, cate = "", isub="", content="";
	while(getline(std::cin,line)){
		std::vector<string> ary;
		size_t ntok = splitTxt(line,ary);
		if(ntok!=3)continue;

		if(cate.compare("")==0){
			cate=ary[0];
			isub=ary[1];
			content=ary[2];
			addToCluster(cs,isub,content);
		}else if(cate.compare(ary[0])==0){
			addToCluster(cs,ary[1],ary[2]);
		}else{
			process(cs,cate);
			cate=ary[0];
			isub=ary[1];
			content=ary[2];
			addToCluster(cs,isub,content);
		}
	}

	process(cs,cate);
	if(cs){
		delete cs;
		cs=NULL;
	}
	return 0;
}


size_t addToCluster(ClustersCached * cluster,const string & nodeStr, const string & descStr){
	//std::cout<<"new center\t"<<nodeStr.c_str()<<"\t\t"<<descStr.c_str()<<endl;
	Center *dc= new TextCenter(nodeStr,descStr,true);
	//std::cout<<"add this center\t"<<endl;
	if(!cluster->add(dc)){
		//std::cout<<"add failed\t"<<endl;
		delete dc;
	}
}

void calClusters(ClustersCached * cluster, string & cate){
	//cluster->setSepIndex(sepIndex);
	cluster->clustering();
	ClustersCached::iterator itrCluster=cluster->begin();
	std::set<std::string>::iterator itrName;
	size_t schIndex=0;
	for(; itrCluster!=cluster->end(); itrCluster++){
		//if(itrCluster->feature.size()<MaxLen)continue;
		if(!(*itrCluster))continue;
		std::cout<<cate.c_str()<<"-"<<schIndex++<<"\t";
		(*itrCluster)->dump(cout);
	}
	cluster->drop();
}

void calClustersStep(ClustersCached * cluster, string & cate){
	//cluster->setSepIndex(sepIndex);
	cluster->clusteringStep();
	ClustersCached::iterator itrCluster=cluster->begin();
	std::set<std::string>::iterator itrItem;
	for(; itrCluster!=cluster->end(); itrCluster++){
		//if(itrCluster->feature.size()<MaxLen)continue;
		if(!(*itrCluster))continue;
		std::cout<<cate.c_str()<<"\t1\t";
		std::set<std::string> name=(*itrCluster)->getName();
		for(itrItem=name.begin(); itrItem!=name.end(); itrItem++){
			std::cout<<itrItem->c_str()<<";";
		}
		std::cout<<"\t";
		std::set<std::string> feature=(*itrCluster)->getFeature();
		for(itrItem=feature.begin(); itrItem!=feature.end(); itrItem++){
			//std::cout<<itrItem->c_str()<<","<<(*itrCluster)->weights[*itrItem]<<";";
			std::cout<<itrItem->c_str()<<";";
		}
		std::cout<<endl;
	}
	//std::cout<<"drop...."<<endl;
	cluster->drop();
	//std::cout<<"cluster size=="<<cluster->size()<<endl;
}
