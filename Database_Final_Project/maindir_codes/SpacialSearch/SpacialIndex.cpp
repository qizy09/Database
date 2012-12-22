#include "spacialIndex.h"

void SpacialIndex::createIndex(const string &fileName)
{
	char line[1024]; int i = 0;
	ifstream inf(fileName);
	Json::Reader reader;
	Json::Value jsonObj;
	
	data.clear();
	inf.getline(line, 800);
	while (strlen(line) > 1)
	{

		if (reader.parse(line, jsonObj))
		{
			Entity ent(i++, (jsonObj["addr"]).asString(), 0, (jsonObj["latlng"][0]).asDouble(), (jsonObj["latlng"][1]).asDouble(), (jsonObj["name"]).asString(), (jsonObj["pcode"]).asInt(), (jsonObj["url"]).asString());
			data.push_back(ent);
			indexTree.insert(make_pair(ent.name, ent.id));
		}
		inf.getline(line, 800);
	}
//	printf("fuck\n");
	inf.close();
}
int SpacialIndex::search(string query, double lat, double lng, vector<Entity> &result)
{
	//result.clear();
	multimap<std::string, int>::iterator top[5], it, beg = indexTree.lower_bound(query);
	(query.at(query.length() - 1))++;
	multimap<std::string, int>::iterator end = indexTree.upper_bound(query);
	int cnt = 0, i; double dis[5], dist;
	//string query1 = query;
	//(query1.at(query.length() - 1))++;
	for (it = beg; it != end; it++)
	{
			double l1 = data[it->second].lat, l2 = data[it->second].lng;
			dist = (lat - l1) * (lat - l1) + (lng - l2) * (lng - l2);
			if (cnt >= 5)
			{
				i = 5;
				while (i > 0 && dist < dis[i - 1])
					if (i < 5)
					{
						dis[i] = dis[i - 1], top[i] = top[i - 1];
						i--;
					}
					else i --;
				if (i < 5)
				{
					dis[i] = dist;
					top[i] = it;
				}
			} else
			{
				i = cnt++;
				while (i > 0 && dist < dis[i - 1])
					dis[i] = dis[i - 1], top[i] = top[i - 1], i--;
				dis[i] = dist;
				top[i] = it;
			}
	}
	for (i = 0; i < cnt; i++)
	{
		result.push_back(data[top[i]->second]);
	}
	return cnt;
}
//multimap<std::string, int> indexTree
//vector<Entity> data;
