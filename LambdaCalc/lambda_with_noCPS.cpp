#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
using namespace std;

enum TokenType { tokenId, tokenLeftBracket, tokenRightBracket, tokenBlankSpace, tokenLambda, tokenDefine, tokenExp, tokenErr };
class Token //单词类
{
public:
	TokenType tokenType; //单词的类型
	string idString; //标识符字符串
	Token(TokenType type, string s) :tokenType(type), idString(s){};
};

enum ExpressionType{ Expid, Explambda, Expdefine, ExpExp };
class Expression
{
public:
	ExpressionType expressionType;
	int id;
	Expression *e1;
	Expression *e2;
	Expression() = default;
	Expression(ExpressionType type, int idd, Expression *exp1, Expression *exp2) :expressionType(type), id(idd)
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
	Expression(const Expression &exp) :expressionType(exp.expressionType), id(exp.id)
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
	bool expressionEqual(Expression *exp1)
	{
		if ((exp1->expressionType != expressionType))
		{
			return false;
		}
		if (exp1->expressionType == Explambda)
		{
			return e1->expressionEqual(exp1->e1);
		}
		else if (exp1->expressionType == ExpExp)
		{
			return (e1->expressionEqual(exp1->e1) && e2->expressionEqual(exp1->e2));
		}
		else if (exp1->expressionType == Expid)
		{
			return (exp1->id == id);
		}
	}
	~Expression()
	{
		delete e1;
		delete e2;
	};
};

map<string, Expression*> evn;

void prettyPrint(Expression *exp);
void simplePrint(Expression *exp);


map<string, int> deBruijn({ { "a", 0 }, { "b", 1 }, { "c", 2 }, { "d", 3 }, { "e", 4 }, { "f", 5 }, { "g", 6 }, { "h", 7 }, { "i", 8 }, { "j", 9 }, { "k", 10 }, { "l", 11 }, { "m", 12 }, { "n", 13 }, { "o", 14 }, { "p", 15 }, { "q", 16 }, { "r", 17 }, { "s", 18 }, { "t", 19 }, { "u", 20 }, { "v", 21 }, { "w", 22 }, { "x", 23 }, { "y", 24 }, { "z", 25 } });
map<int, string> njiurBed({ { 0, "a" }, { 1, "b" }, { 2, "c" }, { 3, "d" }, { 4, "e" }, { 5, "f" }, { 6, "g" }, { 7, "h" }, { 8, "i" }, { 9, "j" }, { 10, "k" }, { 11, "l" }, { 12, "m" }, { 13, "n" }, { 14, "o" }, { 15, "p" }, { 16, "q" }, { 17, "r" }, { 18, "s" }, { 19, "t" }, { 20, "u" }, { 21, "v" }, { 22, "w" }, { 23, "x" }, { 24, "y" }, { 25, "z" }, });
vector<Token> tokenize(string s)
//词法分析器
{
	vector<Token> tokens;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == '(') //左括号
		{
			tokens.push_back(Token(tokenLeftBracket, ""));
		}
		else if (s[i] == ')') //右括号
		{
			tokens.push_back(Token(tokenRightBracket, ""));
		}
		else if (s[i] == ' ') //空格。做分隔符用，暂时不记录
		{
			continue;
		}
		else if (s.substr(i, 6) == "lambda") //lambda
		{
			tokens.push_back(Token(tokenLambda, ""));
			i += 5;
		}
		else if (s.substr(i, 6) == "define") //define
		{
			tokens.push_back(Token(tokenDefine, ""));
			i += 5;
		}
		else
		{
			int k = i;
			while ((s[k + 1] != ' ') && (s[k + 1] != '(') && (s[k + 1] != ')'))
			{
				k++;
				if (k == s.size())
				{
					break;
				}
			}
			tokens.push_back(Token(tokenId, s.substr(i, k - i + 1))); //标识符
			i = k;
		}
	}
	return tokens;
}




