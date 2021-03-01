#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>
#include "cell.h"
#include <unordered_map>
#include <queue>


using namespace std;

//set all exit coordinates and store them as vectors
vector<vector<int>> set_exits(vector<vector<Cell>> &maze, int numOfExits)
{
	int exitCoord1, exitCoord2;
	vector<int> coordinates(2);
	vector<vector<int>> allExits(numOfExits, coordinates);
	for (int i = 0; i < numOfExits; i++)
	{
		exitCoord1 = rand() % 2 == 0 ? 0 : maze.size() - 1;
		exitCoord2 = rand() % (maze.size()-2) + 1;
		allExits[i] = { exitCoord1, exitCoord2 };
		random_shuffle(allExits[i].begin(), allExits[i].end());
	}
	return allExits;
}



//generate the inital maze with room in the middle and start and exit points
vector<vector<Cell>> generate_maze(vector<vector<Cell>> &maze, vector<vector<int>> exits)
{
	int middle = maze.size() / 2.0;

	for (int i = 0; i < maze.size(); i++)
	{
		for (int j = 0; j < maze.size(); j++)
		{
			if (i == middle && j == middle) { maze[i][j].setValue(Cell::Value::start); }
			else if (i - middle <= 1 && i - middle >= -1 && j - middle <= 1 && j - middle >= -1) { maze[i][j].setValue(Cell::Value::floor); }
			else if (i == 0 || j == 0 || i == maze.size() - 1 || j == maze.size() - 1) 
			{
				bool exit = false;
				for (int k = 0; k < exits.size(); k++)
				{
					if (i == exits[k][0] && j == exits[k][1]) { maze[i][j].setValue(Cell::Value::exit); exit = true; }
				}
				if (!exit) { maze[i][j].setValue(Cell::Value::wall); }
			}
			else { (rand() % 2) == true ? maze[i][j].setValue(Cell::Value::floor) : maze[i][j].setValue(Cell::Value::wall); }

		}
	}
	return maze;
}




//edit the initial maze to have a solvable structure 
vector<vector<Cell>> edit_maze(vector<vector<Cell>> cells)
{
	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells.size(); j++)
		{
			if (cells[i][j].getValue() == Cell::Value::exit)
			{
				(i == 0) ? cells[i + 1][j].setValue(Cell::Value::floor) : (j == 0) ? cells[i][j + 1].setValue(Cell::Value::floor) : (j == cells.size() - 1) ?
					cells[i][j - 1].setValue(Cell::Value::floor) : cells[i - 1][j].setValue(Cell::Value::floor);
			}
			if (i > 1 && i < cells.size() - 2 && j > 0 && j < cells.size() - 2)
			{
				if (cells[i - 1][j].getValue() == Cell::Value::wall && cells[i][j - 1].getValue() == Cell::Value::wall) { cells[i - 1][j].setValue(Cell::Value::floor); }
				if (cells[i - 1][j].getValue() == Cell::Value::wall && cells[i][j + 1].getValue() == Cell::Value::wall) { cells[i][j + 1].setValue(Cell::Value::floor); }
				if (cells[i + 1][j].getValue() == Cell::Value::wall && cells[i][j - 1].getValue() == Cell::Value::wall) { cells[i + 1][j].setValue(Cell::Value::floor); }
				if (cells[i + 1][j].getValue() == Cell::Value::wall && cells[i][j + 1].getValue() == Cell::Value::wall) { cells[i][j + 1].setValue(Cell::Value::floor); }
			}
		} 
	}
	return cells;
}





