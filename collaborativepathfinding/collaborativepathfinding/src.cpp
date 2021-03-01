#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>
#include <unordered_map>
#include <queue>
#include "cell.h"
#include <thread>


using namespace std;

//set all entrance coordinates and store them as vectors
vector<vector<int>> set_exits(vector<vector<Cell>>& maze, int numOfEntrances)
{
	int eCoord1, eCoord2;
	vector<int> coordinates(2);
	vector<vector<int>> allEntrances(numOfEntrances, coordinates);
	for (int i = 0; i < numOfEntrances; i++)
	{
		eCoord1 = rand() % 2 == 0 ? 0 : maze.size() - 1;
		eCoord2 = rand() % (maze.size() - 2) + 1;
		allEntrances[i] = { eCoord1, eCoord2 };
		random_shuffle(allEntrances[i].begin(), allEntrances[i].end());
	}
	return allEntrances;
}




//generate the inital maze with room in the middle and start and exit points
vector<vector<Cell>> generate_maze(vector<vector<Cell>>& maze, vector<vector<int>> entrances)
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
				bool entrance = false;
				for (int k = 0; k < entrances.size(); k++)
				{
					if (i == entrances[k][0] && j == entrances[k][1]) { maze[i][j].setValue(Cell::Value::entrance); entrance = true; }
				}
				if (!entrance) { maze[i][j].setValue(Cell::Value::wall); }
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
			if (cells[i][j].getValue() == Cell::Value::entrance)
			{
				(i == 0) ? cells[i + 1][j].setValue(Cell::Value::floor) : (j == 0) ? cells[i][j + 1].setValue(Cell::Value::floor) : (j == cells.size() - 1) ?
					cells[i][j - 1].setValue(Cell::Value::floor) : cells[i - 1][j].setValue(Cell::Value::floor);
			}
			if (i > 1 && i < cells.size() - 2 && j > 1 && j < cells.size() - 2)
			{
				if (cells[i - 1][j].getValue() == Cell::Value::wall && cells[i][j - 1].getValue() == Cell::Value::wall) { cells[i - 1][j].setValue(Cell::Value::floor); }
				if (cells[i - 1][j].getValue() == Cell::Value::wall && cells[i][j + 1].getValue() == Cell::Value::wall) { cells[i][j + 1].setValue(Cell::Value::floor); }
				if (cells[i + 1][j].getValue() == Cell::Value::wall && cells[i][j - 1].getValue() == Cell::Value::wall) { cells[i + 1][j].setValue(Cell::Value::floor); }
				if (cells[i + 1][j].getValue() == Cell::Value::wall && cells[i][j + 1].getValue() == Cell::Value::wall) { cells[i][j + 1].setValue(Cell::Value::floor); }
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells.size(); j++)
		{
			cells[i][j].setPreValue(cells[i][j].getValue());
		}
	}
	return cells;
}




//add players to entrances
vector<vector<Cell>> enter_players(vector<vector<Cell>> cells) 
{
	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells.size(); j++)
		{
			if (cells[i][j].getValue() == Cell::Value::entrance) { cells[i][j].setValue(Cell::Value::player); }
		}
	}

	return cells;
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
			case Cell::Value::start: cout << "F"; break;
			case Cell::Value::entrance: cout << "E"; break;
			case Cell::Value::path: cout << "o"; break;
			case Cell::Value::player: cout << "P"; break;
			}
		}
		cout << "\n";
	}
}




//move player one step towards the finish point unless next cell is occupied by another player
vector<vector<Cell>> progress_player(vector<vector<Cell>> maze, int current, int predecessor)
{
	for (int i = 0; i < maze.size(); i++)
	{
		for (int j = 0; j < maze.size(); j++)
		{
			if (maze[i][j].getId() == predecessor) 
			{ 
				if (maze[i][j].getValue() == Cell::Value::player)
				{
					print_maze(maze);
					cout << "a player can't move" << endl;
					return maze;
				}
				maze[i][j].setValue(Cell::Value::player); 
			}
		}
	}

	for (int i = 0; i < maze.size(); i++)
	{
		for (int j = 0; j < maze.size(); j++)
		{
			if (maze[i][j].getId() == current) { maze[i][j].setValue(maze[i][j].getPreValue()); }
		}
	}

	print_maze(maze);
	return maze;

}



