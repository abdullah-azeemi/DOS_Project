#pragma once
#include"Utility.h"

class textEditor
{
	// for editing mode only ---------------//
	list<list<char>> paragraph;
	list<list<char>>::iterator lineNumber;
	list<char>::iterator currLetter;

	// for processing mode only --------------// 
	list<vector<string>> dummyParagraph;
	vector<char> dummyDelimeters;
	list<vector<string>>::iterator dummyLineNumber;

	//multiple files 
	list<string> openedFiles;
	list<string>::iterator currOpenedFile;

	//cursor
	int cursorRow;
	int cursorCol;

	// Delimeters
	vector<char> delimeters{ ' ', ',', '.', '?', '!', '/', ';',':'};
	vector<char> delimeters2{',', '.', '?', '!', '/', ';',':' };

	struct textState
	{
		list<list<char>> savePara;
		int cursorR;
		int cursorC;

		textState(const list<list<char>>& p, const int& cR, const int& cC) :savePara(p), cursorR(cR), cursorC(cC)
		{
			;
		}
	};
	//deques for redo,undo
	deque<textState> redo;
	deque<textState>undo;

	void saveState(deque<textState>& dQ)
	{
		textState tS(paragraph, cursorRow, cursorCol);

		if (dQ.size() == 5)
		{
			dQ.pop_front();
		}
		dQ.push_back(tS);
	}

	void loadState(deque<textState>& dQ)
	{
		if (dQ.empty())
			return;

		textState newState(dQ.back());
		dQ.pop_back();

		this->deleteParagraph();

		paragraph = newState.savePara;
		cursorCol = newState.cursorC;
		cursorRow = newState.cursorR;
		lineNumber = paragraph.begin();
		advance(lineNumber, cursorRow);
		currLetter = lineNumber->begin();
		if (lineNumber->size() != 0)
			advance(currLetter, cursorCol - 1);
	}

	void deleteParagraph()
	{
		//clearing memory before making copy
		for (auto itr = paragraph.begin(); itr != paragraph.end(); itr++)
		{
			itr->clear();
		}
		paragraph.clear();
		paragraph.push_back(list<char>());
		cursorRow = 0;
		cursorCol = 0;
		lineNumber = paragraph.begin();
	}

	//graphicalHelpers

	void clean(int row)const
	{
		auto traverse = paragraph.begin();
		advance(traverse, row);
		int limit = traverse->size() + 2;
		gotoRowCol(row, 0);
		for (int i = 0; i < limit; i++)
		{
			cout << " ";
		}
		gotoRowCol(cursorRow, cursorCol);
	}
	void displayLine(const list<char>& line, int clr = 0)const
	{
		clean(cursorRow);
		gotoRowCol(cursorRow, 0);
		SetClr(clr, 15);
		for (auto itr = line.begin(); itr != line.end(); itr++)
		{
			cout << *itr;
		}
		gotoRowCol(cursorRow, cursorCol);
	}
	void displayParagraph()
	{
		system("cls");
		for (auto rItr = paragraph.begin(); rItr != paragraph.end(); rItr++)
		{
			for (auto cItr = (*rItr).begin(); cItr != (*rItr).end(); cItr++)
			{
				cout << (*cItr);
			}
			cout << endl;
		}
		gotoRowCol(cursorRow, cursorCol);
	}

	//splicing helper,sets iterator accordingly
	void spliceHelper(list<char>& sender, list<char>& receiver, const int& combineAt)
	{
		auto itr = sender.begin();
		advance(itr, combineAt);
		receiver.splice(receiver.begin(), sender, itr, sender.end());
	}
	void emptyRedoQueue()
	{
		redo.clear();
	}

	void saveFileExit(string fileName)
	{
		//erase from opened files
		//auto itr = find(openedFiles.begin(), openedFiles.end(), fileName);
		//openedFiles.erase(itr);
		//currOpenedFile = openedFiles.begin();
		fileName += ".txt";
		ofstream writeBack(fileName);
		for (auto rItr = paragraph.begin(); rItr != paragraph.end(); rItr++)
		{
			for (auto cItr = (*rItr).begin(); cItr != (*rItr).end(); cItr++)
			{
				writeBack << (*cItr);
			}
			writeBack << endl;
		}
		this->deleteParagraph();
	}
	void loadFile(string fileName,bool isEncoded)
	{
		//currOpenedFile = openedFiles.end();
		//currOpenedFile--;
		string format = fileName;
		format += ".txt";
		bool isFirst = true;
		ifstream reader(format);
		string line = "";
		while (getline(reader, line))
		{
			if (isFirst == false)
			{
				paragraph.push_back(list<char>());
				lineNumber++;
			}
			else
				isFirst = false;
			for (int i = 0; i < line.size(); i++)
			{
				if (!isEncoded)
				{
					lineNumber->push_back(line[i]);
				}
				else
				{
					lineNumber->push_back(char(line[i]-10));
				}
			}
			line.clear();
		}
		currLetter = lineNumber->begin();
		cursorRow = paragraph.size() - 1;
		cursorCol = lineNumber->size();
		if (cursorCol != 0)//boundary check as advance also decrements
			advance(currLetter, cursorCol - 1);
		displayParagraph();
		gotoRowCol(cursorRow, cursorCol);
	}
	void switchFiles()//to allow opening multiple files
	{
		if (this->openedFiles.size() == 1)
		{
			return;
		}
		currOpenedFile++;
		if (currOpenedFile == openedFiles.end())
		{
			currOpenedFile = openedFiles.begin();
		}
	}

