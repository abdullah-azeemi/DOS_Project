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
	vector<char> delimeters{ ' ', ',', '.', '?', '!', '/', ';' };

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
		auto itr = find(openedFiles.begin(), openedFiles.end(), fileName);
		openedFiles.erase(itr);
		currOpenedFile = openedFiles.begin();
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
		currOpenedFile = openedFiles.end();
		currOpenedFile--;
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

	bool isWordADelimiter(const string& word)
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
		this->openedFiles.push_back(fileName);//add name to opened files
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
				movementInProcessing();//for testing arrow keys
				
				saveContent();
				bool x = false;
				// Testing all form here //
				vector<position> an = findWord("cat", x);
				int w = findSentence("i am a very good cat");
				position w1 = findSubword("ver");
				int avg = averageWordlenght();
				int sp = specialCharacters();
				int sCount = noOfSentences();
				printProcessing(an);
				_getch();
				addPrefix("cat", "meow");
				printProcessing();
				_getch();
				saveContentreverse();
				
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
				line += word + " ";
			}
			list<char> charList(line.begin(), line.end());
			paragraph.push_back(charList);
		}
	}


	// Processing Mode functions ----------------//
	// caseSensitive
	vector<position> findWord(string findWord, bool& isFound) {
		position defaultCase{ -1,-1 }; // -1 for not found
		vector<position> ans;
		int rowItr = 0;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				if (word == findWord) {
					position pos;
					pos.ri = rowItr;
					pos.ci = colItr;
					isFound = true;
					ans.push_back(pos);
				}
				colItr++;
			}
			rowItr++;
		}
		if (ans.size() == 0) {
			ans.push_back(defaultCase);
		}
		return ans;
	}
	int findSentence(string findSentence) {
		int row = 0;
		for (auto line : dummyParagraph) {
			string sentence;
			for (auto word : line) {
				sentence += word;
				sentence += " ";
			}
			if (!sentence.empty()) {
				sentence.pop_back();
			}

			if (sentence == findSentence) {
				return row;
			}
			row++;
		}
		return -1;
	}
	int averageWordlenght() {
		int nItr = 0, sum = 0;
		for (auto line : dummyParagraph) {
			for (auto word : line) {
				sum += word.size();
				nItr++;
			}
		}
		return sum / nItr;

	}
	int noOfSentences() {
		int countSentences = 0;
		for (auto line : dummyParagraph) {
			string sentence;
			for (auto word : line) {
				sentence += word;
				sentence += " ";
			}
			if (!sentence.empty()) {
				sentence.pop_back();
			}
			countSentences++;
		}
		return countSentences;
	}
	int largestWordlength() {
		int maxWordlength = 0;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				max(maxWordlength, word.size());
			}
		}
		return maxWordlength;
	}
	int smallestWordlength() {
		int minWordlength = 0;
		for (auto line : dummyParagraph) {
			int colItr = 0;
			for (auto word : line) {
				min(minWordlength, word.size());
			}
		}
		return minWordlength;
	}

	position findSubword(string subword) {
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

	// findWord Next 
	

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
				colIter++;
			}
			colIter = 0;
			rowIter++;
			cout << endl;
			
		}
	}
	void processingLinePrint(list<vector<string>>::iterator line,int row)const
	{
		system("color f0");
		system("cls");
		gotoRowCol(row, 0);
		SetClr(0, 15);
		for (auto itr = line->begin(); itr != line->end(); itr++)
		{
			cout << (*itr);
		}
	}
	void processingCleanLinePrint(list<vector<string>>::iterator line, int row)const
	{
		system("color f0");
		system("cls");
		gotoRowCol(row, 0);
		SetClr(0, 15);
		for (auto itr = line->begin(); itr != line->end(); itr++)
		{
			for (int i = 0; i < itr->size(); i++)
			{
				cout << " ";
			}
		}
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

	//processing Mode Main
	void processingMode()
	{
		saveContent();
		bool x = false;
		// Testing all form here //
		vector<position> an = findWord("cat", x);
		int w = findSentence("i am a very good cat");
		position w1 = findSubword("ver");
		int avg = averageWordlenght();
		int sp = specialCharacters();
		int sCount = noOfSentences();
		printProcessing(an);
		_getch();
		addPrefix("cat", "meow");
		printProcessing();
		_getch();
		saveContentreverse();
	}

	void movementInProcessing()//later will be placed in processing mode
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
				break;
			}
			else if (currButtonPressed == 117)//U pressed Upper case word
			{
				toUpperWord(currentWord);
				processingLinePrint(dummyLineNumberV2,pRow);
			}
			else if (currButtonPressed == 21)//CTRL+U pressed Upper case letter
			{
				toUpperLetter(currentWord);
				processingLinePrint(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 108)//L pressed Lower case word
			{
				toLowerWord(currentWord);
				processingLinePrint(dummyLineNumberV2, pRow);
			}
			else if (currButtonPressed == 12)//CTRL+L pressed lower case letter
			{
				toLowerLetter(currentWord);
				processingLinePrint(dummyLineNumberV2, pRow);
			}
			gotoRowCol(pRow, pCol);
		}

	}
};
 