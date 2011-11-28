#ifndef __TRACE_H_
#define __TRACE_H_

//void trace(const char* sLog,char* filePath="C:\\TraceLog.log");
#include <iostream>
#include <fstream>
using namespace std;
void trace(const char* sLog,char* filePath="C:\\TraceLog.log")
{
	fstream fileLog;
	fileLog.open(filePath,ios::app);
	fileLog<<sLog<<"\n";
}

#endif


//Sử dụng

/*
char sTrace[10];
itoa(needTraceVar,sTrace,10);
trace(sTrace);*/
