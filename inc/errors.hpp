#ifndef HOLYC_ERRRORS_H
#define HOLYC_ERRRORS_H

#include <iostream>

namespace holyc{

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

}

#endif
