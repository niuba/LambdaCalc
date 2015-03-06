#include "lambda.h"
vector<Token> tokenize(string s)
//词法分析器
{
	vector<Token> tokens;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == '(') //左括号
		{
			tokens.push_back(Token(TokenLeftBracket, ""));
		}
		else if (s[i] == ')') //右括号
		{
			tokens.push_back(Token(TokenRightBracket, ""));
		}
		else if (s[i] == ' ') //空格。做分隔符用，暂时不记录
		{
			continue;
		}
		else if (s.substr(i, 6) == "lambda") //lambda
		{
			tokens.push_back(Token(TokenLambda, ""));
			i += 5;
		}
		else if (s.substr(i, 6) == "define") //define
		{
			tokens.push_back(Token(TokenDefine, ""));
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
			tokens.push_back(Token(TokenId, s.substr(i, k - i + 1))); //标识符
			i = k;
		}
	}
	return tokens;
}

AST *parser(vector<Token>::iterator *pToken)
//语法分析器，构建抽象语法树
{
	AST *ast = new AST();
	if ((*pToken)->tokenType == TokenId)	//只有一个元素
	{

		ast->leftVal = new Token(*(*pToken));
	}
	else if ((*pToken)->tokenType == TokenLeftBracket)	//必须以左括号开头
	{
		//第一个元素		
		(*pToken)++;
		if ((*pToken)->tokenType == TokenLeftBracket)
		{
			ast->leftAST = parser(pToken);
		}
		else
		{
			ast->leftVal = new Token(*(*pToken));
		}
		//第二个元素	**TODO 未做元素个数检查，(a)则会有问题**
		(*pToken)++;
		if ((*pToken)->tokenType == TokenLeftBracket)
		{
			ast->rightAST = parser(pToken);
		}
		else
		{
			ast->rightVal = new Token(*(*pToken));
		}
		(*pToken)++;
		while ((*pToken)->tokenType != TokenRightBracket)
		{
			if ((*pToken)->tokenType == TokenLeftBracket)
			{
				AST *astTmp = new AST(NULL, NULL, ast, parser(pToken));
				ast = astTmp;
			}
			else
			{
				AST *astTmp = new AST(NULL, new Token(*(*pToken)), ast, NULL);
				ast = astTmp;
			}
			(*pToken)++;
		}
	}
	return ast;
}

Expression *buildExpression(AST *ast, map<string, int> deBruijnTmp)
//构建表达式对象
{
	if (ast->leftVal != NULL)
	{
		// (x (y|E|NULL))
		string val = ast->leftVal->idString;
		if (ast->rightVal != NULL)
		{
			// (x y)
			return new Expression(ExpExp, -3,
				findInDeBruijnTmpOrDeBruijnOrEvn(val, &deBruijnTmp),
				findInDeBruijnTmpOrDeBruijnOrEvn(ast->rightVal->idString, &deBruijnTmp));
		}
		else if (ast->rightAST != NULL)
		{
			// (x E)
			return new Expression(ExpExp, -3,
				findInDeBruijnTmpOrDeBruijnOrEvn(val, &deBruijnTmp),
				buildExpression(ast->rightAST, deBruijnTmp));
		}
		else
		{
			// x
			return findInDeBruijnTmpOrDeBruijnOrEvn(val, &deBruijnTmp);
		}
	}
	else if ((ast->leftAST->leftVal != NULL) && (ast->leftAST->rightVal != NULL))
	{
		if (ast->leftAST->leftVal->tokenType == TokenLambda)
		{		
			// lambda
			string val = ast->leftAST->rightVal->idString;
			deBruijnTmpChange(1, &deBruijnTmp);
			deBruijnTmp[val] = 0;
			if (ast->rightVal != NULL)
			{
				//(lambda k p)
				return new Expression(ExpLambda, -1,
					findInDeBruijnTmpOrDeBruijnOrEvn(val, &deBruijnTmp), NULL);
			}
			else if (ast->rightAST != NULL)
			{
				//(lambda k E)
				return new Expression(ExpLambda, -1,
					buildExpression(ast->rightAST, deBruijnTmp), NULL);
			}
		}
		else if (ast->leftAST->leftVal->tokenType == TokenDefine)
		{
			// define
			string val = ast->leftAST->rightVal->idString;
			if (ast->rightVal != NULL)
			{
				//(define k p)
				evn[val] = findInDeBruijnTmpOrDeBruijnOrEvn(val, &deBruijnTmp);
				return new Expression(ExpDefine, -2, evn[val], NULL);
			}
			else if (ast->rightAST != NULL)
			{
				//(define k E)
				evn[val] = buildExpression(ast->rightAST, deBruijnTmp);
				return new Expression(ExpDefine, -2, evn[val], NULL);
			}
		}
		else
		{
			if(ast->rightAST != NULL)
			{
				// ((x y) E)
				return new Expression(ExpExp, -3,
					new Expression(ExpExp, -3,
						findInDeBruijnTmpOrDeBruijnOrEvn(ast->leftAST->leftVal->idString, &deBruijnTmp),
						findInDeBruijnTmpOrDeBruijnOrEvn(ast->leftAST->rightVal->idString, &deBruijnTmp)),
					buildExpression(ast->rightAST, deBruijnTmp));
			}
			else if (ast->rightVal != NULL)
			{
				// ((x y) x)
				return new Expression(ExpExp, -3,
					new Expression(ExpExp, -3,
					findInDeBruijnTmpOrDeBruijnOrEvn(ast->leftAST->leftVal->idString, &deBruijnTmp),
					findInDeBruijnTmpOrDeBruijnOrEvn(ast->leftAST->rightVal->idString, &deBruijnTmp)),
					findInDeBruijnTmpOrDeBruijnOrEvn(ast->rightVal->idString, &deBruijnTmp));
			}		
		}
	}
	else if (ast->rightAST != NULL)
	{
		// (E E)
		return new Expression(ExpExp, -3,
			buildExpression(ast->leftAST, deBruijnTmp),
			buildExpression(ast->rightAST, deBruijnTmp));
	}
	else if (ast->rightVal != NULL)
	{
		// (E x)
		return new Expression(ExpExp, -3,
			buildExpression(ast->leftAST, deBruijnTmp),
			findInDeBruijnTmpOrDeBruijnOrEvn(ast->rightVal->idString, &deBruijnTmp));
	}
}

