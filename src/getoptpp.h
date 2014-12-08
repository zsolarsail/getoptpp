/*
 * Copyright (c) 2014, ZSolarsail, All rights reserved.
 *
 * This file is part of "libgetoptpp", which is distributed under the
 * terms of GNU Lesser General Public License (version 3) as 
 * published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
*/

#pragma once
#ifndef __LIB_GETOPTPP_H
#define __LIB_GETOPTPP_H 1

#include <string>
#include <vector>
#include <map>

namespace getoptpp {

using namespace std;

// ----------------------

// c[clong]::

enum ArgFlag {
    NO,
    REQUIRED,
    OPTIONAL
};


struct option {
    char s_name = 0;
    string l_name;
    ArgFlag flag = NO;
    string val;

    option() = default;
    
    option(char _s_name, const string &_l_name=string(), ArgFlag _flag=NO) :
	s_name(_s_name),
	l_name(_l_name),
	flag(_flag)
    {
    };
	
};

bool parse_option_string(vector<option> &vopt, const char *opt_str);

// ----------------------
class parser;


class parse_result {
friend class parser;
private:

    map<char, option*> ms;
    map<string, option*> ml;

    string tmp;
public:

    string error;
    vector<option> opt;

    int argc;
    vector<string> argv;

    parse_result() = default;
    parse_result(const parse_result&) = delete;
    void operator =(const parse_result&) = delete;



    const string& short_opt(char short_name) const
    {
	auto j = ms.find(short_name);
	return (j != ms.end()) ? j->second->val : tmp;
    };
    
    const string& operator[](char short_name) const { return short_opt(short_name); };


    const string& long_opt(const string& long_name) const
    {
	auto j = ml.find(long_name);
	return (j != ml.end()) ? j->second->val : tmp;
    };
    
    const string& operator[](const string& long_name) const { return long_opt(long_name); };

private:
    bool set_vopt(vector<option> &_vopt);
    
    void erase(void);
};


// ----------------------

class parser {
private:

    parse_result res;

    bool ok_ = false;

public:
    parser() = default;
    parser(const parser&) = delete;
    void operator =(const parser&) = delete;
    
    parser(vector<option> &vopt, int _argc, const void *_argv) { parse(vopt, _argc, _argv); };
    parser(const char *opt_str, int _argc, const void *_argv) { parse(opt_str, _argc, _argv); };

    bool parse(vector<option> &vopt, int _argc, const void*_argv);
    
    bool parse(const char *opt_str, int _argc, const void *_argv);

    const parse_result& result(void) const { return res; };
    const parse_result& operator()(void) const { return res; };
    
    
    bool ok(void) const { return ok_; };
    operator bool() const { return ok(); };

    void erase(void);
};



// ----------------------





}; // namespace getoptpp
#endif
