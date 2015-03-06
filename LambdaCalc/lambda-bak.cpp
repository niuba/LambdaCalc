#include <string>
#include <vector>
#include <iostream>
#include <map>
using namespace std;

enum TokenType { id, leftBracket, rightBracket, blankSpace, lambda, err, define, tokenEnd, tokenExp };
class Token
{
public:
	TokenType tokenType;
	string idString;
	Token(TokenType type, string s) :tokenType(type), idString(s){};
};
vector<Token> tokenize(string s)
{
	vector<Token> tokens;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == '(')
		{
			tokens.push_back(Token(leftBracket, ""));
		}
		else if (s[i] == ')')
		{
			tokens.push_back(Token(rightBracket, ""));
		}
		else if (s[i] == ' ')
		{
			continue;
		}
		else if (s.substr(i, 6) == "lambda")
		{
			tokens.push_back(Token(lambda, ""));
			i += 5;
		}
		else if (s.substr(i, 6) == "define")
		{
			tokens.push_back(Token(define, ""));
			i += 5;
		}
		else
		{
			int k = i;
			while ((s[k + 1] != ' ') && (s[k + 1] != '(') && (s[k + 1] != ')'))
			{
				if (k == s.size())
				{
					tokens.push_back(Token(err, ""));
					i = k;
					break;
				}
				else
				{
					k++;
				}
			}
			tokens.push_back(Token(id, s.substr(i, k - i + 1)));
			i = k;
		}
	}
	return tokens;
}
enum ExpressionType{ Expid, Explambda, Expdefine, ExpExp };
class Expression
{
public:
	ExpressionType expressionType;
	string idString;
	Expression *e1;
	Expression *e2;
	Expression() = default;
	Expression(ExpressionType type, string id, Expression *exp1, Expression *exp2) :expressionType(type), idString(id)
	{
		if (exp1 != NULL)
		{
			e1 = new Expression(*exp1);
		}
		if (exp2 != NULL)
		{
			e2 = new Expression(*exp2);
		}
	};
	Expression(const Expression &exp) :expressionType(exp.expressionType), idString(exp.idString)
	{
		if (exp.e1 != NULL)
		{
			e1 = new Expression(*exp.e1);
		}
		if (exp.e2 != NULL)
		{
			e2 = new Expression(*exp.e2);
		}

	};
	~Expression()
	{
		delete e1;
		delete e2;
	};
};
map<string, Expression*> evn;
void reduction(Expression *exp);
void prettyPrint(Expression *exp);
vector<Token>::iterator findEndToken(vector<Token>::iterator beginToken)
{
	if (beginToken->tokenType == leftBracket)
	{
		vector<Token>::iterator nextEndToken;
		nextEndToken = beginToken;
		int stack = 0;
		stack++;
		while (stack != 0)
		{
			nextEndToken++;
			if (nextEndToken->tokenType == rightBracket)
			{
				stack--;
			}
			else if (nextEndToken->tokenType == leftBracket)
			{
				stack++;
			}
		}
		return nextEndToken;
	}
}
Expression *parser(vector<Token>::iterator beginToken, vector<Token>::iterator endToken)
{
	Expression *exp;
	if (beginToken->tokenType == leftBracket)
	{
		beginToken++;
		if (beginToken->tokenType == lambda)
		{
			beginToken++;
			if (beginToken->tokenType == id)
			{
				if ((beginToken + 1)->tokenType == id)
				{
					return new Expression(Explambda, beginToken->idString, new Expression(Expid, (beginToken + 1)->idString, NULL, NULL), NULL);
				}
				else
				{
					vector<Token>::iterator nextEndToken;
					nextEndToken = findEndToken(beginToken + 1);
					return new Expression(Explambda, beginToken->idString, parser(beginToken + 1, nextEndToken), NULL);
				}
			}
		}
		else if (beginToken->tokenType == define)
		{
			beginToken++;
			if (beginToken->tokenType == id)
			{
				if ((beginToken + 1)->tokenType == id)
				{
					return new Expression(Expdefine, beginToken->idString, new Expression(Expid, (beginToken + 1)->idString, NULL, NULL), NULL);
				}
				else
				{
					vector<Token>::iterator nextEndToken;
					nextEndToken = findEndToken(beginToken + 1);
					return new Expression(Expdefine, beginToken->idString, parser(beginToken + 1, nextEndToken), NULL);
				}
			}
		}
		else
		{
			if (beginToken->tokenType == id)
			{
				if ((beginToken + 1)->tokenType == id)
				{
					return new Expression(ExpExp, "", new Expression(Expid, beginToken->idString, NULL, NULL), new Expression(Expid, (beginToken + 1)->idString, NULL, NULL));
				}
				else
				{
					vector<Token>::iterator nextEndToken;
					nextEndToken = findEndToken(beginToken + 1);
					return new Expression(ExpExp, "", new Expression(Expid, beginToken->idString, NULL, NULL), parser(beginToken + 1, nextEndToken));
				}
			}
			else
			{
				vector<Token>::iterator nextEndToken;
				nextEndToken = findEndToken(beginToken);
				if ((nextEndToken + 1)->tokenType == id)
				{
					return new Expression(ExpExp, "", parser(beginToken, nextEndToken), new Expression(Expid, (nextEndToken + 1)->idString, NULL, NULL));
				}
				else
				{
					vector<Token>::iterator nextEndToken1;
					nextEndToken1 = findEndToken(nextEndToken + 1);
					return new Expression(ExpExp, "", parser(beginToken, nextEndToken), parser(nextEndToken + 1, nextEndToken1));
				}
			}
		}
	}
}



