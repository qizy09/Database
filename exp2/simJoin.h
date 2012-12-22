/* 
 * The simJoin class handles the creation, deletion, and loading of an index. 
 * */

#pragma once
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <unordered_set>
#include <sstream>
using namespace std;

template<typename T1, typename T2, typename T3>
struct triple{
  T1 id1;
  T2 id2;
  T3 sim;
};
const int SUCCESS = 1;
const int FAILURE = 0; 

class simJoin {
public:
	simJoin   (): file1(NULL), file2(NULL) {
		for (int i = 0; i < 101; ++i) {
			cal_buf[i][0] = i;
			cal_buf[0][i] = i;
		}
	}
	~simJoin  () { 
		if (file1 == NULL) 
			delete []file1;
		if (file2 == NULL) 
			delete []file2;
	}

  bool SimilarityJoinED(const char * file1, const char * file2, unsigned q, unsigned threshold, vector< triple<unsigned, unsigned, unsigned> > &results);
  
  bool SimilarityJoinJaccard(const char * file1, const char * file2, unsigned q, double threshold, vector< triple<unsigned, unsigned, double> > &results);

private:
  bool readFile(const char *fileName, vector<string> &data);
  int  calculateED(const string& query, const string& target, const unsigned& threshold, unsigned& ed);
  inline unsigned get_min(const unsigned& a, const unsigned& b, const unsigned& c);
  bool check_data(const char * fileName1, const char * fileName1);
  void simJoin::get_tokens(const string& str, unordered_multiset<string>& terms, unsigned q);
  double simJoin::calculate_jaccard(const unordered_multiset<string>& qterms, const unordered_multiset<string>& vterms);

private:
  char * file1;
  char * file2;
  unsigned cal_buf[101][101];
  vector<string> data1;
  vector<string> data2;
}; 
