#ifndef _CENTER_H_
#define _CENTER_H_

#include "utility.h"
#include "set_optr.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>

//#define min(a, b) ((a) <= (b) ? (a) : (b))
//#define max(a, b) ((a) >= (b) ? (a) : (b))

typedef SET Name,Feature;

class Center{
	public:
		virtual ~Center(){weights.clear();}
		virtual void dump(std::ostream &os);//const=0;
		virtual float distanceComm(const Center *c); //calculate distance between two centers using common friends
		virtual float distanceMax(const Center *c); //calculate distance between two centers using max portion of common friends
		virtual void mergeWeights(Center *c);

		virtual void merge(Center *c){} //merge two centers
		virtual const Feature& getFeature()const=0;
		virtual const Name& getName()const=0;

		virtual void setFeature(const Feature& f)=0;
		virtual void setName(const Name& n)=0;

		std::map<std::string, float> weights;
};

class DualCenter:public Center{
public:
	void merge(Center *c); //merge two centers

	const Feature& getFeature()const{return this->feature;}
	const Name& getName()const{return this->name;}

	void setFeature(const Feature& f){
		this->feature.clear();
		this->feature=f;
	}
	void setName(const Name& n){
		this->name.clear();
		this->name=n;
	}

	Name name;
	Feature feature;
	//std::map<std::string, float> weights;

	DualCenter(){}
	DualCenter(const Name& n,const Feature& f){
		name=n;
		feature=f;
	}
	DualCenter(const std::string &nstr, const std::string &fstr, bool weighted=true);
	~DualCenter(){
		name.clear();
		feature.clear();
	}
}; 

class SingleCenter:public Center{
public:
	void merge(Center *c);

	const Feature& getFeature()const{return this->feature;}
	void setFeature(const Feature& f){
		this->feature.clear();
		this->feature=f;
	}

	Feature feature;
	//std::map<std::string, float> weights;

	SingleCenter(){}
	SingleCenter(const Feature& f){
		feature=f;
	}
	SingleCenter(const std::string &f, bool weighted=true);
	~SingleCenter(){
		feature.clear();
	}
private:
	const Name& getName()const{}
	void setName(const Name& n){}
};

#endif
