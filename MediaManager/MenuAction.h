#pragma once

/// <summary>
/// Interface for actions to be attached to a MenuItem.
/// </summary>
class MenuAction
{
public:
	virtual void Perform() = 0;
};