void simpleExpression(Expression *exp)
{
	if (exp->expressionType == ExpLambda)
	{
		simpleExpression(exp->e1);
	}
	else if (exp->expressionType == ExpExp)
	{
		simpleExpression(exp->e1);
		simpleExpression(exp->e2);
		if (exp->e1->expressionType == ExpLambda)
		{
			betaReduction(exp);
		}
		
		//if (exp->e1->expressionType == ExpLambda)
		//{
		//	betaReduction(exp);
		//	simpleExpression(exp);
		//}
		//else if (exp->e1->expressionType == ExpExp)
		//{
		//	Expression *expTmp = &Expression(*exp->e1);
		//	simpleExpression(exp->e1);
		//	if (!expTmp->expressionEqual(exp->e1))
		//	{
		//		simpleExpression(exp);
		//	}
		//}
		//else if (exp->e1->expressionType == ExpId)
		//{
		//	simpleExpression(exp->e2);
		//}
	}
}

Expression *tokenizeAndParserAndBuild(string s)
{
	map<string, int> deBruijnTmp;
	deBruijnTmp.clear();
	Expression *exp = buildExpression(parser(&(tokenize(s).begin())), deBruijnTmp);
	return exp;
}

Expression *tokenizeAndParserAndBuildAndSimpleExp(string s)
{
	Expression *exp = tokenizeAndParserAndBuild(s);
	simpleExpression(exp);
	return exp;
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
		return new Expression(ExpId, (*deBruijnTmp)[s], NULL, NULL);
	}
	auto kk = evn.find(s);
	if (kk != evn.end())
	{
		return new Expression(*evn[s]);
	}
	k = deBruijn.find(s);
	if (k != deBruijn.end())
	{
		return new Expression(ExpId, deBruijn[s] + deBruijnTmp->size(), NULL, NULL);
	}
	else
	{
		int size = deBruijn.size();
		deBruijn[s] = size;
		njiurBed[size] = s;
		return new Expression(ExpId, deBruijn[s] + deBruijnTmp->size(), NULL, NULL);
	}
}

void expressionShift(Expression *exp, int d, int c)
{
	if (exp->expressionType == ExpId)
	{
		if (exp->id >= c)
		{
			exp->id += d;
		}
	}
	else if (exp->expressionType == ExpLambda)
	{
		expressionShift(exp->e1, d, c + 1);
	}
	else if (exp->expressionType == ExpExp)
	{
		expressionShift(exp->e1, d, c);
		expressionShift(exp->e2, d, c);
	}
}

