#pragma once
#include<iostream>
#include<conio.h>
#include<ctime>
#include<chrono>
#include<algorithm>
#include<windows.h>
#include<cstdio>
#include<list>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<stack>
#include<ranges>
#include<iterator>
#include<filesystem>
#include<queue>

#define _CRT_SECURE_NO_WARNINGS // suppresing for ctime warning

using namespace std;

static void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}
static void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}
static void SetClr(int clr)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}
static void displayHeader(string Version, string Credits) {
	gotoRowCol(0, 25);
	cout << Version;
	gotoRowCol(1, 25);
	cout << Credits;
}
static string getTimeDate() {
	std::time_t now = std::time(nullptr);
	string ss;
	std::tm localTimeStruct;
	localtime_s(&localTimeStruct, &now);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%I:%M %p %m/%d/%Y", &localTimeStruct);
	ss += buffer;
	return ss;
}