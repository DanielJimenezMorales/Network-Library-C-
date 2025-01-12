#pragma once

class IInputController;
class ICursor;

struct InputComponent
{
public:
	InputComponent(IInputController* inputController, ICursor* cursor) : inputController(inputController), cursor(cursor) {};

	const IInputController* inputController;
	const ICursor* cursor;
};
