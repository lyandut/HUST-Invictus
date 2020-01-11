//
// Created by liyan on 2019/10/29.
//

#ifndef MYMAXSAT_MAXSATEXCEPTION_HPP
#define MYMAXSAT_MAXSATEXCEPTION_HPP

#include <exception>

class MaxSATException : public std::exception {
public:
	MaxSATException() {}
	const char * what() const throw () {
		return "MaxSAT Exception";
	}

private:

};

#endif //MYMAXSAT_MAXSATEXCEPTION_HPP