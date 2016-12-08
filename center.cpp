#include "center.h"

void Center::dump(std::ostream &os){
	const Feature& f=this->getFeature();
	if(f.size()==(size_t)0){
		os<<std::endl;
	}
	Feature::iterator itr=f.begin();
	float sum=0.0;
	std::stringstream desc;
	for(;itr!=f.end();itr++){
		sum+=this->weights[*itr];
		desc<<*itr<<","<<this->weights[*itr]<<";";
	}
	os<<sum/f.size()<<"\t"<<desc.str()<<std::endl;
}

float Center::distanceComm(const Center *c){
			const Feature& f1=this->getFeature();
			const Feature& f2=c->getFeature();
			float cn=0.0;
			SET::iterator itr1=f1.begin();
			SET::iterator itr2=f2.begin();
			while(itr1!=f1.end() && itr2!=f2.end()){
				if(*itr1==*itr2){
					cn+=1;
					itr1++;itr2++;
				}else if(*itr1 < *itr2){
					itr1++;
				}else{
					itr2++;
				}
			}
			return cn;
}

float Center::distanceMax(const Center *c){
	const Feature& f1=this->getFeature();
	const Feature& f2=c->getFeature();
	float cn=0.0;
	SET::iterator itr1=f1.begin();
	SET::iterator itr2=f2.begin();
	while(itr1!=f1.end() && itr2!=f2.end()){
		if(*itr1==*itr2){
			cn+=1;
			itr1++;itr2++;
		}else if(*itr1 < *itr2){
			itr1++;
		}else{
			itr2++;
		}
	}
	return _max<float>(cn/f1.size(),cn/f2.size());
}

void Center::mergeWeights(Center *c){
	std::map<std::string, float>::iterator witr_c,find;
	witr_c=c->weights.begin();
	for(;witr_c!=c->weights.end();witr_c++){
		find=this->weights.find(witr_c->first);
		if(find==this->weights.end()){
			this->weights[witr_c->first]=witr_c->second;
		}else{
			this->weights[witr_c->first]=(find->second+witr_c->second)/2.0;
		}
	}
}

/*
void DualCenter::dump(std::ostream &os)const{
	Feature f=this->getFeature();
	Feature::iterator itr=f.begin();
	float sum=0.0;
	stringstream desc;
	for(;itr!=f.end();itr++){
		sum+=this->weights[*itr];
		desc<<*itr<<","<<this->weights[*itr]<<";";
	}
	os<<sum/f.size()<<"\t"<<desc.str()<<std::endl;
}
*/

void DualCenter::merge(Center *c){
	this->mergeWeights(c);
	const Name& n1=this->getName();
	const Feature& f1=this->getFeature();
	const Name& n2=c->getName();
	const Feature& f2=c->getFeature();

	this->setName(n1|n2);
	this->setFeature(f1&f2|n1|n2);
}

DualCenter::DualCenter(const std::string &nstr, const std::string &fstr, bool weighted){
	std::vector<std::string> ary;
	if(splitTxt(nstr,ary,";")>0){
		this->name=Name(ary.begin(),ary.end());
	}else{std::cerr<<"nodeStr format error"<<std::endl;}
	ary.clear();

	if(splitTxt(fstr,ary,";")>0){
		for(size_t i=0; i<ary.size(); i++){
			std::vector<std::string> arytok;
			if(2==splitTxt(ary[i],arytok,",")){
				this->feature.insert(arytok[0]);
				if(weighted==false)continue;
				if(this->weights.find(arytok[0])!=this->weights.end()){
					if(this->weights[arytok[0]]<atoi(arytok[1].c_str()))
						this->weights[arytok[0]]=atoi(arytok[1].c_str());
				}else{
						this->weights[arytok[0]]=atoi(arytok[1].c_str());
				}
			}
			arytok.clear();
		}
	}else{std::cerr<<"feature str format error"<<std::endl;}
	ary.clear();
}

SingleCenter::SingleCenter(const std::string &fstr, bool weighted){
	std::vector<std::string> ary;
	if(splitTxt(fstr,ary,";")>0){
		for(size_t i=0; i<ary.size(); i++){
			std::vector<std::string> arytok;
			if(2==splitTxt(ary[i],arytok,",")){
				this->feature.insert(arytok[0]);
				if(weighted==false)continue;
				if(this->weights.find(arytok[0])!=this->weights.end()){
					if(this->weights[arytok[0]]<atoi(arytok[1].c_str()))
						this->weights[arytok[0]]=atoi(arytok[1].c_str());
				}else{
						this->weights[arytok[0]]=atoi(arytok[1].c_str());
				}
			}
			arytok.clear();
		}
	}else{std::cerr<<"feature str format error"<<std::endl;}
	ary.clear();
}

void SingleCenter::merge(Center *c){
	this->mergeWeights(c);
	const Feature& f1=this->getFeature();
	const Feature& f2=c->getFeature();
	this->setFeature(f1|f2);
}
