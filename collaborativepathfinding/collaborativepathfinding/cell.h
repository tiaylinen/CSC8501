#pragma once
class Cell
{
public:
	enum class Value { start, entrance, floor, wall, path, player };

	Cell();

	Cell(Value value);

	void setValue(Cell::Value value);
	Value getValue();

	void setPreValue(Cell::Value previousValue);
	Value getPreValue();

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
	Value previousValue;
	int row;
	int col;
	int distance;
	int id;
};

