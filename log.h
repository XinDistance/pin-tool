#include "Common.h"
typedef enum
{
	TRACELOG,
	FUNCTIONLOG,
	SYMBOLLOG,
	THREADLOG
}LOG_TYPE;
struct TRACEINFO
{
	THREADID id;
	ADDRINT  addr;
	const char    *disassemble;
};
struct FUNCTIONINFO
{
	ADDRINT addr;
	ADDRINT target;
	const char  *imagename;
	const char  *functionname;
};
struct SYMBOLINFO
{
	ADDRINT base;
	const char  *imagename;
	const char  *functionname;
};
struct THREADINFO
{
	THREADID id;
};
struct LOG_PACKET
{
	LOG_TYPE type;
	union
	{
		TRACEINFO traceinfo;
		FUNCTIONINFO functioninfo;
		SYMBOLINFO symbolinfo;
		THREADINFO threadinfo;
	};
};
class LOGCENTER
{
public:
	LOGCENTER()
	{

	}
	bool logto(const LOG_PACKET &packet)
	{
		bool ret;
		switch (packet.type)
		{
		case TRACELOG:
			ret = TraceLog(packet.traceinfo);
			break;
		case FUNCTIONLOG:
			ret = FunctionLog(packet.functioninfo);
			break;
		case SYMBOLLOG:
			ret = SymbolLog(packet.symbolinfo);
			break;
		case THREADLOG:
			ret = ThreadLog(packet.threadinfo);
			break;
		default:
			ret = false;
			break;
		}
		return ret;
	}
	void endlog()
	{
		if (functionfile != NULL)
			fclose(functionfile);
		if (symbolfile != NULL)
			fclose(symbolfile);
		if (threadfile != NULL)
			fclose(threadfile);
		for (vector<FILE*>::iterator it = tracefiles.begin(); it != tracefiles.end(); it++)
		{
			fclose(*it);
		}
	}
private:
	vector<FILE*> tracefiles;
	FILE *functionfile;
	FILE *symbolfile;
	FILE *threadfile;
	string GenerateFileNameFromID(THREADID id)
	{
		char temp[20];
		_itoa(id, temp, 10);
		strcat(temp, "thread.log");
		return "log\\" + string(temp);
	}
	bool TraceLog(const TRACEINFO &info)
	{
		if (tracefiles.size() <= (int)info.id)
		{
			tracefiles.push_back(fopen(GenerateFileNameFromID(info.id).c_str(), "w"));
		}
		FILE *file = tracefiles[info.id];
		if (info.disassemble!=NULL)
			return fprintf(file, "%p:%s\n", info.addr, info.disassemble);
		else
			return fprintf(file, "%p\n", info.addr);
	}
	bool FunctionLog(const FUNCTIONINFO &info)
	{
		
		if (functionfile == NULL)
		{
			functionfile = fopen("log\\function.log", "w");
		}
		if (info.functionname == NULL)
			return fprintf(functionfile, "%p:%p\n", info.addr, info.target);
		return fprintf(functionfile, "%p:%p %s\n", info.addr, info.target, info.functionname);
	}
	bool SymbolLog(const SYMBOLINFO &info)
	{
		if (info.functionname == NULL )
			return false;
		if (symbolfile == NULL)
		{
			symbolfile = fopen("log\\symbol.log", "w");
		}
		return fprintf(symbolfile, "%p:%s!%s\n", info.base, info.imagename, info.functionname);
	}
	bool ThreadLog(const THREADINFO &info)
	{
		if (threadfile == NULL)
		{
			threadfile = fopen("log\\thread.log", "w");
		}
		return fprintf(threadfile, "%d start\n", info.id);
	}
};