vector<Token>::iterator findEndToken(vector<Token>::iterator beginToken)
{
	if (beginToken->tokenType == tokenLeftBracket)
	{
		vector<Token>::iterator nextEndToken;
		nextEndToken = beginToken;
		int stack = 0;
		stack++;
		while (stack != 0)
		{
			nextEndToken++;
			if (nextEndToken->tokenType == tokenRightBracket)
			{
				stack--;
			}
			else if (nextEndToken->tokenType == tokenLeftBracket)
			{
				stack++;
			}
		}
		return nextEndToken;
	}
}




void deBruijnTmpChange(int k, map<string, int> *deBruijnTmp)
{
	for (auto &each : *deBruijnTmp)
	{
		each.second += k;
	}
}
Expression *findInDeBruijnTmpOrDeBruijnOrEvn(string s, map<string, int> *deBruijnTmp)
{
	auto k = deBruijnTmp->find(s);
	if (k != deBruijnTmp->end())
	{
		return new Expression(Expid, (*deBruijnTmp)[s], NULL, NULL);
	}
	auto kk = evn.find(s);
	if (kk != evn.end())
	{
		return new Expression(*evn[s]);
	}
	k = deBruijn.find(s);
	if (k != deBruijn.end())
	{
		return new Expression(Expid, deBruijn[s] + deBruijnTmp->size(), NULL, NULL);
	}
	else
	{
		int size = deBruijn.size();
		deBruijn[s] = size;
		njiurBed[size] = s;
		return new Expression(Expid, deBruijn[s] + deBruijnTmp->size(), NULL, NULL);
	}
}

//语法分析
Expression *parser(vector<Token>::iterator beginToken, vector<Token>::iterator endToken, map<string, int> deBruijnTmp)
{
	if (beginToken->tokenType == tokenLeftBracket)
	{
		beginToken++;
		if (beginToken->tokenType == tokenLambda)
		{
			beginToken++;
			if (beginToken->tokenType == tokenId)
			{
				deBruijnTmpChange(1, &deBruijnTmp);
				deBruijnTmp[beginToken->idString] = 0;
				if ((beginToken + 1)->tokenType == tokenId)
				{
					return new Expression(Explambda, -1, findInDeBruijnTmpOrDeBruijnOrEvn((beginToken + 1)->idString, &deBruijnTmp), NULL);
				}
				else
				{
					vector<Token>::iterator nextEndToken;
					nextEndToken = findEndToken(beginToken + 1);
					return new Expression(Explambda, -1, parser(beginToken + 1, nextEndToken, deBruijnTmp), NULL);
				}
			}
		}
		else if (beginToken->tokenType == tokenDefine)
		{
			beginToken++;
			if (beginToken->tokenType == tokenId)
			{
				auto k = deBruijn.find(beginToken->idString);
				if (k == deBruijn.end())
				{
					int size = deBruijn.size();
					deBruijn[beginToken->idString] = size;
					njiurBed[size] = beginToken->idString;
				}

				if ((beginToken + 1)->tokenType == tokenId)
				{
					evn[beginToken->idString] = findInDeBruijnTmpOrDeBruijnOrEvn((beginToken + 1)->idString, &deBruijnTmp);
					return new Expression(Expdefine, deBruijn[beginToken->idString], evn[beginToken->idString], NULL);
				}
				else if ((beginToken + 1)->tokenType == tokenLeftBracket)
				{
					vector<Token>::iterator nextEndToken;
					nextEndToken = findEndToken(beginToken + 1);
					evn[beginToken->idString] = parser(beginToken + 1, nextEndToken, deBruijnTmp);
					return new Expression(Expdefine, deBruijn[beginToken->idString], evn[beginToken->idString], NULL);
				}
			}
		}
		else if (beginToken->tokenType == tokenId)
		{
			if (beginToken + 1 == endToken)
			{
				return findInDeBruijnTmpOrDeBruijnOrEvn(beginToken->idString, &deBruijnTmp);
			}
			if ((beginToken + 1)->tokenType == tokenId)
			{
				return new Expression(ExpExp, -2, findInDeBruijnTmpOrDeBruijnOrEvn(beginToken->idString, &deBruijnTmp), findInDeBruijnTmpOrDeBruijnOrEvn((beginToken + 1)->idString, &deBruijnTmp));
			}
			else
			{
				vector<Token>::iterator nextEndToken;
				nextEndToken = findEndToken(beginToken + 1);
				return new Expression(ExpExp, -2, findInDeBruijnTmpOrDeBruijnOrEvn(beginToken->idString, &deBruijnTmp), parser(beginToken + 1, nextEndToken, deBruijnTmp));
			}
		}
		else
		{
			vector<Token>::iterator nextEndToken;
			nextEndToken = findEndToken(beginToken);
			if ((nextEndToken + 1)->tokenType == tokenId)
			{
				return new Expression(ExpExp, -2, parser(beginToken, nextEndToken, deBruijnTmp), findInDeBruijnTmpOrDeBruijnOrEvn((nextEndToken + 1)->idString, &deBruijnTmp));
			}
			else
			{
				vector<Token>::iterator nextEndToken1;
				nextEndToken1 = findEndToken(nextEndToken + 1);
				return new Expression(ExpExp, -2, parser(beginToken, nextEndToken, deBruijnTmp), parser(nextEndToken + 1, nextEndToken1, deBruijnTmp));
			}
		}
	}
	else if (beginToken->tokenType == tokenId)
	{
		return  findInDeBruijnTmpOrDeBruijnOrEvn(beginToken->idString, &deBruijnTmp);
	}
}

