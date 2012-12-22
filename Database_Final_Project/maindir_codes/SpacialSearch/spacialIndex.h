#ifndef _SPACIAL_INDEX_
#define _SPACIAL_INDEX_
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include "json\json.h"
//#include <stdlib.h>
using namespace std;

const int MAX_RESULTS_NUM = 5;

struct Entity 
{
public:
	int id;
	string addr;
	__int64 date;
	double lat;
	double lng;
	string name;
	int pcode;
	string url;
	Entity(const int &i, const string &a, const __int64 &d, const double &la, const double &ln, const string &n, const int &p, const string &u): id(i), addr(a),	date(d), lat(la), lng(ln), name(n),	pcode(p), url(u) { };
	Entity() {}
	//Entity ent(0, addr, 0, lat, lng, name, 0, url)
};

class SpacialIndex
{
public:
	void createIndex(const string &fileName);
	int search(string query, double lat, double lng, vector<Entity> &result);
//private:
	multimap <string, int> indexTree;
	vector <Entity> data;
};

#endif