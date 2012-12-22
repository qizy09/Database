/* 
 * The AEE_Indexer class handles the creation, deletion, and loading of an index. All 
 * necessary initialization of the indexer component should take place within the 
 * constructor for the AEE_Indexer class. 
 * */

#pragma once
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <queue>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <climits>
using namespace std;

template<typename T1, typename T2, typename T3, typename T4>
struct quadruple{
  T1 id;
  T2 pos;
  T3 len;
  T4 sim;
};

template<typename T1, typename T2, typename T3, typename T4>
bool operator< (const quadruple<T1, T2, T3, T4>& q1, const quadruple<T1, T2, T3, T4>& q2)
{
	if (q1.id < q2.id)
		return true;
	else if (q1.id > q2.id)
		return false;
	else if (q1.pos < q2.pos)
		return true;
	else if (q1.pos > q2.pos)
		return false;
	else if (q1.len < q2.len)
		return true;
	else
		return false;
}

struct substr {
	int index;
	int pos;
	int len;
	substr(int i, int p, int l):index(i), pos(p), len(l) {}
};

const int SUCCESS = 1;
const int FAILURE = 0; 

class AEE_Indexer {
public:
	AEE_Indexer   () {
		for (int i = 0; i < 101; ++i) {
			cal_buf[i][0] = i;
			cal_buf[0][i] = i;
		}
		max_strlen = 0;
		min_strlen = INT_MAX;
	};
	~AEE_Indexer  () {};

	bool CreateIndex(const char * fileName, unsigned q); // create an index  

	bool AppEntityExtractED(const char *doc, unsigned threshold, vector< quadruple<unsigned, unsigned, unsigned, unsigned> > &results);

	bool AppEntityExtractJaccard(const char *doc, double threshold, vector<quadruple<unsigned, unsigned, unsigned, double> > &results);

private:
	void get_terms(const string& str, unordered_multiset<string>& terms);
	void get_candidate(const string& doc, int min_len, int maxlen, int tau, vector<substr>& terms);
	int maxi(int i, int j);
	int calculateED(const string& query, const string& target, const unsigned& threshold, unsigned& ed);
	unsigned get_min(const unsigned& a, const unsigned& b, const unsigned& c);
	void get_candidate_jac(const string& doc, int min_len, int max_len, double threshold, vector<substr>& terms);
	double calculate_jaccard(const unordered_multiset<string>& qterms, 
						 const unordered_multiset<string>& vterms);
private:
  char * dataFilename;
  char * indexFilename;
  int max_strlen;
  int min_strlen;
  vector<string> data;
  unordered_map<string, vector<int> > inverted_list;
  unsigned cal_buf[101][101];
  unsigned q_value;
}; 
