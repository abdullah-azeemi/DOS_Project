#pragma once
#include"Utility.h"

class textEditor
{
	list<list<char>> paragraph;
	list<list<char>>::iterator lineNumber;
	list<char>::iterator currLetter;

	//multiple files 
	list<string> openedFiles;
	list<string>::iterator currOpenedFile;

	//cursor
	int cursorRow;
	int cursorCol;

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
		if(lineNumber->size()!=0)
			advance(currLetter, cursorCol-1);
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
		int limit = traverse->size()+2;
		gotoRowCol(row, 0);
		for (int i = 0; i < limit; i++)
		{
			cout<<" ";
		}
		gotoRowCol(cursorRow, cursorCol);
	}

	void displayLine(const list<char>& line)const
	{
		clean(cursorRow);
		gotoRowCol(cursorRow, 0);
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

	void spliceHelper(list<char>& sender, list<char>& receiver,const int& combineAt)
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

	void loadFile(string fileName)
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
				lineNumber->push_back(line[i]);
			}
			line.clear();
		}
		currLetter = lineNumber->begin();
		cursorRow = paragraph.size()-1;
		cursorCol = lineNumber->size();
		if(cursorCol!=0)//boundary check as advance also decrements
			advance(currLetter, cursorCol-1);
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
	int countSentences()const
	{
		int sentence = 0;
		for (auto rItr = paragraph.begin(); rItr != paragraph.end(); rItr++)
		{
			for (auto cItr = (*rItr).begin(); cItr != (*rItr).end(); cItr++)
			{
				if ((*cItr) == '.' || (*cItr) == '?' || (*cItr) == '!')
				{
					sentence++;
				}
			}
		}
		return sentence;
	}

public:
	
	textEditor() :paragraph({ list<char>() }), cursorRow(0), cursorCol(0)
	{
		lineNumber = paragraph.begin();
	}

	void editFile(string fileName,bool load=false)
	{
		this->openedFiles.push_back(fileName);//add name to opened files
		//set color to white
		system("color f0");
		system("cls");
		if (load)
		{
			loadFile(fileName);
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
					if(old!=0)
						advance(currLetter, cursorCol-1);
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
			else//data entry
			{
				emptyRedoQueue();
				saveState(undo);
				if (lineNumber->empty() || (cursorCol==0 && lineNumber->size()!=0))//insert at front
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
};
