#include "Expression.h"
map<string, Expression* > evn;	//���л���



//Expression *cps(Expression *exp)
//{
//
//	if (exp->expressionType == ExpId)
//	{
//		return new Expression(ExpLambda, -1, new Expression(ExpExp, -2, new Expression(ExpId, 0, NULL, NULL), new Expression(ExpId, exp->id + 1, NULL, NULL)), NULL);
//	}
//	else if (exp->expressionType == ExpLambda)
//	{
//		Expression *e1 = new Expression(*exp->e1);
//		expressionShift(e1, 1, 1);
//		return new Expression(ExpLambda, -1, new Expression(ExpExp, -2, new Expression(ExpId, 0, NULL, NULL), new Expression(ExpLambda, -1, cps(e1), NULL)), NULL);
//	}
//	else if (exp->expressionType == ExpExp)
//	{
//		Expression *e1 = new Expression(*exp->e1);
//		Expression *e2 = new Expression(*exp->e2);
//		expressionShift(e1, 1, 0);
//		expressionShift(e2, 2, 1);
//		return new Expression(ExpLambda, -1,
//			new Expression(ExpExp, -2,
//			cps(e1),
//			new Expression(ExpLambda, -1,
//			new Expression(ExpExp, -2,
//			cps(e2),
//			new Expression(ExpLambda, -1,
//			new Expression(ExpExp, -1,
//			new Expression(ExpExp, -1,
//			new Expression(ExpId, 1, NULL, NULL),
//			new Expression(ExpId, 0, NULL, NULL)),
//			new Expression(ExpId, 2, NULL, NULL)),
//			NULL)),
//			NULL)),
//			NULL);
//	}
//}