	//VIM FUNCTIONS
	bool compare(int rowIter, int colIter, vector<position> selectedPos) {
		for (int ri = 0; ri < selectedPos.size(); ri++) {
			if (rowIter == selectedPos[ri].ri && colIter == selectedPos[ri].ci) {
				return true;
			}
		}
		return false;
	}
	bool isWordADelimiter(const string& word)const
	{
		if (word.size() != 1)
		{
			return false;
		}

		char c = word[0];
		if (std::ranges::find(delimeters, c) != delimeters.end())
		{
			return true;
		}
		return false;
	}
	void toUpperWord(vector<string>::iterator& word)
	{
		string toUp = (*word);
		std::for_each(toUp.begin(), toUp.end(), [](char& ch) {ch = std::toupper(ch); });
		(*word) = toUp;
	}
	void toLowerWord(vector<string>::iterator& word)
	{
		string toLow = (*word);
		std::for_each(toLow.begin(), toLow.end(), [](char& ch) {ch = std::tolower(ch); });
		(*word) = toLow;
	}
	void toUpperLetter(vector<string>::iterator& word)
	{
		string toUp = (*word);
		int index;
		gotoRowCol(80, 0);
		cout << "Enter letter Number you wish to Convert To Upper(1-BASED INDEXING)" << endl;
		cin >> index;
		processingCleanPrompt(80);
		processingCleanPrompt(81);
		index--;
		if (index < 0 || index >= toUp.size())
			return;
		std::for_each(toUp.begin()+index, toUp.begin()+index+1, [](char& ch) {ch = std::toupper(ch); });
		(*word) = toUp;
	}
	void toLowerLetter(vector<string>::iterator& word)
	{
		string toLow = (*word);
		int index;
		gotoRowCol(80, 0);
		cout << "Enter letter Number you wish to Convert To Lower(1-BASED INDEXING)" << endl;
		cin >> index;
		processingCleanPrompt(80);
		processingCleanPrompt(81);
		index--;
		if (index < 0 || index >= toLow.size())
			return;
		std::for_each(toLow.begin() + index, toLow.begin() + index + 1, [](char& ch) {ch = std::tolower(ch); });
		(*word) = toLow;
	}

public:

