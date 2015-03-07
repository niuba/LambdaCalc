#include "AST.h"
#ifndef _lambda_std_h
#define _lambda_std_h
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
using namespace std;
#endif

#define ExpLambda -1
#define ExpExp -2
#define ExpError -3

map<string, int> deBruijn({ { "a", 0 }, { "b", 1 }, { "c", 2 }, { "d", 3 }, { "e", 4 }, { "f", 5 }, { "g", 6 }, { "h", 7 }, { "i", 8 }, { "j", 9 }, { "k", 10 }, { "l", 11 }, { "m", 12 }, { "n", 13 }, { "o", 14 }, { "p", 15 }, { "q", 16 }, { "r", 17 }, { "s", 18 }, { "t", 19 }, { "u", 20 }, { "v", 21 }, { "w", 22 }, { "x", 23 }, { "y", 24 }, { "z", 25 } });	//默认上下文索引
map<int, string> njiurBed({ { 0, "a" }, { 1, "b" }, { 2, "c" }, { 3, "d" }, { 4, "e" }, { 5, "f" }, { 6, "g" }, { 7, "h" }, { 8, "i" }, { 9, "j" }, { 10, "k" }, { 11, "l" }, { 12, "m" }, { 13, "n" }, { 14, "o" }, { 15, "p" }, { 16, "q" }, { 17, "r" }, { 18, "s" }, { 19, "t" }, { 20, "u" }, { 21, "v" }, { 22, "w" }, { 23, "x" }, { 24, "y" }, { 25, "z" } });


class Expression
{
public:
	int id;
	Expression *e1;
	Expression *e2;
	Expression() :id(ExpError), e1(NULL), e2(NULL){};
	~Expression();
	Expression(int i, Expression *exp1, Expression *exp2);
	Expression(const Expression &exp);
	Expression(AST *a);
	Expression(Tokens *t);
	Expression(string *s);
	void expressionShift(int d,int c);
	void replace(int j, Expression *s);
	void betaReduction();
private:
	Expression* findInDeBruijnTmp(string s, map<string, int> *deBruijnTmp);
	Expression* Expression::buildExpression(AST *ast, map<string, int> deBruijnTmp);
	void deBruijnTmpChange(int k, map<string, int> *deBruijnTmp);
};

Expression::~Expression()
{
	if (e1 != NULL)
	{
		delete e1;
	}
	if (e2 != NULL)
	{
		delete e2;
	}
};

Expression::Expression(int i, Expression *exp1, Expression *exp2)
{
	id = i;
	if (exp1 != NULL)
	{
		e1 = new Expression(*exp1);
	}
	else
	{
		e1 = NULL;
	}
	if (exp2 != NULL)
	{
		e2 = new Expression(*exp2);
	}
	else
	{
		e2 = NULL;
	}
};

Expression::Expression(const Expression &exp)
{
	id = exp.id;
	if (exp.e1 != NULL)
	{
		e1 = new Expression(*exp.e1);
	}
	else
	{
		e1 = NULL;
	}
	if (exp.e2 != NULL)
	{
		e2 = new Expression(*exp.e2);
	}
	else
	{
		e2 = NULL;
	}

};