//BFS to find shortest path to the finishing point 
vector<vector<int>> find_path(vector<vector<Cell>>& maze, vector<vector<int>> entrances, int i)
{
	vector<bool> size(maze.size());
	vector<vector<bool>> visited(maze.size(), size);

	//mark all cells as unvisited
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

	//find entrance id
	int entrance = maze[entrances[i][0]][entrances[i][1]].getId();

	//instantiate a matrix which connects each cell to the previous cell by id
	vector<int> pair(2);
	vector<vector<int>> previous(q.size(), pair);
	previous.push_back({ start.getId(), -1 });

	while (!q.empty()) {
		Cell p = q.front();
		q.pop();

		if (p.getId() == entrance)
		{
			dist = p.getDistance();
		}

		// move up
		if (p.getRow() - 1 >= 0 && visited[p.getRow() - 1][p.getColumn()] == false)
		{
			Cell a = maze[p.getRow() - 1][p.getColumn()];
			a.setDistance(p.getDistance() + 1);
			q.push(a);
			visited[p.getRow() - 1][p.getColumn()] = true;
			previous.push_back({ a.getId(), p.getId() });
		}

		// move down 
		if (p.getRow() + 1 < maze.size() && visited[p.getRow() + 1][p.getColumn()] == false)
		{
			Cell b = maze[p.getRow() + 1][p.getColumn()];
			b.setDistance(p.getDistance() + 1);
			q.push(b);
			visited[p.getRow() + 1][p.getColumn()] = true;
			previous.push_back({ b.getId(), p.getId() });
		}

		// move left 
		if (p.getColumn() - 1 >= 0 && visited[p.getRow()][p.getColumn() - 1] == false)
		{
			Cell c = maze[p.getRow()][p.getColumn() - 1];
			c.setDistance(p.getDistance() + 1);
			q.push(c);
			visited[p.getRow()][p.getColumn() - 1] = true;
			previous.push_back({ c.getId(), p.getId() });
		}

		// move right 
		if (p.getColumn() + 1 < maze.size() && visited[p.getRow()][p.getColumn() + 1] == false)
		{
			Cell d = maze[p.getRow()][p.getColumn() + 1];
			d.setDistance(p.getDistance() + 1);
			q.push(d);
			visited[p.getRow()][p.getColumn() + 1] = true;
			previous.push_back({ d.getId(), p.getId() });
		}

	}
	return previous;
}



//save cell ids that are on each path
vector<int> save_path(vector<vector<Cell>> &maze, vector<vector<int>> previous, vector<vector<int>> entrances, int i)
{

	vector<int> path;

	int entrance = maze[entrances[i][0]][entrances[i][1]].getId();
	int middle = maze.size() / 2;
	Cell start = maze[middle][middle];

	vector<int> cells;
	for (int i = 0; i < previous.size(); i++)
	{
		cells.push_back(previous[i][0]);
	}

	auto index = find(cells.begin(), cells.end(), entrance);
	int cellIndex = index - cells.begin();

	int current = previous[cellIndex][0];
	int predecessor = previous[cellIndex][1];

	while (true)
	{
		if (current == start.getId()) { path.push_back(predecessor); break; }
		path.push_back(current);
		current = predecessor;
		auto index = find(cells.begin(), cells.end(), current);
		cellIndex = index - cells.begin();
		current = previous[cellIndex][0];
		predecessor = previous[cellIndex][1];
	}
	path.push_back(predecessor);

	return path;
}


vector<int> find_longest_path(vector<vector<int>> paths)
{
	vector<int> biggest = paths[0];
	for (int i = 0; i < paths.size()-1; i++)
	{
		if (paths[i+1].size() > paths[i].size()) { biggest = paths[i+1]; }
	}
	return biggest;
}


void save_file(string filename, vector<vector<Cell>> maze)
{
	ofstream outfile;
	outfile.open(filename + ".txt");		//open the file
	streambuf* coutbuf = cout.rdbuf();		//redirecting cout to the file
	cout.rdbuf(outfile.rdbuf());
	print_maze(maze);
	outfile.close();						//close the file
	cout.rdbuf(coutbuf);
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
		if (cin.good())
		{
			if (temp < 0) { cout << "size can't be negative!! " << endl; continue; }
			else { dimension = temp; cout << endl; }
		}
		else
		{
			cin.clear();
			cout << "not a valid size! please try again" << endl;
			cin.ignore();
			continue;
		}
		

		//ask user for the amount of players
	enterPlayers:
		cout << "enter the amount of players: ";
		int numOfPlayers;
		cin >> temp;
		if (temp < 0) { cout << "the amount of exits can't be negative!! " << endl; goto enterPlayers; }
		else { numOfPlayers = temp; cout << endl; }

		//create a matrix from user input and generate a maze of the right size
		vector<vector<Cell>> maze(dimension, vector<Cell>(dimension));
		vector<vector<int>> allEntrances = set_exits(maze, numOfPlayers);
		maze = edit_maze(generate_maze(maze, allEntrances));
		maze = enter_players(maze);
		print_maze(maze);


		//find exits
		cout << "enter -1 to quit the program or anything else to move the players: ";
		int input;
		cin >> input;
		if (input == -1) { break; }
		else { cin.ignore(); }
		vector<vector<int>> paths;
		for (int i = 0; i < allEntrances.size(); i++)
		{
			paths.push_back(save_path(maze, find_path(maze, allEntrances, i), allEntrances, i));
		}

		int biggest = find_longest_path(paths).size();
		for (int i = 0; i < biggest; i++)
		{
			for (int j = 0; j < paths.size(); j++)
			{
				if (i >= paths[j].size()-2) { continue; }
				maze = progress_player(maze, paths[j][i], paths[j][i + 1]);
			}
		}


		//saving the maze
		cout << "Wanna save the maze? choose file name: ";
		string filename;
		cin >> filename;
		thread th (save_file, filename, maze);
		th.join();


		//loading a maze
	loadfile:
		cout << "Let's load a maze. Choose a file to open: ";
		cin >> filename;
		ifstream inFile;
		inFile.open(filename + ".txt");
		if (inFile.is_open())
		{
			cout << inFile.rdbuf();
		}
		else
		{
			cout << "file not found, try again \n" << endl;
			goto loadfile;
		}
		inFile.close();

		cout << "go again? enter -1 to quit the program or anything else to re-run " << endl;
		int in;
		cin >> in;
		if (in == -1) { break; }
		else { cin.ignore(); }
	}
}