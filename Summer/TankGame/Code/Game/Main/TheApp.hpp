#pragma once
#include "Engine/Core/Tools/Command.hpp"
#include <string>

class Blackboard;


class App
{

public:
	App();
	~App();
	void StartUp(); 
	void RunFrame(); // this tells everyone else to do a frame
	void Render() const;
	void Update();

	void LoadFromXML( const std::string& filePath, Blackboard& out_whereToStoreValues);

	float GetTimeSinceProgramStart() const {return m_timeSinceStart;} 

public:
	bool m_isQuitting;
	float m_timeSinceStart;
	
};

void RunThreadTest(Command& cb);
void ThreadTest();
void ReadBigBoi(void* data);
void BigTextTest(Command& cb);
void FlushTest(Command& cb);

extern App* g_theApp;
