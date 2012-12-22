#include "AEE_indexer.h"

void AEE_Indexer::get_terms(const string& str, unordered_multiset<string>& terms)
{
	if (str.size() <= q_value)
		terms.insert(str);
	else {
		for (unsigned i = 0, end = str.size() - q_value + 1; i < end; ++i)
			terms.insert(str.substr(i, q_value));
	}
}

int AEE_Indexer::maxi(int i, int j)
{
	return i > j ? i : j;
}

void AEE_Indexer::get_candidate(const string& doc, int min_len, int max_len, int tau, vector<substr>& terms)
{
	for (int index = 0; index < data.size(); ++index) {
//		cout << data[index] << endl;
//		int buf[2][10000];
		int count[10000];
//		memset(buf, 0, sizeof(buf));
		memset(count, 0, sizeof(count));
		for (int i = 0; i <= doc.size() - q_value; ++i) {
			const string& s = doc.substr(i, q_value);
			unordered_map<string, vector<int> >::const_iterator it = inverted_list.find(s);
			if (it != inverted_list.end()) {
				const vector<int>& vec = it->second;
				pair<vector<int>::const_iterator, vector<int>::const_iterator> bounds = equal_range(vec.begin(), vec.end(), index);
				count[i] = bounds.second - bounds.first;
			} else
				count[i] = 0;
		}
//		cout << min_len << " " << max_len << endl;
		for (int i = min_len; i <= max_len; ++i) {
			int ori = 0;
			for (int j = 0; j <= i - q_value; ++j)
				ori += count[j];
			int pos = 0, occ = maxi(data[index].size(), i) - q_value + 1 - tau * q_value;
//			cout << min_len << " " << max_len << " " << i << endl;
			while (pos <= (int)doc.size() - i) {
				if (ori >= occ)
					terms.push_back(substr(index, pos, i));
//				cout << ori << " " << pos << endl;
				ori += count[pos + i - q_value + 1] - count[pos];
				++pos;
			}
		}
	}
}

void AEE_Indexer::get_candidate_jac(const string& doc, int min_len, int max_len, double threshold, vector<substr>& terms)
{
	for (int index = 0; index < data.size(); ++index) {
//		int buf[2][10000];
		int count[10000];
//		memset(buf, 0, sizeof(buf));
		memset(count, 0, sizeof(count));
		for (int i = 0; i <= doc.size() - q_value; ++i) {
			const string& s = doc.substr(i, q_value);
			unordered_map<string, vector<int> >::const_iterator it = inverted_list.find(s);
			if (it != inverted_list.end()) {
				const vector<int>& vec = it->second;
				pair<vector<int>::const_iterator, vector<int>::const_iterator> bounds = equal_range(vec.begin(), vec.end(), index);
				count[i] = bounds.second - bounds.first;
			} else
				count[i] = 0;
		}
		for (int i = min_len; i <= max_len; ++i) {
			int ori = 0;
			for (int j = 0; j <= i - q_value; ++j)
				ori += count[j];
			int pos = 0;
			double occ = (data[index].size() + i - 2 * q_value + 2) * threshold / (1 + threshold);
			while (pos <= (int)doc.size() - i) {
				if (ori >= occ) {
					terms.push_back(substr(index, pos, i));
				}
				ori += count[pos + i - q_value + 1] - count[pos];
				++pos;
			}
		}
	}
}

/*
 * This method creates an index on the strings stored in the given data file name. 
 * The format of the file is as follows: 
 * each line represents a string. 
 * The ID of each string is its line number, starting from 0. 
 * The index is created in memory. 
 */