void _simplePrint(Expression *exp)
{
	if (exp->id == ExpLambda)
	{
		cout << "(lambda";
		_simplePrint(exp->e1);
		cout << ')';
	}
	else if (exp->id == ExpExp)
	{
		cout << "(";
		_simplePrint(exp->e1);
		cout << " ";
		_simplePrint(exp->e2);
		cout << ')';
	}
	else if (exp->id >-1)
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
	if (exp->id == ExpLambda)
	{
		int size = njiurBedTmp.size();
		njiurBedTmp[size] = njiurBed[size];
		cout << "(lambda " << njiurBedTmp[size] << " ";
		_prettyPrint(exp->e1, njiurBedTmp);
		cout << ')';
	}
	else if (exp->id == ExpExp)
	{
		cout << "(";
		_prettyPrint(exp->e1, njiurBedTmp);
		cout << " ";
		_prettyPrint(exp->e2, njiurBedTmp);
		cout << ')';
	}
	else if (exp->id>-1)
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





void deBruijnInit()
{
	int size = deBruijn.size();
	for (size_t i = 0; i < 1000; i++)
	{
		char buf[10];
		_itoa_s(i, buf, 10);
		string s = "x";
		s = s + buf;
		deBruijn[s] = size+i;
		njiurBed[size + i] = s;
	}
}


void testMode()
{

}


void eval(Expression* exp)
{
	if (exp->id == ExpExp)
	{
		//��exp��(e1 e2)��ʽ
		if (exp->e1->id>-1)
		{
			// ���(x ?) x�Ƿ��š�������������ɱ���
			if (exp->e1->id >  - 1)
			{
				auto k = evn.find(njiurBed[exp->e1->id]);
				if (k != evn.end())
				{
					// x�ڻ����˵��x�Ƿ���
					exp->e1->~Expression();
					exp->e1 = new Expression(*k->second);
					exp->betaReduction();
					eval(exp);
					return;
				}
			}
			else if (exp->e2->id == ExpExp)
			{
				// x���ڻ����˵��x�Ǹ�������������ɱ���
				// ��?��(e1 e2)��ʽ����?��ֵ��Ȼ�󷵻�
				eval(exp->e2);
				return;
			}
			else
			{
				// ?�Ƿ��š�������������ɱ�����lambda����û�м�����ֵ������
				return;
			}
		}
		else if (exp->e1->id==ExpExp)
		{
			//��e1��(exp exp)��ʽ����e1��ֵ
			eval(exp->e1);
		}
		if (exp->e1->id == ExpLambda)
		{
			exp->betaReduction();
			eval(exp);
			return;
		}
	}
	else if (exp->id >-1)
	{
		// exp�Ƿ��š�������������ɱ���
		auto k = evn.find(njiurBed[exp->id]);
		if (k != evn.end())
		{
			// x�ڻ����˵��x�Ƿ��ţ��ҿ϶���lambda����
			exp->~Expression();
			*exp = *new Expression(*k->second);
			eval(exp);
			return;
		}
	}
}
Expression *tokenizeAndParserAndBuildAndSimpleExp(string s)
{
	Expression *exp = new Expression(&s);
	return exp;
}
void simpleExpression(Expression *exp,int p)
{
	if (exp->id == ExpLambda)
	{
		simpleExpression(exp->e1,p+1);
	}
	else if (exp->id == ExpExp)
	{
		exp->expressionShift(-p,p);
		eval(exp);
		exp->expressionShift(p, p);
		if (exp->id == ExpLambda)
		{
			simpleExpression(exp->e1,p+1);
		}
		else if (exp->id == ExpExp)
		{
			simpleExpression(exp->e2,p);
		}
	}
	else if (exp->id >-1)
	{
		auto k = evn.find(njiurBed[exp->id-p]);
		if (k != evn.end())
		{
			// x�ڻ����˵��x�Ƿ��ţ��ҿ϶���lambda����
			exp->~Expression();
			*exp = *new Expression(*k->second);
			simpleExpression(exp,p);
		}
	}
}
void defineExp(string *s)
{
	int k = 2;
	while ((*s)[k + 1] != '=')
	{
		k++;
		if (k == s->size())
		{
			break;
		}
	}
	evn[s->substr(2, k - 1)] = tokenizeAndParserAndBuildAndSimpleExp(s->substr(k+2, s->size()-k-1));
	simpleExpression(evn[s->substr(2, k - 1)],0);
}
void repr()
{
	string s;
	Expression *exp = new Expression();
	vector<function<void(Expression*)>> printFuncVector{ simplePrint, prettyPrint };
	int printMode = 1;
	while (true)
	{
		cout << "��>>";
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
				simpleExpression(exp,0);
				printFuncVector[printMode](exp);
			}
			else if (s[1] == ':')
			{
				defineExp(&s);
			}
			continue;
		}
		if (s == "")
		{
			continue;
		}
		exp->~Expression();
		exp = tokenizeAndParserAndBuildAndSimpleExp(s);
		eval(exp);
		simpleExpression(exp,0);
		printFuncVector[printMode](exp);
	}
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

	evn["1"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda f (lambda x (f x)))");
	evn["2"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda f (lambda x (f (f x))))");
	evn["3"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda f (lambda x (f (f (f x)))))");

	evn["fix"] = new Expression(new string("(lambda f ((lambda x (f (lambda y ((x x) y)))) (lambda x (f (lambda y ((x x) y))))))"));
	evn["fact_g"] = tokenizeAndParserAndBuildAndSimpleExp("(lambda fct (lambda n (((if ((equal? n) 0)) 1) ((mult n) (fct (pred n))))))");
	evn["fact"] = tokenizeAndParserAndBuildAndSimpleExp("(fix fact_g)");
	//evn["succ_cps"] = cps(evn["succ"]);
	//evn["not_cps"] = cps(evn["not"]);
	//evn["1_cps"] = cps(evn["1"]);
	//evn["0_cps"] = cps(evn["0"]);
}
int main()
{
	evnInit();
	deBruijnInit();
	repr();
	return 0;
}