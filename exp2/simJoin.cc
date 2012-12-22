#include "simJoin.h"

bool simJoin::readFile(const char* fileName, vector<string> &data)
{
	if (data.size() > 0)
		data.clear();
	ifstream fin(fileName);
	if (!fin)
		return false;
	string line;
	while (::getline(fin, line)) {
		data.push_back(line);
	}
	return true;
}

bool simJoin::check_data(const char * fileName1, const char * fileName2)
{
	if (file1 == NULL || strcmp(file1, fileName1) != 0) {
		if (file1 == NULL)
			file1 = new char[256];
		strcpy(file1, fileName1);
		if (!readFile(fileName1, data1))
			return FAILURE;
		
	}
	if (file2 == NULL || strcmp(file2, fileName2)) {
		if (file2 == NULL)
			file2 = new char[256];
		strcpy(file2, fileName2);
		if (!readFile(fileName2, data2))
			return FAILURE;
	}
	return SUCCESS;
}

int simJoin::calculateED(const string& query, const string& target, const unsigned& threshold, unsigned& ed)
{
	unsigned m = query.size(), n = target.size();
	for (unsigned i = 1; i <= m; ++i)
		for (unsigned j = 1; j <= n; ++j) {
			cal_buf[i][j] = get_min(cal_buf[i - 1][j] + 1, cal_buf[i][j - 1] + 1, 
					            cal_buf[i - 1][j - 1] + (query[i - 1] == target[j - 1] ? 0 : 1));
		}
	if ((ed = cal_buf[m][n]) > threshold)
		return -1;
	return 0;
}

unsigned simJoin::get_min(const unsigned& a, const unsigned& b, const unsigned& c)
{
	return a > b ? (b > c ? c : b) : (a > c ? c : a);
}

 /*
 * It should do a similarity join operation betweent the set of strings from the two data files
 * such that the edit distance between two string is not larger than the given threshold. The 
 * format of result is a triple of number which respectively stand for the ID of string from the 
 * first data file, the ID of string from the second data file and the edit distance between the
 * two strings and All results are stored in a triple, sorted based on the IDs of the string from
 * the first file and then the IDs of the string from the second file in an ascending order. Return 
 * an error if the similarity join operation is failed.
 */


bool simJoin::SimilarityJoinED(const char * file1, const char * file2, unsigned q, unsigned threshold, vector< triple<unsigned, unsigned, unsigned> > &results) {
	if (check_data(file1, file2) == FAILURE)
		return FAILURE;
	triple<unsigned, unsigned, unsigned> result_item;
	for (vector<string>::const_iterator it_first = data1.begin(); it_first != data1.end(); ++it_first)
		for (vector<string>::const_iterator it_second = data2.begin(); it_second != data2.end(); ++it_second) {
			if (abs((int)it_first->size() - (int)it_second->size()) > threshold)
				continue;
			unsigned ed;
			if (calculateED(*it_first, *it_second, threshold, ed) == 0) {
				result_item.id1 = it_first - data1.begin();
				result_item.id2 = it_second - data2.begin();
				result_item.sim = ed;
				results.push_back(result_item);
			}
			
		}
	return SUCCESS;
}

void simJoin::get_tokens(const string& str, unordered_multiset<string>& tokens, unsigned q_value)
{
	istringstream sin(str);
	string token;
	while (sin >> token)
		tokens.insert(token);
}

double simJoin::calculate_jaccard(const unordered_multiset<string>& qtokens, 
						 const unordered_multiset<string>& vtokens)
{
	unsigned q_size = qtokens.size(), v_size = vtokens.size();
	const unordered_multiset<string>* smaller, *bigger;
	unsigned intersection = 0;
	if (q_size < v_size)
		smaller = &qtokens, bigger = &vtokens;
	else
		smaller = &vtokens, bigger = &qtokens;
	unordered_multiset<string>::const_iterator it = smaller->begin();
	while(it != smaller->end()) {
		const string& str = *it;
		unsigned count_bigger = bigger->count(str);
		unsigned count_smaller = 1;
		while (++it != smaller->end() && *it == str)
			++count_smaller;
		intersection += (count_bigger > count_smaller ? count_smaller : count_bigger);
	}
	return 1.0 * intersection / (q_size + v_size - intersection);
}

/*
 * It should do a similarity join operation betweent the set of strings from the two data files
 * such that the jaccard similarity between two string is not smaller than the given threshold. The 
 * format of result is a triple of number which respectively stand for the ID of string from the 
 * first data file, the ID of string from the second data file and the jaccard similarity between 
 * the two strings and All results are stored in a triple, sorted based on the IDs of the string from
 * the first file and then the IDs of the string from the second file in an ascending order. Return 
 * an error if the similarity join operation is failed.
 */

bool simJoin::SimilarityJoinJaccard(const char * file1, const char * file2, unsigned q, double threshold, vector< triple<unsigned, unsigned, double> > &results) {
	if (check_data(file1, file2) == FAILURE)
		return FAILURE;
	triple<unsigned, unsigned, double> result_item;
	for (vector<string>::const_iterator it_first = data1.begin(); it_first != data1.end(); ++it_first) {
		unordered_multiset<string> qtokens;
		get_tokens(*it_first, qtokens, q);
		unsigned qtokens_num = qtokens.size();
		for (vector<string>::const_iterator it_second = data2.begin(); it_second != data2.end(); ++it_second) {
			const string& data_str = *it_second;
			double jaccard;
			unordered_multiset<string> vtokens;
			get_tokens(data_str, vtokens, q);
			unsigned vtokens_num = vtokens.size();
			if (qtokens_num == 0 || vtokens_num == 0)
				continue;
			double div = qtokens_num > vtokens_num ? 1.0 * vtokens_num / qtokens_num : 1.0 * qtokens_num / vtokens_num;
			if (div < threshold) {
				continue;
			}
			jaccard = calculate_jaccard(qtokens, vtokens);
			if (jaccard >= threshold) {
				result_item.id1 = it_first - data1.begin();
				result_item.id2 = it_second - data2.begin();
				result_item.sim = jaccard;
				results.push_back(result_item);
			}
		}
	}
	return SUCCESS;
}
