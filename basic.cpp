
/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs
 *  and could serve as the starting point for developing your first PIN tool
 */

#include "log.h"
#include "config.h"
#include "DLL.h"

//----------------------------------------
//----------------------------------------

Config config;
LOGCENTER logcenter;
GLOBALINFO    globalinfos;



//----------------------------------------
//----------------------------------------
bool AddrInRange(ADDRINT addr)
{
	static bool startExecute = false;
	if (addr == config.GetStartTrace())
		startExecute = true;
	if (!startExecute)
		return false;
	if (addr >= config.GetTraceHead()  && addr <= config.GetTraceEnd())
	{
		return true;
	}
	for (IMG &img : globalinfos.dlls)
	{
		if (addr >= IMG_LowAddress(img) && addr <= IMG_LowAddress(img)+IMG_SizeMapped(img))
		{
			return false;
		}		
	}
	return true;
}
//----------------------------------------
//----------------------------------------

VOID CallLog(ADDRINT ip, ADDRINT callAddr)
{

	LOG_PACKET packet;
	FUNCTIONINFO info;
	info.addr = ip;
	info.target = callAddr;
	map<ADDRINT, string>::iterator it = globalinfos.funcs.find((ADDRINT)callAddr);
	if (it != globalinfos.funcs.end())
	{
		string &funcname = it->second;
		info.functionname = funcname.c_str();
	}
	else
	{
		info.functionname = NULL;
	}
	info.imagename = NULL;
	packet.type = FUNCTIONLOG;
	packet.functioninfo = info;
	logcenter.logto(packet);
}
VOID TraceLog(THREADID id, ADDRINT ip)
{
	LOG_PACKET packet;
	TRACEINFO info;
	info.id = id;
	info.addr = ip;
	if (config.islogdetail())
		info.disassemble = globalinfos.disassemble[ip].c_str();
	else
		info.disassemble = NULL;
	packet.type = TRACELOG;
	packet.traceinfo = info;
	logcenter.logto(packet);
	
}
VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{

}
//---------------------------------------------------
//---------------------------------------------------
VOID INSStart(INS ins, VOID*)
{
	if (!AddrInRange(INS_Address(ins)))
		return;
	globalinfos.disassemble[INS_Address(ins)] = INS_Disassemble(ins);
	if (INS_IsCall(ins))
	{
		INS_InsertCall(ins, IPOINT_BEFORE,(AFUNPTR) CallLog, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_END);
	}
	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)TraceLog, IARG_THREAD_ID, IARG_INST_PTR, IARG_END);
}

VOID Image(IMG img, VOID *v)
{
	if (IMG_IsMainExecutable(img))
	{
		config.SetStartTrace(IMG_Entry(img));
		for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
		{
			string segname = ".text";
			config.getAttribute("textsegname", segname);
			if (SEC_Name(sec) == segname)
			{
				config.SetTraceHead(SEC_Address(sec));
				config.SetTraceEnd(SEC_Address(sec) + SEC_Size(sec));
			}
		}
	}
	else
	{
		globalinfos.dlls.push_back(img);
		for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym))
		{
			string undFuncName = PIN_UndecorateSymbolName(SYM_Name(sym), UNDECORATION_NAME_ONLY);
			ADDRINT addr = IMG_LowAddress(img) + SYM_Value(sym);
			globalinfos.funcs.insert(pair<ADDRINT, string>(addr, undFuncName));
		}
	}
}
VOID Fini(INT32 code, VOID *v)
{
	config.end();
	logcenter.endlog();

}
//----------------------------------------
//----------------------------------------
int main(int argc, char *argv[])
{
	// Initialize PIN library. Print help message if -h(elp) is specified
	// in the command line or the command line is invalid 

	PIN_InitSymbols();
	if (PIN_Init(argc, argv))
	{
		return 0;
	}

	// Register function to be called to instrument traces
	//TRACE_AddInstrumentFunction(TraceStart, 0);
	INS_AddInstrumentFunction(INSStart, 0);
	PIN_AddThreadStartFunction(ThreadStart, 0);
	PIN_AddFiniFunction(Fini, 0);
	IMG_AddInstrumentFunction(Image, 0);


	PIN_StartProgram();

	return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
