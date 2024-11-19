#pragma once

class Object;
class SendBuffer;

class Component
{
public:
	virtual void Update(const float DT) = 0;
private:

};