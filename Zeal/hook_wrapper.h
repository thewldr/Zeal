#pragma once
#include "memory.h"
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "psapi.lib")
#define czVOID(c) (void)c
enum hook_type_
{
	hook_type_replace_call,
	hook_type_jmp,
	hook_type_detour
};
class hook
{
private:
	void replace_call(int addr, int dest);
	void replace(int addr, int dest);
	void detour(int addr, int dest);
	BYTE* original_bytes;
public: //methods
	void remove();
	~hook()
	{
		remove();
	}
	hook() : address{}, original_bytes{}, destination{}, trampoline{}, hook_type{ hook_type_detour } { };
	template<typename X, typename T>
	hook(X addr, T dest, hook_type_ hooktype = hook_type_detour, int byte_count = 5)
	{
		orig_byte_count = byte_count;
		original_bytes = new BYTE(byte_count);
		address = (int)addr;
		destination = (int)dest;
		hook_type = hooktype;
		mem::copy((int)original_bytes, (BYTE*)addr, byte_count);

		switch (hook_type)
		{
			case hook_type_detour:
			{
				detour((int)addr, (int)dest);
				break;
			}
			case hook_type_replace_call:
			{
				replace_call((int)addr, (int)dest);
				break;
			}
			case hook_type_jmp:
			{
				replace_call((int)addr, (int)dest);
				break;
			}
		}
	}
	template<typename T>
	T original(T fnType) {
		czVOID(fnType);
		return (T)trampoline;
	}
	void rehook();
public: //variables
	int destination;
	int address;
	int trampoline;
	int orig_byte_count;
	hook_type_ hook_type;
};
class HookWrapper
{
public:
	std::unordered_map<std::string, hook*> hook_map;
	template<typename X, typename T>
	hook* Add(std::string name, X addr, T fnc, hook_type_ type, int byte_count = 5)
	{
		hook* x = new hook(addr, fnc, type, byte_count);
		hook_map[name] = x;
		x->hook_type = type;
		return x;
	}
	~HookWrapper()
	{
		for (auto& hook : hook_map)
			hook.second->remove();
	}
};