vector<vector<Cell>> findExit(vector<vector<Cell>> &maze, vector<vector<int>> exits, int i)
{
	vector<bool> size(maze.size());
	vector<vector<bool>> visited(maze.size(), size);

	//mark cells as unvisited
	for (int i = 0; i < maze.size(); i++)
	{
		for (int j = 0; j < maze.size(); j++)
		{
			visited[i][j] = (maze[i][j].getValue() != Cell::Value::wall) ? false : true;
		}
	}

	//assign cells rows, columns and ids
	int k = 0;
	for (int i = 0; i < maze.size(); i++)
	{
		for (int j = 0; j < maze.size(); j++)
		{
			maze[i][j].setRow(i);
			maze[i][j].setColumn(j);
			maze[i][j].setId(k++);
		}
	}

	//find start point and instantiate queue with cells to visit
	int middle = maze.size() / 2;
	queue<Cell> q;
	Cell start = maze[middle][middle];
	start.setDistance(0);
	q.push(start);
	visited[start.getRow()][start.getColumn()] = true;
	int dist = start.getDistance();

	//set exit
	int exit = maze[exits[i][0]][exits[i][1]].getId();

	//instantiate a matrix which connects each cell to the previous cell by id
	vector<int> pair(2);
	vector<vector<int>> previous(q.size(), pair);
	previous.push_back({ start.getId(), -1 });

	while (!q.empty()) {
		Cell p = q.front();
		q.pop();
		
		if (p.getId() == exit)
		{
			dist = p.getDistance();
		}

		// move up
		if (p.getRow()-1 >= 0 && visited[p.getRow() - 1][p.getColumn()] == false)
		{
			Cell a = maze[p.getRow() - 1][p.getColumn()];
			a.setDistance(p.getDistance() + 1);
			q.push(a);
			visited[p.getRow() - 1][p.getColumn()] = true;
			previous.push_back({ a.getId(), p.getId() });
		}

		// move down 
		if (p.getRow()+1 < maze.size() && visited[p.getRow() + 1][p.getColumn()] == false)
		{
			Cell b = maze[p.getRow() + 1][p.getColumn()];
			b.setDistance(p.getDistance() + 1);
			q.push(b);
			visited[p.getRow() + 1][p.getColumn()] = true;
			previous.push_back({ b.getId(), p.getId() });
		}

		// move left 
		if (p.getColumn()-1 >= 0 && visited[p.getRow()][p.getColumn() - 1] == false)
		{
			Cell c = maze[p.getRow()][p.getColumn()-1];
			c.setDistance(p.getDistance() + 1);
			q.push(c);
			visited[p.getRow()][p.getColumn() - 1] = true;
			previous.push_back({ c.getId(), p.getId() });
		}

		// move right 
		if (p.getColumn()+1 < maze.size() && visited[p.getRow()][p.getColumn() + 1] == false)
		{
			Cell d = maze[p.getRow()][p.getColumn() + 1];
			d.setDistance(p.getDistance() + 1);
			q.push(d);
			visited[p.getRow()][p.getColumn() + 1] = true;
			previous.push_back({ d.getId(), p.getId() });
		}

	}

	vector<int> cells;
	for (int i = 0; i < previous.size(); i++)
	{
		cells.push_back(previous[i][0]);
	}

	auto index = find(cells.begin(), cells.end(), exit);
	int cellIndex = index - cells.begin();

	int current = previous[cellIndex][0];
	int predecessor = previous[cellIndex][1];


	while (true)
	{
		if (predecessor == start.getId()) { break; }
		for (int i = 0; i < maze.size(); i++)
		{
			for (int j = 0; j < maze.size(); j++)
			{
				if (maze[i][j].getId() == predecessor) { maze[i][j].setValue(Cell::Value::path); }
			}
		}
		current = predecessor;
		auto index = find(cells.begin(), cells.end(), current);
		cellIndex = index - cells.begin();
		current = previous[cellIndex][0];
		predecessor = previous[cellIndex][1];
	}

	return maze;
}





//draw the final maze
void print_maze(vector<vector<Cell>> maze)
{
	for (int i = 0; i < maze.size(); i++)
	{
		for (int j = 0; j < maze.size(); j++)
		{
			switch (maze[i][j].getValue())
			{
			case Cell::Value::floor: cout << " "; break;
			case Cell::Value::wall: cout << "X"; break;
			case Cell::Value::start: cout << "S"; break;
			case Cell::Value::exit: cout << "E"; break;
			case Cell::Value::path: cout << "o"; break;
			}
		}
		cout << "\n";
	}
}



int main()
{
	srand(time(NULL));

	while (true)
	{
		//ask user for size and handle input
		cout << "enter maze size: ";
		int dimension;
		int temp;
		cin >> temp;
		if (temp < 0) { cout << "size can't be negative!! " << endl; continue; }
		else { dimension = temp; cout << endl; }

		//ask user for the amount of exits
		enterExits:
		cout << "enter the amount of exits: ";
		int numOfExits;
		cin >> temp;
		if (temp < 0) { cout << "the amount of exits can't be negative!! " << endl; goto enterExits; }
		else { numOfExits = temp; cout << endl; }

		//create a matrix from user input and generate a maze of the right size
		vector<vector<Cell>> maze(dimension, vector<Cell>(dimension));
		vector<vector<int>> allExits = set_exits(maze, numOfExits);
		maze = edit_maze(generate_maze(maze, allExits));
		print_maze(maze);



		//find exits
		cout << "enter anything to find the exits " << endl;
		int input;
		cin >> input;
		cin.ignore();
		for (int i = 0; i < allExits.size(); i++) {
			maze = findExit(maze, allExits, i);
		}
		print_maze(maze);



		//saving the maze
		cout << "let's save the maze. choose file name: " << endl;
		string filename;
		cin >> filename;
		ofstream outfile;
		outfile.open(filename + ".txt");		//open the file
		streambuf* coutbuf = cout.rdbuf();	//redirecting cout to the file
		cout.rdbuf(outfile.rdbuf());
		print_maze(maze);
		outfile.close();						//close the file
		cout.rdbuf(coutbuf);
		

		//load a maze
		cout << "let's open a maze from a file. type in a file to open: " << endl;
		cin >> filename;
		ifstream inFile;
		inFile.open(filename + ".txt");
		if (inFile.is_open()) { cout << inFile.rdbuf(); }

		cout << "enter -1 to exit or anything else to re-run " << endl;
		int cont;
		cin >> cont;
		if (cont == -1)
		{
			cout << "bye!"; break;
		}
		else 
		{
			cin.ignore();
		}
		
	}
}