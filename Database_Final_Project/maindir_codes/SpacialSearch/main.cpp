//#include <stdlib.h>
#include <ctime>
#include <Windows.h>
#include "spacialIndex.h"
#include "json/json.h"
#include "fcgi/fcgi_stdio.h" 
#include <iostream>

#pragma comment(lib, "libfcgi.lib")
//#pragma comment(lib, "libjson.lib")

string urldecode(string str_source);

int main(void) 
{ 
	/* Initialization Code */ 
	SpacialIndex index;
	LARGE_INTEGER freq;

	index.createIndex("F:/spatial/zipcode-address.json");
	QueryPerformanceFrequency(&freq);
	/* Start of response loop */ 
	while(FCGI_Accept() >= 0) { 
	/* body of response loop */ ; 
		LARGE_INTEGER start ,end;
		QueryPerformanceCounter(&start);

		char *query_string = getenv("QUERY_STRING");//"Gil";
		string query = urldecode(query_string);
		int pos1 = query.find('=') + 1, pos2 = query.find('&', pos1),
			pos3 = query.find('=', pos2) + 1, pos4 = query.find('&', pos3),
			pos5 = query.find('=', pos4) + 1;
		string keyword = query.substr(pos1, pos2 - pos1);
		double lat = atof(query.substr(pos3, pos4 - pos3).c_str()),
				lng = atof(query.substr(pos5, query.size() - pos5).c_str());

		vector<Entity> result;
		int count = index.search(keyword, lat, lng, result);

		QueryPerformanceCounter(&end);
		double seconds = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
		seconds = (int)(1E6 * seconds + 0.5) / 1E6;
		Json::Value value(Json::arrayValue);
		for (int i = 0; i < result.size(); ++i) {
			Json::Value vi;
			Entity e = result[i];
			vi["_id"] = Json::Value(e.id);
			vi["date"] = Json::Value(e.date);
			vi["addr"] = Json::Value(e.addr);
			vi["lat"] = Json::Value(e.lat);
			vi["lng"] = Json::Value(e.lng);
			vi["name"] = Json::Value(e.name);
			vi["pcode"] = Json::Value(e.pcode);
			vi["url"] = Json::Value(e.url);
			value.append(vi);
		}
		Json::Value response;
		response["time"] = Json::Value(seconds);
		response["count"] = Json::Value(count);
		response["content"] = value;
		Json::FastWriter writer;
		string str = writer.write(response);
//		cout<< str.c_str()<< endl;
		printf("Content-type: text/plain\r\n\r\n%s", str.c_str());
	} 
	return 0;
/* End of response loop */ 
}

static int php_htoi(char *s)
{
	int value;
	int c;

	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

	return (value);
}

string urldecode(string str_source)
{
	char const *in_str = str_source.c_str();
	int in_str_len = strlen(in_str);
	int out_str_len = 0;
	string out_str;
	char *str;

	str = _strdup(in_str);
	char *dest = str;
	char *data = str;

	while (in_str_len--) {
		if (*data == '+') {
			*dest = ' ';
		}
		else if (*data == '%' && in_str_len >= 2 && isxdigit((int) *(data + 1)) 
			&& isxdigit((int) *(data + 2))) {
				*dest = (char) php_htoi(data + 1);
				data += 2;
				in_str_len -= 2;
		} else {
			*dest = *data;
		}
		data++;
		dest++;
	}
	*dest = '\0';
	out_str_len =  dest - str;
	out_str = str;
	free(str);
    return out_str;
}
/* }}} */
