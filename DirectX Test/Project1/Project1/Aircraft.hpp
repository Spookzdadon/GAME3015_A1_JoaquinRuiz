#pragma once
#include "Entity.hpp"

class Aircraft :
	public Entity
{
public:
	enum Type
	{
		Eagle,
		Raptor,
	};

public:
	Aircraft(Type type, Game* game);

private:
	virtual void updateCurrent(const GameTimer& gt);
	virtual void drawCurrent() const;
	virtual void buildCurrent();

private:
	Type mType;
	std::string mSprite;
};