Expression* Expression::findInDeBruijnTmp(string s, map<string, int> *deBruijnTmp)
{
	auto k = deBruijnTmp->find(s);
	if (k != deBruijnTmp->end())
	{
		return new Expression((*deBruijnTmp)[s], NULL, NULL);
	}
	k = deBruijn.find(s);
	if (k != deBruijn.end())
	{
		return new Expression(deBruijn[s] + deBruijnTmp->size(), NULL, NULL);
	}
	else
	{
		int size = deBruijn.size();
		deBruijn[s] = size;
		njiurBed[size] = s;
		return new Expression(deBruijn[s] + deBruijnTmp->size(), NULL, NULL);
	}
}
void Expression::deBruijnTmpChange(int k, map<string, int> *deBruijnTmp)
{
	for (auto &each : *deBruijnTmp)
	{
		each.second += k;
	}
}
Expression* Expression::buildExpression(AST *ast, map<string, int> deBruijnTmp)
//构建表达式对象
{
	if (ast->leftVal != NULL)
	{
		// (x (y|E|NULL))
		string val = *ast->leftVal;
		if (ast->rightVal != NULL)
		{
			// (x y)
			return new Expression(ExpExp,
				findInDeBruijnTmp(val, &deBruijnTmp),
				findInDeBruijnTmp(*ast->rightVal, &deBruijnTmp));
		}
		else if (ast->rightAST != NULL)
		{
			// (x E)
			return new Expression(ExpExp,
				findInDeBruijnTmp(val, &deBruijnTmp),
				buildExpression(ast->rightAST, deBruijnTmp));
		}
		else
		{
			// x
			return findInDeBruijnTmp(val, &deBruijnTmp);
		}
	}
	else if ((ast->leftAST->leftVal != NULL) && (ast->leftAST->rightVal != NULL))
	{
		if (*ast->leftAST->leftVal == "lambda")
		{
			// lambda
			string val = *ast->leftAST->rightVal;
			deBruijnTmpChange(1, &deBruijnTmp);
			deBruijnTmp[val] = 0;
			if (ast->rightVal != NULL)
			{
				//(lambda k p)
				return new Expression(ExpLambda,
					findInDeBruijnTmp(*ast->rightVal, &deBruijnTmp), NULL);
			}
			else if (ast->rightAST != NULL)
			{
				//(lambda k E)
				return new Expression(ExpLambda, 
					buildExpression(ast->rightAST, deBruijnTmp), NULL);
			}
		}
		else
		{
			if (ast->rightAST != NULL)
			{
				// ((x y) E)
				return new Expression(ExpExp,
						new Expression(ExpExp,
							findInDeBruijnTmp(*ast->leftAST->leftVal, &deBruijnTmp),
							findInDeBruijnTmp(*ast->leftAST->rightVal, &deBruijnTmp)),
						buildExpression(ast->rightAST, deBruijnTmp));
			}
			else if (ast->rightVal != NULL)
			{
				// ((x y) x)
				return new Expression(ExpExp, 
						new Expression(ExpExp, 
							findInDeBruijnTmp(*ast->leftAST->leftVal, &deBruijnTmp),
							findInDeBruijnTmp(*ast->leftAST->rightVal, &deBruijnTmp)),
						findInDeBruijnTmp(*ast->rightVal, &deBruijnTmp));
			}
		}
	}
	else if (ast->rightAST != NULL)
	{
		// (E E)
		return new Expression(ExpExp,
			buildExpression(ast->leftAST, deBruijnTmp),
			buildExpression(ast->rightAST, deBruijnTmp));
	}
	else if (ast->rightVal != NULL)
	{
		// (E x)
		return new Expression(ExpExp, 
			buildExpression(ast->leftAST, deBruijnTmp),
			findInDeBruijnTmp(*ast->rightVal, &deBruijnTmp));
	}
}
Expression::Expression(AST *a)
//构建表达式对象
{
	map<string, int> deBruijnTmp;
	deBruijnTmp.clear();
	Expression *exp = buildExpression(a, deBruijnTmp);
	e1 = exp->e1;
	e2 = exp->e2;
	id = exp->id;
}

Expression::Expression(Tokens *t)
{
	AST *a = new AST(&t);
	Expression *exp = new Expression(a);
	delete a;
	e1 = exp->e1;
	e2 = exp->e2;
	id = exp->id;
}

Expression::Expression(string *s)
{
	Tokens *t = new Tokens(s);
	Expression *exp = new Expression(t);
	delete t;
	e1 = exp->e1;
	e2 = exp->e2;
	id = exp->id;
}
void Expression::expressionShift(int d, int c)
{
	Expression *exp = this;
	if (exp->id >-1)
	{
		if (exp->id >= c)
		{
			exp->id += d;
		}
	}
	else if (exp->id == ExpLambda)
	{
		exp->e1->expressionShift(d, c + 1);
	}
	else if (exp->id== ExpExp)
	{
		exp->e1->expressionShift(d, c);
		exp->e2->expressionShift(d, c);
	}
}

void Expression::replace(int j, Expression *s)
{
	Expression *exp = this;
	if (exp->id >-1)
	{
		if (exp->id == j)
		{
			exp->~Expression();
			*exp = *new Expression(*s);
		}
	}
	else if (exp->id == ExpLambda)
	{
		Expression *sTmp = new Expression(*s);
		sTmp->expressionShift(1, 0);
		exp->e1->replace(j + 1, sTmp);
		sTmp->~Expression();
	}
	else if (exp->id== ExpExp)
	{
		exp->e1->replace(j, s);
		exp->e2->replace(j, s);
	}
}

void Expression::betaReduction()
{
	Expression *exp = this;
	if (exp->id == ExpExp)
	{
		if (exp->e1->id == ExpLambda)
		{
			exp->e2->expressionShift(1, 0);
			exp->e1->e1->replace(0, exp->e2);
			exp->e1->e1->expressionShift(-1, 0);
			Expression *expTmp = new Expression(*exp->e1->e1);
			exp->~Expression();
			*exp = *expTmp;
		}
	}
}