void replace(int j, Expression *s, Expression *exp)
{
	if (exp->expressionType == ExpId)
	{
		if (exp->id == j)
		{
			exp->~Expression();
			*exp = *new Expression(*s);
		}
	}
	else if (exp->expressionType == ExpLambda)
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

void betaReduction(Expression *exp)
{
	expressionShift(exp->e2, 1, 0);
	replace(0, exp->e2, exp->e1->e1);
	expressionShift(exp->e1->e1, -1, 0);
	Expression *expTmp = new Expression(*exp->e1->e1);
	exp->~Expression();
	*exp = *expTmp;
}

Expression *cps(Expression *exp)
{

	if (exp->expressionType == ExpId)
	{
		return new Expression(ExpLambda, -1, new Expression(ExpExp, -2, new Expression(ExpId, 0, NULL, NULL), new Expression(ExpId, exp->id + 1, NULL, NULL)), NULL);
	}
	else if (exp->expressionType == ExpLambda)
	{
		Expression *e1 = new Expression(*exp->e1);
		expressionShift(e1, 1, 1);
		return new Expression(ExpLambda, -1, new Expression(ExpExp, -2, new Expression(ExpId, 0, NULL, NULL), new Expression(ExpLambda, -1, cps(e1), NULL)), NULL);
	}
	else if (exp->expressionType == ExpExp)
	{
		Expression *e1 = new Expression(*exp->e1);
		Expression *e2 = new Expression(*exp->e2);
		expressionShift(e1, 1, 0);
		expressionShift(e2, 2, 1);
		return new Expression(ExpLambda, -1,
			new Expression(ExpExp, -2,
			cps(e1),
			new Expression(ExpLambda, -1,
			new Expression(ExpExp, -2,
			cps(e2),
			new Expression(ExpLambda, -1,
			new Expression(ExpExp, -1,
			new Expression(ExpExp, -1,
			new Expression(ExpId, 1, NULL, NULL),
			new Expression(ExpId, 0, NULL, NULL)),
			new Expression(ExpId, 2, NULL, NULL)),
			NULL)),
			NULL)),
			NULL);
	}
}

void _simplePrint(Expression *exp)
{
	if (exp->expressionType == ExpLambda)
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
	else if (exp->expressionType == ExpId)
	{
		cout << exp->id;
	}
}

void simplePrint(Expression *exp)
{
	_simplePrint(exp);
	cout << endl;
}

void _prettyPrint(Expression *exp, map<int, string> njiurBedTmp)
{
	if (exp->expressionType == ExpLambda)
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
	else if (exp->expressionType == ExpId)
	{
		int l = njiurBedTmp.size() - exp->id - 1;
		if (l < 0)
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

void evnInit()
{
	evn["0"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda f (lambda x x))");
	evn["true"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda t (lambda f t))");
	evn["false"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda t (lambda f f))");

	evn["if"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda l (lambda m (lambda n ((l m) n))))");
	evn["and"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda b (lambda c ((b c) false)))");
	evn["or"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda b (lambda c ((b true) c)))");
	evn["not"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda b ((b false) true))");
	evn["pair"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda f (lambda s (lambda b ((b f) s))))");
	evn["first"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda p (p true))");
	evn["second"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda p (p false))");


	evn["succ"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda n (lambda f (lambda x (f ((n f) x)))))");
	evn["plus"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (lambda n (lambda f (lambda x ((m f) ((n f) x))))))");
	evn["mult"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (lambda n (lambda f (m (n f)))))");
	evn["pred_zz"] = tokenizeAndParserAndBuildAndSimpleExp("((pair 0) 0)");
	evn["pred_ss"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda p ((pair (second p)) (succ (second p))))");
	evn["pred"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (first ((m pred_ss) pred_zz)))");
	evn["sub"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (lambda n ((n pred) m)))");
	evn["zero?"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m ((m (lambda x false)) true))");
	evn["<="] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (lambda n (zero? ((sub m) n))))");
	evn[">="] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (lambda n (zero? ((sub n) m))))");
	evn["equal?"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda m (lambda n ( (and ((>= m) n)) ((<= m) n) ) ))");
	evn["pow"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda b (lambda e (e b)))");

	evn["1"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 0)");
	evn["2"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 1)");
	evn["3"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 2)");
	evn["4"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 3)");
	evn["5"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 4)");
	evn["6"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 5)");
	evn["7"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 6)");
	evn["8"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 7)");
	evn["9"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 8)");
	evn["10"] = tokenizeAndParserAndBuildAndSimpleExp("(succ 9)");




	evn["fix"] = tokenizeAndParserAndBuild("(lambda f ((lambda x (f (lambda y ((x x) y)))) (lambda x (f (lambda y ((x x) y))))))");
	evn["g"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda fct (lambda n (((if ((equal? n) 0)) 1) ((mult n) (fct (pred n))))))");
	evn["fact"] = tokenizeAndParserAndBuildAndSimpleExp("(fix g)");

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


void testMode()
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
				testMode();
			}
			else if (s == ":simple")
			{
				simpleExpression(exp);
				printFuncVector[printMode](exp);
			}
			continue;
		}
		if (s == "")
		{
			continue;
		}
		exp = tokenizeAndParserAndBuildAndSimpleExp(s);
		if (exp->expressionType == ExpExp)
		{
			if (exp->e1->expressionType == ExpLambda)
			{
				betaReduction(exp);
			}
		}
		printFuncVector[printMode](exp);
	}
}

int main()
{
	evnInit();
	deBruijnInit();
	repr();
	return 0;
}