	textEditor() :paragraph({ list<char>() }), cursorRow(0), cursorCol(0)
	{
		lineNumber = paragraph.begin();
	}
	void editFile(string fileName, bool load = false, bool isEncoded = false)
	{
		//this->openedFiles.push_back(fileName);//add name to opened files
		//set color to white
		system("color f0");
		system("cls");
		if (load)
		{
			loadFile(fileName,isEncoded);
		}
		int currButtonPressed;


		while (1)
		{
			currButtonPressed = _getch();

			if (currButtonPressed == 224)//are arrow keys
			{
				currButtonPressed = _getch();

				if (currButtonPressed == 72)//Up
				{
					if (cursorRow != 0)
					{
						lineNumber--;
						cursorRow--;

						if (cursorCol > (*lineNumber).size())//see cursor is not away from last character in line
						{
							cursorCol = (*lineNumber).size();
						}
						currLetter = lineNumber->begin();
						for (int i = 0; i < cursorCol - 1; i++)
							currLetter++;
						//advance(currLetter, cursorCol - 2);//recheck
					}
				}
				else if (currButtonPressed == 75)//left
				{
					if (cursorCol != 0)
					{
						if (cursorCol != 1)//boundary check
						{
							currLetter--;
						}

						cursorCol--;

					}
				}
				else if (currButtonPressed == 77)//right
				{
					if (cursorCol != (*lineNumber).size())
					{
						cursorCol++;
						if (cursorCol > 1)//boundary check
						{
							currLetter++;
						}
					}
				}
				else if (currButtonPressed == 80)//Down
				{
					if (cursorRow != paragraph.size() - 1)
					{
						lineNumber++;
						cursorRow++;

						if (cursorCol > (*lineNumber).size())
						{
							cursorCol = (*lineNumber).size();
						}
						currLetter = lineNumber->begin();
						for (int i = 0; i < cursorCol - 1; i++)
						{
							currLetter++;
						}
					}
				}
				else if (currButtonPressed == 83)//delete
				{
					if (cursorCol != lineNumber->size())
					{
						saveState(undo);
						emptyRedoQueue();
						if (cursorCol == 0)
						{
							(*lineNumber).erase(lineNumber->begin());//delete only character
							cursorCol = 0;
							displayLine(*lineNumber);
						}
						else
						{
							auto dummy = currLetter;
							lineNumber->erase(++dummy);
							displayLine(*lineNumber);
						}
					}
				}
			}
			else if (currButtonPressed == 8)//Backspace
			{
				if (cursorCol == 0 && cursorRow == 0)
					continue;

				emptyRedoQueue();
				saveState(undo);

				if (cursorRow > 0 && cursorCol == 0)//move one line back
				{
					auto temp = lineNumber;
					--lineNumber;
					int old = lineNumber->size();
					cursorCol = lineNumber->size();
					lineNumber->splice(lineNumber->end(), *temp, temp->begin(), temp->end());
					cursorRow--;
					paragraph.erase(temp);
					currLetter = lineNumber->begin();
					if (old != 0)
						advance(currLetter, cursorCol - 1);
					displayParagraph();
					continue;
				}
				else
				{
					//traverse in current line
					auto temp = currLetter;
					cursorCol--;
					if (temp != lineNumber->begin())
					{
						--temp;
					}
					lineNumber->erase(currLetter);//delete letter
					currLetter = temp;
					clean(cursorRow);
					displayLine(*lineNumber);
					gotoRowCol(cursorRow, cursorCol);

				}

			}
			else if (currButtonPressed == 13)//enter
			{
				saveState(undo);
				emptyRedoQueue();
				/*
				if (cursorRow == paragraph.size() - 1)
				{
					auto dummy = lineNumber;
					paragraph.push_back(list<char>());
					lineNumber++;
					cursorRow++;
					if (cursorCol != dummy->size())
					{
						auto itr = dummy->begin();
						advance(itr, cursorCol);
						lineNumber->splice(lineNumber->begin(), *dummy, itr, dummy->end());
					}
					cursorCol = 0;
					displayParagraph();
				}
				*/
				if (cursorRow == paragraph.size() - 1)
				{
					auto dummy = lineNumber;
					paragraph.push_back(list<char>());//add new line
					lineNumber++;//move to new line
					if (cursorCol != dummy->size())//have to splice
					{
						spliceHelper((*dummy), (*lineNumber), cursorCol);
					}
					cursorRow++;//move to new line
					cursorCol = 0;//start of new line
					currLetter = lineNumber->begin();
					displayParagraph();
				}
				else
				{
					auto dummy = lineNumber;
					paragraph.insert(lineNumber, list<char>());//iinsert in between
					cursorRow++;
					dummy--;
					auto temp = (*lineNumber);
					(*lineNumber) = (*dummy);
					(*dummy) = temp;
					if (cursorCol != dummy->size())
					{
						spliceHelper((*dummy), (*lineNumber), cursorCol);
					}
					currLetter = lineNumber->begin();
					cursorCol = 0;
					displayParagraph();
				}
			}
			else if (currButtonPressed == 26)//CTRL+Z UNDO
			{
				saveState(redo);
				system("cls");
				loadState(undo);
				displayParagraph();
			}
			else if (currButtonPressed == 25)//CTRL+Y REDO
			{
				if (redo.empty())
					continue;
				saveState(undo);
				system("cls");
				loadState(redo);
				displayParagraph();
			}
			else if (currButtonPressed == 27)//Escape close editor and save file
			{
				system("cls");
				system("color 0f");
				saveFileExit(fileName);
				break;
			}

			else if (currButtonPressed == 19) { // Ctrl + S // testing processing functions here
				//processingMode();
				processingMode();//Processing Mode
				displayParagraph();
			}

			else//data entry
			{
				emptyRedoQueue();
				saveState(undo);
				if (lineNumber->empty() || (cursorCol == 0 && lineNumber->size() != 0))//insert at front
				{
					lineNumber->push_front(currButtonPressed);//automatically converts to ascii to char
					currLetter = (*lineNumber).begin();
					cursorCol++;
				}
				else if (cursorCol == (*lineNumber).size())
				{
					lineNumber->push_back(currButtonPressed);
					cursorCol++;
					currLetter++;
				}
				else//use insert for insertion at middle
				{
					//do currLetter++ as inserts behind the provided iterator element
					currLetter++;
					lineNumber->insert(currLetter, char(currButtonPressed));
					currLetter--;
					cursorCol++;
				}

				displayLine(*lineNumber);
			}
			gotoRowCol(cursorRow, cursorCol);//for arrows
		}
	}
	// form editing to processing
	void saveContent() {

		// first we will delete the deep copy
		for (auto itr : dummyParagraph) {
			itr.clear();
		}
		dummyParagraph.clear();

		for (auto itr : paragraph) {
			vector<string> words;
			string word;
			for (auto charItr = itr.begin(); charItr != itr.end(); ++charItr) {
				if (std::ranges::find(delimeters, *charItr) != delimeters.end()) {
					dummyDelimeters.push_back(*charItr);
					if(word.empty()==false)
						words.push_back(word);
					word.clear();
					
					word += *charItr;
					words.push_back(word);
					word.clear();
					
				}
				
				else {
					word += *charItr;
				}
			}
			if (word.empty() == false) {
				words.push_back(word);
			}
			dummyParagraph.push_back(words);
		}

	}
	// from processing to editing
	void saveContentreverse() {
		
		for (auto& line : paragraph) {
			line.clear(); 
		}
		paragraph.clear(); 

		for (auto words : dummyParagraph) {
			string line;
			for (auto word : words) {
				line += word;
			}
			list<char> charList(line.begin(), line.end());
			paragraph.push_back(charList);
			lineNumber = paragraph.begin();
			currLetter = lineNumber->begin();
			cursorCol = 0;
			cursorRow = 0;
		}
	}

