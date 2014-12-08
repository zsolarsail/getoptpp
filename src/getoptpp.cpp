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

#include <stdio.h>

#include <cctype>
#include <cstring>

#include "getoptpp.h"

namespace getoptpp {

const string PRESENT = "<PRESENT>";

// ----------------------

bool parse_option_string(vector<option> &vopt, const char *opt_str)
{
    char s_name;
    string l_name;
    ArgFlag flag;
    string val;

    const char *p = opt_str;
    const char *q;

    while(1)
    {
	s_name = 0;
	flag = NO;
	l_name.erase();
	val.erase();
	
	while(isspace(*p))
	    p++;
	
	if(!*p)
	    break;
	
	if(isalnum(*p))
	    s_name = *p++;
	
	if(*p == '[')
	{
	    if(!(q = strchr(++p, ']')))
		return false;
	
	    l_name.append(p, q-p);
	    if(l_name.empty())
		return false;
	
	    p = q + 1;
	};

	if(!*p)
	    break;

	if(*p == ':')
	{
	    flag = REQUIRED;
	    if(*++p == ':')
	    {
		flag = OPTIONAL;
		p++;
	    };
	};
	
	
	if(s_name || !l_name.empty())
	    vopt.emplace_back(s_name, l_name, flag);
	else
	    return false;
    };


    if(s_name || !l_name.empty())
	vopt.emplace_back(s_name, l_name, flag);

    return true;
};


// ----------------------

void parse_result::erase(void)
{
    opt.clear();
    ms.clear();
    ml.clear();
    argc = 0;
    argv.clear();
    error.erase();
};

bool parse_result::set_vopt(vector<option> &_vopt)
{
    erase();
    opt = std::move(_vopt);
    
    for(auto &x: opt)
    {
	if(ms.find(x.s_name) != ms.end())
	{
	    error = "option dupplicate '" + x.s_name;
	    error += '\'';
	    return false;
	};
	
	ms[x.s_name] = &x;

	if(ml.find(x.l_name) != ml.end())
	{
	    error = "option dupplicate '" + x.l_name;
	    error += '\'';
	    return false;
	};
	
	ml[x.l_name] = &x;
    };
    
    return true;
};

// ----------------------

bool parser::parse(vector<option> &vopt, int _argc, const void*_argv)
{
    const char **__argv = (const char**)_argv;
    erase();
    
    if(!res.set_vopt(vopt))
	return false;

    option *opt = 0;
    const char *p, *q;
    string l_name, val;

    bool wait_val = false;
    bool wait_optional = false;

    for(int i=0; i<_argc; i++)
    {
	const char *a = __argv[i];
	p = a;
	
	if(wait_val || wait_optional)
	{
	    wait_val = wait_optional = false;
	    opt->val = p;
	    continue;
	};


	if(*p != '-')
	{
	    res.argc++;
	    res.argv.push_back(string(p));
	    continue;
	};


	if(!*++p)
	{
	    res.argc++;
	    res.argv.push_back(string("-"));
	    continue;
	};
	
	val.erase();
	
	if(*p == '-')
	{
	    l_name.erase();
	    q = strchr(++p, '=');
	    if(q)
	    {
		l_name.append(p, q-p);
		val = q+1;
	    }
	    else
		l_name = p;
	    
	    if(l_name.empty())
	    {
		res.error = "empty long option";
		return false;
	    };
	    
	    auto j = res.ml.find(l_name);
	    if(j == res.ml.end())
	    {
		res.error = "bad option '";
		res.error += p-2;
		res.error += '\'';
		return false;
	    };
	    
	    opt = j->second;
	    
	    if(opt->flag == NO)
	    {
		if(q)
		{
		    res.error = "value isn't required '";
		    res.error += p-2;
		    res.error += '\'';
		    return false;
		};
		
		opt->val = PRESENT;
		continue;
	    };
	    
	    if(opt->flag == REQUIRED)
	    {
		if(!val.empty())
		    opt->val = val;
		else
		    wait_val = true;
		
		continue;
	    };
	    
	    if(!val.empty())
		opt->val = val;
	    else
	    {
		opt->val = PRESENT;
		wait_optional = true;
	    };
	
	    continue;
	}
	else
	{
	    char c;
	    while((c = *p++))
	    {
		auto j = res.ms.find(c);
		if(j == res.ms.end())
		{
		    res.error = "bad option '";
		    res.error += c;
		    res.error += '\'';
		    return false;
		};
		
		opt = j->second;
		
		if(opt->flag == NO)
		{
		    opt->val = PRESENT;
		    continue;
		};
		
		if(opt->flag == REQUIRED)
		{
		    if(*p)
			opt->val = p;
		    else
			wait_val = true;
		
		    break;
		};
		
		if(*p)
		    opt->val = p;
		else
		{
		    opt->val = PRESENT;
		    wait_optional = true;
		};
		
	    };
	};
    };

    if(wait_val)
    {
	res.error = "missed value: '";
	if(opt->s_name)
	{
	    res.error += opt->s_name;
	    res.error += '\'';
	    if(!opt->l_name.empty())
		res.error += ", '";
	};
	
	if(!opt->l_name.empty())
	{
	    res.error += opt->l_name;
	    res.error += '\'';
	};
	return false;
    };


    ok_ = true;
    return ok_;
};

// ----------------------

bool parser::parse(const char *opt_str, int _argc, const void *_argv)
{
    erase();
    vector<option> vopt;

    return parse_option_string(vopt, opt_str) ? parse(vopt, _argc, _argv) : false;
};

// ----------------------

void parser::erase(void)
{
    res.erase();
    ok_ = false;
};

// ----------------------
// ----------------------
// ----------------------
// ----------------------







}; // namespace getoptpp

