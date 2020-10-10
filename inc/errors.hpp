#ifndef NEGATRON_ERRRORS_H
#define NEGATRON_ERRRORS_H

#define EXPAND2(x) #x
#define EXPAND1(x) EXPAND2(x)
#define CODELOC __FILE__ ":" EXPAND1(__LINE__) " - "
#define TODO(x) throw new ToDoError(CODELOC #x);

#include <iostream>

namespace holeyc{

class InternalError{
public:
	InternalError(const char * msgIn) : myMsg(msgIn){}
	std::string msg(){ return myMsg; }
private:
	std::string myMsg;
};

class ToDoError{
public:
	ToDoError(const char * msgIn) : myMsg(msgIn){}
	const char * msg(){ return myMsg; }
private:
	const char * myMsg;
};

class Report{
public:
	static void fatal(
		size_t l, 
		size_t c, 
		const char * msg
	){
		std::cerr << "FATAL [" << l << "," << c << "]: " 
		<< msg  << std::endl;
	}

	static void fatal(
		size_t l, 
		size_t c, 
		const std::string msg
	){
		fatal(l,c,msg.c_str());
	}

	static void warn(
		size_t l,
		size_t c,
		const char * msg
	){
		std::cerr << "*WARNING* [" << l << "," << c << "]: " 
		<< msg  << std::endl;
	}

	static void warn(
		size_t l,
		size_t c,
		const std::string msg
	){
		warn(l,c,msg.c_str());
	}
};

}

#endif