//移位
void expressionShift(Expression *exp, int d, int c)
{
	if (exp->expressionType == Expid)
	{
		if (exp->id >= c)
		{
			exp->id += d;
		}
	}
	else if (exp->expressionType == Explambda)
	{
		expressionShift(exp->e1, d, c + 1);
	}
	else if (exp->expressionType == ExpExp)
	{
		expressionShift(exp->e1, d, c);
		expressionShift(exp->e2, d, c);
	}
}

//代换
void replace(int j, Expression *s, Expression *exp)
{
	if (exp->expressionType == Expid)
	{
		if (exp->id == j)
		{
			exp->~Expression();
			*exp = *new Expression(*s);
		}
	}
	else if (exp->expressionType == Explambda)
	{
		Expression *sTmp = new Expression(*s);
		expressionShift(sTmp, 1, 0);
		replace(j + 1, sTmp, exp->e1);
		sTmp->~Expression();
	}
	else if (exp->expressionType == ExpExp)
	{
		replace(j, s, exp->e1);
		replace(j, s, exp->e2);
	}
}

//beta归约
void betaReduction(Expression *exp)
{
	if (exp->expressionType == ExpExp)
	{
		if (exp->e1->expressionType == Explambda)
		{
			betaReduction(exp->e2);
			expressionShift(exp->e2, 1, 0);
			replace(0, exp->e2, exp->e1->e1);
			expressionShift(exp->e1->e1, -1, 0);
			Expression *expTmp = new Expression(*exp->e1->e1);
			exp->~Expression();
			*exp = *expTmp;
		}
	}
}

//CPS变换
Expression *cps(Expression *exp)
{
	if (exp->expressionType == Expid)
	{
		return new Expression(Explambda, -1, new Expression(ExpExp, -2, new Expression(Expid, 0, NULL, NULL), new Expression(Expid, exp->id + 1, NULL, NULL)), NULL);
	}
	else if (exp->expressionType == Explambda)
	{
		Expression *e1 = new Expression(*exp->e1);
		expressionShift(e1, 1, 1);
		return new Expression(Explambda, -1, new Expression(ExpExp, -2, new Expression(Expid, 0, NULL, NULL), new Expression(Explambda, -1, cps(e1), NULL)), NULL);
	}
	else if (exp->expressionType == ExpExp)
	{
		Expression *e1 = new Expression(*exp->e1);
		Expression *e2 = new Expression(*exp->e2);
		expressionShift(e1, 1, 0);
		expressionShift(e2, 2, 1);
		return new Expression(Explambda, -1,
			new Expression(ExpExp, -2,
			cps(e1),
			new Expression(Explambda, -1,
			new Expression(ExpExp, -2,
			cps(e2),
			new Expression(Explambda, -1,
			new Expression(ExpExp, -1,
			new Expression(ExpExp, -1,
			new Expression(Expid, 1, NULL, NULL),
			new Expression(Expid, 0, NULL, NULL)),
			new Expression(Expid, 2, NULL, NULL)),
			NULL)),
			NULL)),
			NULL);
	}
}

