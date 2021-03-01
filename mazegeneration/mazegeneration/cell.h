#pragma once
class Cell
{
public:
	enum class Value { start, exit, floor, wall, path };

	Cell();

	Cell(Value value);

	void setValue(Cell::Value value);
	Value getValue();

	void setRow(int row);
	int getRow();

	void setColumn(int col);
	int getColumn();

	void setDistance(int distance);
	int getDistance();

	void setId(int id);
	int getId();

private:
	Value value;
	int row;
	int col;
	int distance;
	int id;
};

