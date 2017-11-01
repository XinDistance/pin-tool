#include "Common.h"
using namespace std;

class Config
{
public:
	Config(string filename = "config.txt")
	{
		file = fopen(filename.c_str(), "r");
		this->filename = filename;

		while (!feof(file))
		{
			char temp[500];
			fuckfgets(temp, file);
			int len = strlen(temp);
			char *split = strchr(temp, '=');
			if (split == 0 || split == temp + len - 1)
				continue;
			string key = string(temp, split);
			string value = string(split + 1, temp + len);
			configs[key] = value;
		}
		ParseParameter();
	}
	bool getAttribute(const string &key, string &value)
	{
		if (configs.find(key) != configs.end())
		{
			value = configs[key];
			return true;
		}
		else
		{
			return false;
		}
	}
	ADDRINT GetStartTrace()
	{
		return starttrace;
	}
	ADDRINT GetEndTrace()
	{
		return endtrace;
	}
	ADDRINT GetTraceHead()
	{
		return tracehead;
	}
	ADDRINT GetTraceEnd()
	{
		return traceend;
	}
	BOOL islogdetail()
	{
		return logdetail;
	}
	void SetStartTrace(ADDRINT s)
	{
		if (starttrace == 0)
			starttrace = s;
	}
	void SetTraceHead(ADDRINT s)
	{
		if (tracehead == 0)
			tracehead = s;
	}
	void SetTraceEnd(ADDRINT s)
	{
		if (traceend == 0)
			traceend = s;
	}
	void end()
	{
		fclose(file);
	}
private:
	string filename;
	FILE *file;
	map<string, string> configs;

	ADDRINT starttrace;
	ADDRINT endtrace;
	ADDRINT hookpoint;
	string  textsegname;
	ADDRINT tracehead;
	ADDRINT traceend;
	INT    logdetail = 1;
	void initAttr()
	{

	}
	INT32 HexStr2Int(const char *s)
	{
		INT32 ret;
		sscanf(s, "%x", &ret);
		return ret;
	}
	void Int2HexStr(INT32 it, char *s)
	{
		sprintf(s, "%x", it);
	}
	void ParseParameter()
	{
		char temp[20];
		if (configs.find("starttrace") != configs.end())
		{
			starttrace = HexStr2Int(configs["starttrace"].c_str());
		}
		if (configs.find("endstrace") != configs.end())
		{
			endtrace = HexStr2Int(configs["endstrace"].c_str());
		}
		if (configs.find("hookpoint") != configs.end())
		{
			hookpoint = HexStr2Int(configs["hookpoint"].c_str());
		}
		if (configs.find("tracehead") != configs.end())
		{
			tracehead = HexStr2Int(configs["tracehead"].c_str());
		}
		if (configs.find("traceend") != configs.end())
		{
			traceend = HexStr2Int(configs["traceend"].c_str());
		}
		if (configs.find("logdetail") != configs.end())
		{
			logdetail = HexStr2Int(configs["logdetail"].c_str());
		}
	}
	void fuckfgets(char *buf, FILE* file)
	{
		int index = 0;
		while (!feof(file))
		{
			char c;
			fread(&c, 1, 1, file);
			if (c == '\n')
				break;
			else
				buf[index++] = c;
		}
		buf[index] = 0;
	}
};