bool AEE_Indexer::CreateIndex(const char * fileName, unsigned q) {
	q_value = q;

	ifstream fin(fileName);
	if (!fin)
		return FAILURE;
	string line;
	while (getline(fin, line)) {
		data.push_back(line);
		if (line.size() > max_strlen)
			max_strlen = line.size();
		if (line.size() < min_strlen)
			min_strlen = line.size();
		unsigned term_count = line.size() <= q_value ? 1 : line.size() - q_value + 1;
		unordered_multiset<string> terms;
		get_terms(line, terms);
		for (unordered_multiset<string>::const_iterator it = terms.begin();
			 it != terms.end(); ++it) {
				 inverted_list[*it].push_back(data.size() - 1);
		}
	}
	fin.close();

	return SUCCESS;
}

unsigned AEE_Indexer::get_min(const unsigned& a, const unsigned& b, const unsigned& c)
{
	return a > b ? (b > c ? c : b) : (a > c ? c : a);
}

int AEE_Indexer::calculateED(const string& query, const string& target, const unsigned& threshold, unsigned& ed)
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

double AEE_Indexer::calculate_jaccard(const unordered_multiset<string>& qterms, 
						 const unordered_multiset<string>& vterms)
{
	unsigned q_size = qterms.size(), v_size = vterms.size();
	const unordered_multiset<string>* smaller, *bigger;
	unsigned intersection = 0;
	if (q_size < v_size)
		smaller = &qterms, bigger = &vterms;
	else
		smaller = &vterms, bigger = &qterms;
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
 * It should do an approximate entity extract using the index by finding all the substrings in the document
 * file whose edit distance to the entities is within the threshold. The 
 * format of result is a quadruple of integers which respectively stand for the 
 * qualified string ID and the edit distance between the qualified string and 
 * query string. All results are stored in a vector, sorted based on the 
 * qualified string IDs in an ascending order. Return an error if the index is 
 * not constructed or not loaded in memory.
 */

bool AEE_Indexer::AppEntityExtractED(const char *doc, unsigned threshold, vector< quadruple<unsigned, unsigned, unsigned, unsigned> > &results) {
	vector<substr> candidate;
	string docs = string(doc);
	get_candidate(docs, min_strlen - threshold, max_strlen + threshold, threshold, candidate);
	quadruple<unsigned, unsigned, unsigned, unsigned> item;
	for (int i = 0; i < candidate.size(); ++i) {
		unsigned ed;
		const substr& s = candidate[i];
		if (calculateED(data[s.index], docs.substr(s.pos, s.len), threshold, ed) == 0) {
			item.id = s.index;
			item.len = s.len;
			item.pos = s.pos;
			item.sim = ed;
			results.push_back(item);
		}
	}
	::sort(results.begin(), results.end());
	return SUCCESS;
}


/*
 * It should do a search using the index by finding all the strings in the data
 * file whose jaccard similarity to the query string is not smaller than the threshold. The 
 * format of result is a pair of number which respectively stand for the 
 * qualified string ID and the jaccard similarity between the qualified string and 
 * query string. All results are stored in a vector, sorted based on the 
 * qualified string IDs in an ascending order. Return an error if the index is 
 * not constructed or not loaded in memory.
 */

bool AEE_Indexer::AppEntityExtractJaccard(const char *doc, double threshold, vector<quadruple<unsigned, unsigned, unsigned, double> > &results) {
	vector<substr> candidate;
	string docs = string(doc);
	get_candidate_jac(docs, static_cast<int>(min_strlen * threshold + 0.99), max_strlen / threshold, threshold, candidate);
	quadruple<unsigned, unsigned, unsigned, double> item;
	for (int i = 0; i < candidate.size(); ++i) {
		double jac;
		const substr& s = candidate[i];
		
		const string str = docs.substr(s.pos, s.len);
		unordered_multiset<string> terms1, terms2;
		get_terms(str, terms1);
		get_terms(data[s.index], terms2);
		jac = calculate_jaccard(terms1, terms2);
		if (jac >= threshold) {
			item.id = s.index;
			item.len = s.len;
			item.pos = s.pos;
			item.sim = jac;
			results.push_back(item);
		}
	}
	::sort(results.begin(), results.end());
	return SUCCESS;
}
