#include "Tokens.h"
#ifndef _lambda_std_h
#define _lambda_std_h
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
using namespace std;
#endif

class AST
{
public:
	string *leftVal;
	string *rightVal;
	AST *leftAST;
	AST *rightAST;
	AST() :leftVal(NULL), rightVal(NULL), leftAST(NULL), rightAST(NULL){};
	~AST();
	AST(string *lv, string *rv, AST *la, AST *ra) ;
	AST(const AST &a);
	AST(Tokens **t);
	AST(string *s);
};


AST::~AST()
{
	if (leftVal != NULL)
	{
		delete leftVal;
	}
	if (rightVal != NULL)
	{
		delete rightVal;
	}
	if (leftAST != NULL)
	{
		delete leftAST;
	}
	if (rightAST != NULL)
	{
		delete rightAST;
	}
}

AST::AST(string *lv, string *rv, AST *la, AST *ra)
{
	if (lv != NULL)
	{
		leftVal = new string(*lv);
	}
	else
	{
		leftVal = NULL;
	}
	if (rv != NULL)
	{
		rightVal = new string(*rv);
	}
	else
	{
		rightVal = NULL;
	}

	if (la != NULL)
	{
		leftAST = new AST(*la);
	}
	else
	{
		leftAST = NULL;
	}
	if (ra != NULL)
	{
		rightAST = new AST(*ra);
	}
	else
	{
		rightAST = NULL;
	}
}

AST::AST(const AST &a)

{
	if (a.leftVal != NULL)
	{
		leftVal = new string(*a.leftVal);
	}
	else
	{
		leftVal = NULL;
	}

	if (a.rightVal != NULL)
	{
		rightVal = new string(*a.rightVal);
	}
	else
	{
		rightVal = NULL;
	}

	if (a.leftAST != NULL)
	{
		leftAST = new AST(*a.leftAST);
	}

	else
	{
		leftAST = NULL;
	}

	if (a.rightAST != NULL)
	{
		rightAST = new AST(*a.rightAST);
	}

	else
	{
		rightAST = NULL;
	}
}

AST::AST(Tokens **t)
{
	AST *ast = new AST();
	if (*(*t)->idString == "(")	//必须以左括号开头
	{
		//第一个元素		
		*t = (*t)->next;
		if (*(*t)->idString == "(")
		{
			ast->leftAST =new AST(t);
		}
		else
		{
			ast->leftVal = new string(*(*t)->idString);
		}
		//第二个元素	**TODO 未做元素个数检查，(a)则会有问题**
		*t = (*t)->next;
		if (*(*t)->idString == "(")
		{
			ast->rightAST = new AST(t);
		}
		else
		{
			ast->rightVal = new string(*(*t)->idString);
		}
		*t = (*t)->next;
		while (*(*t)->idString != ")")
		{
			if (*(*t)->idString == "(")
			{
				AST *astTmp = new AST(NULL, NULL, ast, new AST(t));
				ast = astTmp;
			}
			else
			{
				AST *astTmp = new AST(NULL, new string(*(*t)->idString), ast, NULL);
				ast = astTmp;
			}
			*t = (*t)->next;
		}
	}
	else //只有一个元素
	{

		ast->leftVal = new string(*(*t)->idString);
	}
	leftVal = ast->leftVal;
	rightVal = ast->rightVal;
	leftAST = ast->leftAST;
	rightAST = ast->rightAST;
}

AST::AST(string *s)
{
	Tokens *t = new Tokens(s);
	AST *ast = new AST(&t);
	delete t;
	leftVal = ast->leftVal;
	rightVal = ast->rightVal;
	leftAST = ast->leftAST;
	rightAST = ast->rightAST;
}