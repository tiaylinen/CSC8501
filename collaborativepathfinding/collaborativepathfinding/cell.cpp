#include "cell.h"

Cell::Cell()
{
}

Cell::Cell(Cell::Value v)
{
	value = v;
}


void Cell::setValue(Cell::Value v)
{
	value = v;
}

Cell::Value Cell::getValue()
{
	return value;
}

void Cell::setPreValue(Cell::Value pre)
{
	previousValue = pre;
}

Cell::Value Cell::getPreValue()
{
	return previousValue;
}

void Cell::setRow(int xn)
{
	row = xn;
}

int Cell::getRow()
{
	return row;
}

void Cell::setColumn(int yn)
{
	col = yn;
}

int Cell::getColumn()
{
	return col;
}

void Cell::setDistance(int dist)
{
	distance = dist;
}

int Cell::getDistance()
{
	return distance;
}

void Cell::setId(int i)
{
	id = i;
}

int Cell::getId()
{
	return id;
}