void _simpleExpression(Expression *exp)
{
	if (exp->expressionType == Explambda)
	{
		_simpleExpression(exp->e1);
	}
	else if (exp->expressionType == ExpExp)
	{
		_simpleExpression(exp->e1);
		_simpleExpression(exp->e2);
		betaReduction(exp);
	}
}
void _simplePrint(Expression *exp)
{
	if (exp->expressionType == Explambda)
	{
		cout << "(lambda";
		_simplePrint(exp->e1);
		cout << ')';
	}
	else if (exp->expressionType == ExpExp)
	{
		cout << "(";
		_simplePrint(exp->e1);
		cout << " ";
		_simplePrint(exp->e2);
		cout << ')';
	}
	else if (exp->expressionType == Expid)
	{
		cout << exp->id;
	}
}
void _prettyPrint(Expression *exp, map<int, string> njiurBedTmp)
{
	if (exp->expressionType == Explambda)
	{
		int size = njiurBedTmp.size();
		njiurBedTmp[size] = njiurBed[size];
		cout << "(lambda " << njiurBedTmp[size] << " ";
		_prettyPrint(exp->e1, njiurBedTmp);
		cout << ')';
	}
	else if (exp->expressionType == ExpExp)
	{
		cout << "(";
		_prettyPrint(exp->e1, njiurBedTmp);
		cout << " ";
		_prettyPrint(exp->e2, njiurBedTmp);
		cout << ')';
	}
	else if (exp->expressionType == Expid)
	{
		int l = njiurBedTmp.size() - exp->id - 1;
		if (l<0)
		{
			cout << njiurBed[exp->id - njiurBedTmp.size()];
		}
		else
		{
			cout << njiurBedTmp[l];
		}

	}
}
void prettyPrint(Expression *exp)
{
	map<int, string> njiurBedTmp;
	_prettyPrint(exp, njiurBedTmp);
	cout << endl;
}
void simplePrint(Expression *exp)
{
	_simplePrint(exp);
	cout << endl;
}
Expression *tokenizeAndParser(string s)
{
	vector<Token> tokens = tokenize(s);
	map<string, int> deBruijnTmp;
	deBruijnTmp.clear();
	Expression *exp = parser(tokens.begin(), tokens.end(), deBruijnTmp);
	return exp;
}