	// Processing Mode functions ----------------//
	// caseSensitive
	vector<position> findWord(string findWord) {
		vector<position> ans;
		int rowItr = 0;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				if (word == findWord) {
					position pos;
					pos.ri = rowItr;
					pos.ci = colItr;
					ans.push_back(pos);
				}
				colItr += word.size();
			}
			rowItr++;
		}
		return ans;
	}
	vector<position> findWordInsensitive(string findWord) {
		std::for_each(findWord.begin(), findWord.end(), [](char& ch) {
			ch = std::tolower(ch); });
		vector<position> ans;
		int rowItr = 0;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				string dummyWord;
				dummyWord.append(word);
				std::for_each(dummyWord.begin(), dummyWord.end(), [](char& ch) {
					ch = std::tolower(ch); });

				if (dummyWord == findWord) {
					position pos;
					pos.ri = rowItr;
					pos.ci = colItr;
					ans.push_back(pos);
				}
				colItr += word.size();
			}
			rowItr++;
		}
		return ans;
	}
	vector<position> findSubword(string & orignalWord,string subword) {
		vector<position> ans;
		position ans1{ -1,-1 };
		int row = 0;
		for (auto line : dummyParagraph) {
			int col = 0;
			for (auto word : line) {
				size_t found = word.find(subword);
				if (found != string::npos) {
					ans1 = { row,col };
					ans.push_back(ans1);
					orignalWord.append(word);
					return ans;
				}
				col = col + word.size();
			}
			row++;
		}
		ans.push_back(ans1);
		return ans;
	}
	

	void movementFindWordsCaseSensitive(string word)
	{
		auto items = findWord(word);
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			//highlight
			printProcessing(items);
		}
	}
	void movementFindWordsCaseInsensitive(string word)
	{
		auto items = findWordInsensitive(word);
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			//highlight
			printProcessing(items);
		}
	}
	void movementFindsubWords(string word) {
		string orignalWord;
		auto items = findSubword(orignalWord,word);
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else{
			//highlight
			highlightSubword(items[0],orignalWord, word);
		}
	}
	vector<position> replaceWord(string findWord,string newWord) 
	{
		vector<position> ans;
		int rowItr = 0;
		for (auto lineItr=dummyParagraph.begin();lineItr!=dummyParagraph.end();lineItr++) 
		{
			int colItr = 0;
			for (auto wordItr=(*lineItr).begin();wordItr!=(*lineItr).end();wordItr++)
			{
				if ((*wordItr) == findWord) {
					position pos;
					(*wordItr) = newWord;
					pos.ri = rowItr;
					pos.ci = colItr;
					ans.push_back(pos);
				}
				colItr += (*wordItr).size();
			}
			rowItr++;
		}
		return ans;
	}
	void movementReplaceWords(string word,string newWord)
	{
		auto items = replaceWord(word,newWord);
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			//highlight
			printProcessing(items);
		}
	}
	vector<iteratorPosition> findNextAll(string word, const position& cursor) {
		int rowIter = 0;
		bool isFound = false;
		vector<iteratorPosition> ans;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++) {
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) {
				if (rowIter == cursor.ri && colIter == cursor.ci) {
					isFound = true;
				}
				if (isFound && *(itr) == word) {
					iteratorPosition pos;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					ans.push_back(pos);
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		return ans;
	}
	void movementFindNextAll(string word, vector<string>::iterator& currentWord,int &pRow,int&pCol)
	{
		auto items = findNextAll(word, position(pRow, pCol));
		if(items.size()==0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found after cursor. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			auto itr = items.begin();
			pRow = itr->ri;
			pCol = itr->ci;
			currentWord = itr->colIter;
			gotoRowCol(pRow, pCol);
			int button;
			while (itr != items.end())
			{
				button = _getch();
				if (button == 224)//are arrow keys
				{
					button = _getch();
					if (button == 75)//left
					{
						if (itr != items.begin())
						{
							itr--;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
					else if (button == 77)//right
					{
						if (itr != (--items.end()))
						{
							itr++;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
				}
				else if (button == 27)//escape
				{
					break;
				}
			}
		}
	}
	iteratorPosition findNext(string word, const position& cursor) 
	{
		int rowIter = 0;
		bool isFound = false;
		iteratorPosition pos{};
		pos.ri = -1;
		pos.ci = -1;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end();lineItr++) 
		{
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) 
			{
				if (rowIter==cursor.ri && colIter==cursor.ci) {
					isFound = true;
				}
				if (isFound && *(itr) == word) {
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					return pos;
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		return pos;
	}
	iteratorPosition findPrev(string word, const position& cursor) 
	{
		int rowIter = 0;
		vector<iteratorPosition> ans;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++) {
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) {
				if (rowIter == cursor.ri && colIter == cursor.ci) {
					if (ans.empty())
					{
						iteratorPosition p;
						p.ci = -1;
						p.ri = -1;
						return p;
					}
					return ans[ans.size() - 1];
				}
				if (*(itr) == word) {
					iteratorPosition pos;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					ans.push_back(pos);
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		iteratorPosition dummy;
		dummy.ri = -1;
		dummy.ci = -1;
		return dummy;
	}
	vector<iteratorPosition> findPrevAll(string word, const position& cursor) {
		int rowIter = 0;
		vector<iteratorPosition> ans;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++) {
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) {
				if (rowIter == cursor.ri && colIter == cursor.ci)
				{
						reverse(ans.begin(), ans.end());
						return ans;
				}
				if (*(itr) == word) {
					iteratorPosition pos;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					ans.push_back(pos);
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		return ans;
	}
	void movementFindPrevAll(string word, vector<string>::iterator& currentWord, int& pRow, int& pCol)
	{
		auto items = findPrevAll(word, position(pRow, pCol));
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found Before Cursor. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			auto itr = items.begin();
			pRow = itr->ri;
			pCol = itr->ci;
			currentWord = itr->colIter;
			gotoRowCol(pRow, pCol);
			int button;
			while (itr != items.end())
			{
				button = _getch();
				if (button == 224)//are arrow keys
				{
					button = _getch();
					if (button == 75)//left
					{
						if (itr != (--items.end()))
						{
							itr++;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
					else if (button == 77)//right
					{
						if (itr !=items.begin())
						{
							itr--;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
				}
				else if (button == 27)//escape
				{
					break;
				}
			}
		}
	}

	//replace from cursor
	iteratorPosition replaceNext(string word, const position& cursor,string newWordToReplace) 
	{
		int rowIter = 0;
		bool isFound = false;
		iteratorPosition pos{};
		pos.ri = -1;
		pos.ci = -1;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end();lineItr++) 
		{
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) 
			{
				if (rowIter==cursor.ri && colIter==cursor.ci) {
					isFound = true;
				}
				if (isFound && *(itr) == word) {
					(*itr) = newWordToReplace;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					return pos;
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		return pos;
	}
	vector<iteratorPosition> replaceNextAll(string word, const position& cursor,string newWordToReplace) {
		int rowIter = 0;
		bool isFound = false;
		vector<iteratorPosition> ans;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++) {
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) {
				if (rowIter == cursor.ri && colIter == cursor.ci) {
					isFound = true;
				}
				if (isFound && *(itr) == word) {
					iteratorPosition pos;
					(*itr) = newWordToReplace;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					ans.push_back(pos);
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		return ans;
	}
	void movementReplaceNextAll(string word, vector<string>::iterator& currentWord, int& pRow, int& pCol,string newWordToReplace)
	{
		auto items = replaceNextAll(word, position(pRow, pCol),newWordToReplace);
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found after cursor. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			printProcessing();
			auto itr = items.begin();
			pRow = itr->ri;
			pCol = itr->ci;
			currentWord = itr->colIter;
			gotoRowCol(pRow, pCol);
			int button;
			while (itr != items.end())
			{
				button = _getch();
				if (button == 224)//are arrow keys
				{
					button = _getch();
					if (button == 75)//left
					{
						if (itr != items.begin())
						{
							itr--;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
					else if (button == 77)//right
					{
						if (itr != (--items.end()))
						{
							itr++;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
				}
				else if (button == 27)//escape
				{
					break;
				}
			}
		}
	}
	iteratorPosition replacePrev(string word, const position& cursor,string newWordToReplace)
	{
		int rowIter = 0;
		vector<iteratorPosition> ans;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++) {
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) {
				if (rowIter == cursor.ri && colIter == cursor.ci) {
					if (ans.empty())
					{
						iteratorPosition p;
						p.ci = -1;
						p.ri = -1;
						return p;
					}
					(*ans[ans.size() - 1].colIter) = newWordToReplace;
					return ans[ans.size() - 1];
				}
				if (*(itr) == word) {
					iteratorPosition pos;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					ans.push_back(pos);
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		iteratorPosition dummy;
		dummy.ri = -1;
		dummy.ci = -1;
		return dummy;
	}
	vector<iteratorPosition> replacePrevAll(string word, const position& cursor,string newWordToReplace) {
		int rowIter = 0;
		vector<iteratorPosition> ans;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++) {
			int colIter = 0;
			for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++) {
				if (rowIter == cursor.ri && colIter == cursor.ci)
				{
					reverse(ans.begin(), ans.end());
					return ans;
				}
				if (*(itr) == word) {
					iteratorPosition pos;
					(*itr) = newWordToReplace;
					pos.colIter = itr;
					pos.ri = rowIter;
					pos.ci = colIter;
					ans.push_back(pos);
				}
				colIter = itr->size() + colIter;
			}
			rowIter++;
		}
		return ans;
	}
	void movementReplacePrevAll(string word, vector<string>::iterator& currentWord, int& pRow, int& pCol,string newWordToReplace)
	{
		auto items = replacePrevAll(word, position(pRow, pCol),newWordToReplace);
		if (items.size() == 0)
		{
			gotoRowCol(80, 0);
			cout << "No Such word was found Before Cursor. Press any key to continue" << endl;
			_getch();
			processingCleanPrompt(80);
		}
		else
		{
			printProcessing();
			auto itr = items.begin();
			pRow = itr->ri;
			pCol = itr->ci;
			currentWord = itr->colIter;
			gotoRowCol(pRow, pCol);
			int button;
			while (itr != items.end())
			{
				button = _getch();
				if (button == 224)//are arrow keys
				{
					button = _getch();
					if (button == 75)//left
					{
						if (itr != (--items.end()))
						{
							itr++;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
					else if (button == 77)//right
					{
						if (itr != items.begin())
						{
							itr--;
							pRow = itr->ri;
							pCol = itr->ci;
							currentWord = itr->colIter;
							gotoRowCol(pRow, pCol);
						}
					}
				}
				else if (button == 27)//escape
				{
					break;
				}
			}
		}
	}
	
	//replace from cursor end
	position findSubword2(string subword) {
		position ans{ -1,-1 };
		int row = 0;
		for (auto line : dummyParagraph) {
			int col = 0;
			for (auto word : line) {
				size_t found = word.find(subword);
				if (found != string::npos) {
					ans = { row,col };
					return ans;
				}
				col++;
			}
			row++;
		}
		return ans;
	}

	position findSentence(string findSentence) {
		position ans{ -1,-1 };
		int row = 0;
		for (auto line : dummyParagraph) {
			string sentence;
			int col = 0;
			int startCol = 0;
			for (auto word : line) {
				if (std::ranges::find(delimeters2, word[0]) != delimeters2.end()) {
					sentence.clear();
					startCol = col;
					col = 0;
				}
				sentence += word;
				if (std::ranges::find(delimeters, sentence[0]) != delimeters.end()) {
					sentence = sentence.substr(1);
				}

				if (sentence == findSentence) {
					ans.ri = row;
					ans.ci = startCol;
					return ans;
				}

				col += word.size();
			}
			row++;
		}
		return ans;
	}


	void printFindSentence(const position& startPos, const string sentence) {
		int startItr = 0;
		SetClr(5, 15);
		gotoRowCol(startPos.ri, startPos.ci);
		for (int ri = 0; ri < sentence.size(); ri++) {
			cout << sentence[ri];
		}
		SetClr(0, 15);
	}


	int averageWordlenght() {
		int words = 0, letters = 0;
		for (auto line : dummyParagraph) {
			for (auto word : line) {
				if (!(isWordADelimiter(word)))
				{
					letters += word.size();
					words++;
				}
			}
		}
		return letters / words;

	}
	int countSentences() 
	{
		vector<char> stoppers{ '.', '?', '!', ';'};
		int count = 0;
		for (auto line : dummyParagraph) {
			for (auto word : line) {
				for (auto ch : word) {
					if (std::ranges::find(stoppers, ch) != stoppers.end()) {
						count++;
					}
				}
			}
		}
		return count;
	}
	int largestWordlength() {
		int maxWordLength = 0;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				if (!(isWordADelimiter(word)))
				{
					maxWordLength = max(maxWordLength, word.size());
				}
			}
		}
		return maxWordLength;
	}
	int smallestWordLength() {
		int minWordLength = INT_MAX;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				if (!(isWordADelimiter(word)))
				{
					minWordLength = min(minWordLength, word.size());
				}
			}
		}
		return minWordLength;
	}
	int countSubstrings(const string& subword) {
		int count = 0;
		for (const auto& line : dummyParagraph) {
			for (const auto& word : line) {
				size_t found = word.find(subword);
				while (found != string::npos) {
					count++;
					found = word.find(subword, found + 1);
				}
			}
		}
		return count;
	}

	int specialCharacters() {
		int count = 0;
		for (auto line : dummyParagraph) {
			for (auto word : line) {
				for (auto ch : word) {
					if (!isalnum(ch)) {
						count++;
					}
				}
			}
		}
		return count;
	}
	int countParagraphs()const
	{
		int num = 0;
		bool hasStarted = false;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++)
		{
			if (!(lineItr->empty()))
			{
				hasStarted = true;
			}
			if (lineItr->empty() && hasStarted)
			{
				num++;
				hasStarted = false;
			}
		}
		if (hasStarted)
		{
			num++;
		}
		return num;
	}
	int findLargestParagraphSize()const
	{
		int num = 0;
		int maxNum = -1;
		bool hasStarted = false;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++)
		{
			if (!(lineItr->empty()))
			{
				hasStarted = true;
			}
			if (lineItr->empty() && hasStarted)
			{
				maxNum = max(maxNum, num);
				num = 0;
				hasStarted = false;
			}
			if (hasStarted)
			{
				for (auto itr = lineItr->begin(); itr != lineItr->end(); itr++)
				{
					if (!(isWordADelimiter((*itr))))
					{
						num++;
					}
				}
			}
		}
		if (hasStarted)
		{
			maxNum = max(maxNum, num);
		}
		return maxNum;
	}
	// findWord Next 
	

	// Prinitng functions
	void printProcessing(vector<position> selectedPos = {}) {
		system("color f0");
		system("cls");
		int clr = 0;
		int rowIter = 0;
		SetClr(0, 15);
		for (auto line : dummyParagraph) {
			int colIter = 0;
			for (auto word : line) {
				
				if (compare(rowIter, colIter, selectedPos)) {
					SetClr(clr = 1, 15);
					cout << word;
					SetClr(clr = 0, 15);
				}
				else {
					cout << word;
				}
				colIter+=word.size();
			}
			rowIter++;
			cout << endl;
		}
	}
	void processingLinePrint(list<vector<string>>::iterator line,int row)const
	{
		gotoRowCol(row, 0);
		SetClr(0, 15);
		for (auto itr = line->begin(); itr != line->end(); itr++)
		{
			cout << (*itr);
		}
	}
	void processingCleanLinePrint(list<vector<string>>::iterator line, int row)const
	{
		gotoRowCol(row, 0);
		SetClr(0, 15);
		cout << "                                                                                                                       ";
	}
	void processingCleanPrompt(int row)
	{
		gotoRowCol(row, 0);
		cout << "                                                                                                                                                                " << endl;
	}
	void highlightSubword(const position& startPos, string word, string subWord) {
		
		int startItr = 0;
		SetClr(5, 15);
		for (int ri = 0; ri < word.length(); ri++){
			gotoRowCol(startPos.ri, startPos.ci+ri);
			if (word[ri] == subWord[startItr]) {
				cout << subWord[startItr];
				startItr++;
			}
		}
		SetClr(0, 15);
	}


	void addPostfix(string findWord, string postWord) {
		for (auto& line : dummyParagraph) {
			int colItr = 0;
			for (auto& word : line) {
				if (word == findWord) {
					word += postWord;
				}
			}
		}
	}
	void addPrefix(string findWord, string preWord) {
		for (auto& line : dummyParagraph) {
			for (auto& word : line) {
				if (word == findWord) {
					word = preWord + word;
				}
			}
		}
	}


	//WORD GAME
	bool canBeMade(const set<char>& main, const set<char>& sub)const
	{
		for (auto itr = sub.begin(); itr != sub.end(); itr++)
		{
			auto find = main.find((*itr));
			if (find == main.end())
			{
				return false;
			}
		}
		return true;
	}
	set<char> convertWordToChar(const string& word)const
	{
		set<char>s;
		for (int i = 0; i < word.size(); i++)
		{
			s.insert(word[i]);
		}
		return s;
	}
	int calculateWordsMade(const string& word)const
	{
		set<char>main = convertWordToChar(word);
		int count = 0;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++)
		{
			for (auto colItr = (*lineItr).begin(); colItr != (*lineItr).end(); colItr++)
			{
				if (isWordADelimiter(*colItr))
				{
					continue;
				}
				set<char>sub = convertWordToChar(*colItr);
				if (canBeMade(main, sub))
				{
					count++;
				}
			}
		}
		return count;
	}
	string wordGame()const
	{
		string word = "-";
		int wordsMadeMax = 0;
		for (auto lineItr = dummyParagraph.begin(); lineItr != dummyParagraph.end(); lineItr++)
		{
			for (auto colItr = (*lineItr).begin(); colItr != (*lineItr).end(); colItr++)
			{
				int wordsMade = 0;
				wordsMade = calculateWordsMade((*colItr));
				if (wordsMade > wordsMadeMax)
				{
					wordsMadeMax = wordsMade;
					word = (*colItr);
				}
			}
		}
		return word;
	}



	//MERGE FILE
	void mergeFile()
	{
		string format;
		gotoRowCol(80, 0);
		cout << "Enter FileName to Merge Into This " << endl;;
		cin >> format;
		processingCleanPrompt(80);
		processingCleanPrompt(81);
		format += ".txt";
		ifstream rdr(format);
		if (!rdr)
		{
			gotoRowCol(80, 0);
			cout << "No Such File Was Found. Press Any Key To Continue";
			_getch();
			processingCleanPrompt(80);
			return;
		}
		string dummy = "";
		vector<string> line;
		while (getline(rdr,dummy))
		{
			string word = "";
			for (int i = 0; i < dummy.size(); i++)
			{
				char c = dummy[i];
				if (std::ranges::find(delimeters, c) != delimeters.end())
				{
					if (!word.empty())
					{
						line.push_back(word);
					}
					word.clear();
					word += c;
					line.push_back(word);
					word.clear();
				}
				else
				{
					word += c;
				}

			}
			if (word.empty() == false)
				line.push_back(word);
			dummyParagraph.push_back(line);
			line.clear();
		}
		printProcessing();
	}
	//Processing Mode
	void processingMode()
	{
		saveContent();
		list<vector<string>>::iterator dummyLineNumberV2;
		vector<string>::iterator currentWord;
		int pRow, pCol;
		dummyLineNumberV2 = dummyParagraph.begin();
		currentWord = (*dummyLineNumberV2).begin();
		int currButtonPressed;
		pRow = 0;
		pCol = 0;
		printProcessing();
		gotoRowCol(pRow, pCol);
		while (true)
		{
			currButtonPressed = _getch();
			if (currButtonPressed == 224)//are arrow keys
			{
				currButtonPressed = _getch();

				if (currButtonPressed == 72)//Up
				{
					if (pRow != 0)
					{
						dummyLineNumberV2--;
						pRow--;
						pCol = 0;
						currentWord = dummyLineNumberV2->begin();
					}
				}
				else if (currButtonPressed == 75)//left
				{
					if (currentWord != (*dummyLineNumberV2).begin())
					{
						currentWord--;
						pCol = pCol - (*currentWord).size();
					}
				}
				else if (currButtonPressed == 77)//right
				{
					if (dummyLineNumberV2->begin() == dummyLineNumberV2->end())
					{
						continue;
					}
					if (currentWord != (--(*dummyLineNumberV2).end()))
					{
						int size = currentWord->size();
						currentWord++;
						pCol = pCol + size;
					}
				}
				else if (currButtonPressed == 80)//Down
				{
					if (pRow != dummyParagraph.size() - 1)
					{
						dummyLineNumberV2++;
						pRow++;
						pCol = 0;
						currentWord = dummyLineNumberV2->begin();
					}
				}
			}
			else if (currButtonPressed == 27)//Escape move to editing mode
			{
				system("cls");
				saveContentreverse();
				break;
			}
			else if (currButtonPressed == 117)//U pressed Upper case word
			{
				toUpperWord(currentWord);
				processingCleanLinePrint(dummyLineNumberV2, pRow);
				processingLinePrint(dummyLineNumberV2,pRow);
			}
			else if (currButtonPressed == 21)//CTRL+U pressed Upper case letter
			{
				toUpperLetter(currentWord);
				processingCleanLinePrint(dummyLineNumberV2, pRow);
				processingLinePrint(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 108)//L pressed Lower case word
			{
				toLowerWord(currentWord);
				processingCleanLinePrint(dummyLineNumberV2, pRow);
				processingLinePrint(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 12)//CTRL+L pressed lower case letter
			{
				toLowerLetter(currentWord);
				processingCleanLinePrint(dummyLineNumberV2, pRow);
				processingLinePrint(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 70)//Shift+f find word Highlight case sensitive
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				movementFindWordsCaseSensitive(word);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 73) {//Shift+i find word Highlight case inSensitive
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				movementFindWordsCaseInsensitive(word);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 79) {//Shift+o find subWord
				string word;
				gotoRowCol(80, 0);
				cout << "Enter Sub word to find:" << endl;
				cin >> word;
				movementFindsubWords(word);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 102)// f pressed find next
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				auto itrPosition=findNext(word,position(pRow,pCol));
				if (itrPosition.ri != -1)
				{
					pRow = itrPosition.ri;
					pCol = itrPosition.ci;
					currentWord = itrPosition.colIter;
					//update line number as well
					dummyLineNumberV2 = dummyParagraph.begin();
					advance(dummyLineNumberV2, pRow);
				}
				else
				{
					gotoRowCol(80, 0);
					cout << "No Such word was found after cursor. Press any key to continue" << endl;
					_getch();
					processingCleanPrompt(80);
				}
			}
			else if (currButtonPressed == 6)// Ctrl +F pressed find next All
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				movementFindNextAll(word, currentWord, pRow, pCol);
				dummyLineNumberV2 = dummyParagraph.begin();
				advance(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 100)// D pressed find prev
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				auto itrPosition = findPrev(word, position(pRow, pCol));
				if (itrPosition.ri != -1)
				{
					pRow = itrPosition.ri;
					pCol = itrPosition.ci;
					currentWord = itrPosition.colIter;
					//update line number as well
					dummyLineNumberV2 = dummyParagraph.begin();
					advance(dummyLineNumberV2, pRow);
				}
				else
				{
					gotoRowCol(80, 0);
					cout << "No Such word was found before cursor. Press any key to continue" << endl;
					_getch();
					processingCleanPrompt(80);
				}
			}
			else if (currButtonPressed == 4)// Ctrl +D pressed find prev All
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				movementFindPrevAll(word, currentWord, pRow, pCol);
				dummyLineNumberV2 = dummyParagraph.begin();
				advance(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 82)//Shift+r replace word Highlight
			{
				string word;
				string newWord;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				gotoRowCol(80, 0);
				cout << "Enter word to replace:" << endl;
				cin >> newWord;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				movementReplaceWords(word,newWord);
				pCol = 0;
				currentWord = dummyLineNumberV2->begin();
			}
			else if (currButtonPressed == 114)// r pressed replace next
			{
				string word;
				string replaceWord;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				gotoRowCol(80, 0);
				cout << "Enter word to replace:" << endl;
				cin >> replaceWord;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				auto itrPosition = replaceNext(word, position(pRow, pCol),replaceWord);
				if (itrPosition.ri != -1)
				{
					pRow = itrPosition.ri;
					pCol = itrPosition.ci;
					currentWord = itrPosition.colIter;
					//update line number as well
					dummyLineNumberV2 = dummyParagraph.begin();
					advance(dummyLineNumberV2, pRow);
					processingCleanLinePrint(dummyLineNumberV2, pRow);
					processingLinePrint(dummyLineNumberV2, pRow);
				}
				else
				{
					gotoRowCol(80, 0);
					cout << "No Such word was found after cursor. Press any key to continue" << endl;
					_getch();
					processingCleanPrompt(80);
				}
			}
			else if (currButtonPressed == 18)// Ctrl +r pressed replace next All
			{
				string word;
				string newWord;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				gotoRowCol(80, 0);
				cout << "Enter word to Replace:" << endl;
				cin >> newWord;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				movementReplaceNextAll(word, currentWord, pRow, pCol,newWord);
				dummyLineNumberV2 = dummyParagraph.begin();
				advance(dummyLineNumberV2, pRow);
				printProcessing();
			}
			else if (currButtonPressed == 101)// E pressed replace prev
			{
				string word;
				string replaceWord;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				gotoRowCol(80, 0);
				cout << "Enter word to replace:" << endl;
				cin >> replaceWord;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				auto itrPosition = replacePrev(word, position(pRow, pCol), replaceWord);
				if (itrPosition.ri != -1)
				{
					pRow = itrPosition.ri;
					pCol = itrPosition.ci;
					currentWord = itrPosition.colIter;
					//update line number as well
					dummyLineNumberV2 = dummyParagraph.begin();
					advance(dummyLineNumberV2, pRow);
					processingCleanLinePrint(dummyLineNumberV2, pRow);
					processingLinePrint(dummyLineNumberV2, pRow);
				}
				else
				{
					gotoRowCol(80, 0);
					cout << "No Such word was found Before cursor. Press any key to continue" << endl;
					_getch();
					processingCleanPrompt(80);
				}
			}
			else if (currButtonPressed == 5)// Ctrl +E pressed replace prev All
			{
				string word;
				string newWord;
				gotoRowCol(80, 0);
				cout << "Enter word to find:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				gotoRowCol(80, 0);
				cout << "Enter word to Replace:" << endl;
				cin >> newWord;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				movementReplacePrevAll(word, currentWord, pRow, pCol, newWord);
				dummyLineNumberV2 = dummyParagraph.begin();
				advance(dummyLineNumberV2, pRow);
				printProcessing();
			}
			else if (currButtonPressed == 19)// CTRL+S find SUBSTRING
			{
				string subString;
				gotoRowCol(80, 0);
				cout << "Enter subString to find:" << endl;
				cin >> subString;
				movementFindsubWords(subString);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 97)//A AVERAGE word length
			{
				gotoRowCol(80, 0);
				cout << "Average word length:" << averageWordlenght()<<endl<<"press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 65)//SHIFT+A special character count
			{
				gotoRowCol(80, 0);
				cout << "Special characters count:" << specialCharacters() << endl << "press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 99)//C count paragraphs
			{
				gotoRowCol(80, 0);
				cout << "Paragraph count:" << countParagraphs() << endl << "press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 3)//CTRL+C count largest paragraph length
			{
				gotoRowCol(80, 0);
				cout << "Largest Paragraph length:" <<findLargestParagraphSize() <<" words"<< endl << "press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 109)//M maximum word length
			{
				gotoRowCol(80, 0);
				cout << "Largest Word length:" << largestWordlength() << endl << "press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 13)//CTRL+M minimum word length
			{
				gotoRowCol(80, 0);
				cout << "Smallest Word length:" << smallestWordLength() << endl << "press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 1)//CTRL+A total sentences count
			{
				gotoRowCol(80, 0);
				cout << "Total sentences:" << countSentences() << endl << "press any key to continue";
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 112)//P Prefix
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to add preFix:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				string preFix;
				gotoRowCol(80, 0);
				cout << "Enter the prefix to add" << endl;
				cin >> preFix;
				addPrefix(word,preFix);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				printProcessing();
			}
			else if (currButtonPressed == 16)//CTRL+P Postfix
			{
				string word;
				gotoRowCol(80, 0);
				cout << "Enter word to add postFix:" << endl;
				cin >> word;
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				string postFix;
				gotoRowCol(80, 0);
				cout << "Enter the PostFix to add" << endl;
				cin >> postFix;
				addPostfix(word, postFix);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				printProcessing();
			}
			else if (currButtonPressed == 119)//word game
			{
				gotoRowCol(80, 0);
				cout << "Word Game Largest Domain Word:" << endl;
				cout << wordGame();
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 89)// SHIFT+y  subString count
			{
				string subString;
				gotoRowCol(79, 0);
				cout << "Enter the SubString : ";
				cin >> subString;
				gotoRowCol(80, 0);
				cout << "Substring Count is :" << countSubstrings(subString) <<endl;
				cout << "press any key to continue" << endl;
				_getch();
				processingCleanPrompt(80);
				processingCleanPrompt(81);
			}
			else if (currButtonPressed == 9)// TAB  MERGE
			{
				mergeFile();
			}
			else if (currButtonPressed == 86) { // SHIFt + v find Sentence
				string sentence;
				gotoRowCol(80, 0);
				cout << "Enter the Sentence : " << endl;;
				getline(cin, sentence);
				processingCleanPrompt(80);
				processingCleanPrompt(81);
				position st = findSentence(sentence);
				if (st.ri != -1 && st.ci != -1) {
					printFindSentence(st, sentence);
				}
				else {
					gotoRowCol(80, 0);
					cout << "Sentence Not Found...." << endl;
					_getch();
					processingCleanPrompt(80);
				}
				}


			gotoRowCol(pRow, pCol);
		}

	}
};