void prettyPrint(Expression *exp)
{
	if (exp->expressionType == Explambda)
	{
		cout << "(lambda " << exp->idString << ' ';
		prettyPrint(exp->e1);
		cout << ')';
	}
	else if (exp->expressionType == ExpExp)
	{
		cout << "(";
		prettyPrint(exp->e1);
		cout << " ";
		prettyPrint(exp->e2);
		cout << ')';
	}
	else if (exp->expressionType == Expid)
	{
		cout << exp->idString;
	}
}

void replace(Expression *exp1, string s, Expression *exp2)
{
	if (exp1->expressionType == Explambda)
	{
		if (exp1->idString != s)
		{
			replace(exp1->e1, s, exp2);
		}
	}
	else if (exp1->expressionType == ExpExp)
	{
		replace(exp1->e1, s, exp2);
		replace(exp1->e2, s, exp2);
	}
	else if (exp1->expressionType == Expid)
	{
		if (exp1->idString == s)
		{
			exp1->~Expression();
			*exp1 = *new Expression(*exp2);
		}
	}
}
void alphaReduction(Expression *exp1, Expression *exp2)
{
	if (exp1->expressionType == Explambda)
	{
		replace(exp1->e1, exp1->idString, exp2);
	}
}
void betaReduction(Expression *exp)
{
	if (exp->e1->expressionType == Explambda)
	{
		alphaReduction(exp->e1, exp->e2);
		Expression *expTmp = new Expression(*exp->e1->e1);
		exp->~Expression();
		*exp = *expTmp;
		reduction(exp);
	}
}

void reduction(Expression *exp)
{
	if (exp->expressionType == Explambda)
	{
		reduction(exp->e1);
	}
	else if (exp->expressionType == ExpExp)
	{
		reduction(exp->e1);
		reduction(exp->e2);
		betaReduction(exp);
	}
	else if (exp->expressionType == id)
	{
		auto k = evn.find(exp->idString);
		if (k != evn.end())
		{
			exp->~Expression();
			*exp = *new Expression(*k->second);
		}
	}
}
Expression *tokenizeAndParser(string s)
{
	vector<Token> tokens = tokenize(s);;
	return parser(tokens.begin(), tokens.end());
}
Expression *lambdaX(Expression *exp, string idString)
{
	return new Expression(Explambda, idString, exp, NULL);
}
Expression *expXexp(Expression *exp1, Expression *exp2)
{
	Expression *exp = new Expression(ExpExp, "", exp1, exp2);
	betaReduction(exp);
	return exp;
}
void evnInit()
{
	evn["SUCC"] = tokenizeAndParser("(lambda n (lambda f (lambda x (f ((n f) x)))))");
	evn["PRED"] = tokenizeAndParser("(lambda n (lambda f (lambda x (((n (lambda g (lambda h (h (g f))))) (lambda u x)) (lambda u u)))))");
	evn["PLUS"] = tokenizeAndParser("(lambda m (lambda n (lambda f (lambda x ((m f) ((n f) x))))))");
	evn["MULT"] = tokenizeAndParser("(lambda m (lambda n (lambda f (m (n f)))))");
	evn["0"] = tokenizeAndParser("(lambda f (lambda x x))");
	evn["POW"] = tokenizeAndParser("(lambda b (lambda e (e b)))");
}
int lambda2int(Expression *exp)
{
	return 0;
}
int main()
{
	evnInit();
	vector<Token> tokens;
	Expression *exp1 = expXexp(evn["SUCC"], evn["0"]);
	Expression *exp2 = expXexp(evn["SUCC"], exp1);
	Expression *exp3 = expXexp(evn["SUCC"], exp2);
	Expression *exp4t = expXexp(evn["POW"], exp2);
	Expression *exp4 = expXexp(exp4t, exp2);
	prettyPrint(exp4);
	cout << endl;
	return 0;
}