void evnInit()
{
	evn["0"] = tokenizeAndParser("(lambda f (lambda x x))");
	evn["true"] = tokenizeAndParser("(lambda t (lambda f t))");
	evn["false"] = tokenizeAndParser("(lambda t (lambda f f))");

	evn["if"] = tokenizeAndParser("(lambda l (lambda m (lambda n ((l m) n))))");
	evn["and"] = tokenizeAndParser("(lambda b (lambda c ((b c) false)))");
	evn["or"] = tokenizeAndParser("(lambda b (lambda c ((b true) c)))");
	evn["not"] = tokenizeAndParser("(lambda b ((b false) true))");
	evn["pair"] = tokenizeAndParser("(lambda f (lambda s (lambda b ((b f) s))))");
	evn["first"] = tokenizeAndParser("(lambda p (p true))");
	evn["second"] = tokenizeAndParser("(lambda p (p false))");


	evn["succ"] = tokenizeAndParser("(lambda n (lambda f (lambda x (f ((n f) x)))))");
	evn["plus"] = tokenizeAndParser("(lambda m (lambda n (lambda f (lambda x ((m f) ((n f) x))))))");
	evn["mult"] = tokenizeAndParser("(lambda m (lambda n (lambda f (m (n f)))))");
	evn["pred_zz"] = tokenizeAndParser("((pair 0) 0)");
	evn["pred_ss"] = tokenizeAndParser("(lambda p ((pair (second p)) (succ (second p))))");
	evn["pred"] = tokenizeAndParser("(lambda m (first ((m pred_ss) pred_zz)))");
	evn["sub"] = tokenizeAndParser("(lambda m (lambda n ((n pred) m)))");
	evn["zero?"] = tokenizeAndParser("(lambda m ((m (lambda x (false))) true))");
	evn["<="] = tokenizeAndParser("(lambda m (lambda n (zero? ((sub m) n))))");
	evn[">="] = tokenizeAndParser("(lambda m (lambda n (zero? ((sub n) m))))");
	evn["equal?"] = tokenizeAndParser("(lambda m (lambda n ( (and ((>= m) n)) ((<= m) n) ) ))");
	evn["pow"] = tokenizeAndParser("(lambda b (lambda e (e b)))");

	evn["1"] = tokenizeAndParser("(succ 0)");
	evn["2"] = tokenizeAndParser("(succ 1)");
	evn["3"] = tokenizeAndParser("(succ 2)");
	evn["4"] = tokenizeAndParser("(succ 3)");
	evn["5"] = tokenizeAndParser("(succ 4)");
	evn["6"] = tokenizeAndParser("(succ 5)");
	evn["7"] = tokenizeAndParser("(succ 6)");
	evn["8"] = tokenizeAndParser("(succ 7)");
	evn["9"] = tokenizeAndParser("(succ 8)");
	evn["10"] = tokenizeAndParser("(succ 9)");

	for (auto &each : evn)
	{
		_simpleExpression(each.second);
	}
	evn["fix"] = tokenizeAndParser("(lambda f ((lambda x (f (lambda y ((x x) y)))) (lambda x (f (lambda y ((x x) y))))))");
	evn["g"] = tokenizeAndParser("(lambda fct (lambda n (((if ((equal? n) 0)) 1) ((mult n) (fct (pred n))))))");
	evn["fact"] = tokenizeAndParser("(fix g)");

	evn["succ_cps"] = cps(evn["succ"]);
	evn["not_cps"] = cps(evn["not"]);
	evn["1_cps"] = cps(evn["1"]);
	evn["0_cps"] = cps(evn["0"]);
}

void deBruijnInit()
{
	for (size_t i = 26; i < 1026; i++)
	{
		char buf[10];
		_itoa_s(i, buf, 10);
		string s = "x";
		s = s + buf;
		deBruijn[s] = i;
		njiurBed[i] = s;
	}
}

int lambda2int(Expression *exp)
{
	int ans = 0;
	if (exp->expressionType == Explambda)
	{
		while (true)
		{
			if (evn["0"]->expressionEqual(exp))
			{
				return ans;
			}
			else
			{
				*exp = *new Expression(ExpExp, -2, evn["PRED"], exp);
				betaReduction(exp);
				ans++;
			}
		}
	}
}
void testString()
{

}
void repr()
{
	string s;
	Expression *exp = new Expression();
	vector<function<void(Expression*)>> printFuncVector{ simplePrint, prettyPrint };
	int printMode = 1;
	while (true)
	{
		cout << "λ>>";
		getline(cin, s);
		if (s[0] == ':')
		{
			if (s == ":q")
			{
				return;
			}
			else if (s == ":print mode 0")
			{
				printMode = 0;
			}
			else if (s == ":print mode 1")
			{
				printMode = 1;
			}
			else if (s == ":test")
			{
				testString();
			}
			else if (s == ":simple")
			{
				_simpleExpression(exp);
				printFuncVector[printMode](exp);
			}
			continue;
		}
		if (s == "")
		{
			continue;
		}
		exp = tokenizeAndParser(s);
		if (exp->expressionType == ExpExp)
		{
			betaReduction(exp);
		}
		printFuncVector[printMode](exp);
	}
}

int main()
{
	evnInit();
	repr();
	return 0;
}