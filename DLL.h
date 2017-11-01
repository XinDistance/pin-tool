#include "Common.h"
class GLOBALINFO
{
public:
	IMG execimg;
	map<ADDRINT, string> funcs;
	vector<IMG> dlls;
	map<ADDRINT, string> disassemble;
};