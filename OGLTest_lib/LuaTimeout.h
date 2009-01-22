#include <string>
struct lua_State;
struct SDL_Thread;

class LuaTimeout
{
public:
	LuaTimeout(std::string _message, float _timeout);
	~LuaTimeout();
	/*
	 * Resumes the lua coroutine and returns the error code.
	 */
	int SafeResume(lua_State* coroutine);

private:
	int call_id; //Incremented with each call
	bool active;
	float timeout_;
	lua_State* luaVM;
	SDL_Thread* thread; //The thread that checks for timeouts
	std::string abort_message; //This is written to Logger:ErrorOut() when a timeout occurs
	static int ExecutionMonitor(void* param);
};


