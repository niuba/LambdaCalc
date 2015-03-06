#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
using namespace std;


enum TokenType { TokenId, TokenLeftBracket, TokenRightBracket, TokenBlankSpace, TokenLambda, TokenDefine, TokenError };
enum ExpressionType { ExpId, ExpLambda, ExpDefine, ExpExp, ExpError };


class Token //������
{
public:
	TokenType tokenType; //���ʵ�����
	string idString; //��ʶ���ַ���
	Token(){};
	~Token(){};
	Token(TokenType type, string s) :tokenType(type), idString(s){};
	Token(TokenType type) :tokenType(type), idString(""){};
	Token(const Token &token) :tokenType(token.tokenType), idString(token.idString){};
};

class AST
{
public:
	Token *leftVal;
	Token *rightVal;
	AST *leftAST;
	AST *rightAST;
	AST() :leftVal(NULL), rightVal(NULL), leftAST(NULL), rightAST(NULL){};
	~AST()
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
			leftAST ->~AST();
		}
		if (rightAST != NULL)
		{
			rightAST->~AST();
		}
	};
	AST(Token *lv, Token *rv, AST *la, AST *ra) :leftVal(lv), rightVal(rv), leftAST(la), rightAST(ra){};
	AST(const AST &a)
	{
		if (a.leftVal != NULL)
		{
			leftVal = new Token(*a.leftVal);
		}
		else
		{
			leftVal = NULL;
		}

		if (a.rightVal != NULL)
		{
			rightVal = new Token(*a.rightVal);
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
	};
};

class Expression
{
public:
	ExpressionType expressionType;
	int id;
	Expression *e1;
	Expression *e2;
	Expression() {};
	~Expression()
	{
		if (e1 != NULL)
		{
			e1->~Expression();
		}
		if (e2 != NULL)
		{
			e2->~Expression();
		}
	};
	Expression(ExpressionType type, int idd, Expression *exp1, Expression *exp2) :expressionType(type), id(idd)
	{
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
	Expression(const Expression &exp) :expressionType(exp.expressionType), id(exp.id)
	{
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
	bool expressionEqual(Expression *exp1)
	{
		if ((exp1->expressionType != expressionType))
		{
			return false;
		}
		if (exp1->expressionType == ExpId)
		{
			return (exp1->id == id);
		}
		else if (exp1->expressionType == ExpLambda)
		{
			return e1->expressionEqual(exp1->e1);
		}
		else if (exp1->expressionType == ExpExp)
		{
			return (e1->expressionEqual(exp1->e1) && e2->expressionEqual(exp1->e2));
		}
	}

};

map<string, Expression*> evn;	//���л���

map<string, int> deBruijn({ { "a", 0 }, { "b", 1 }, { "c", 2 }, { "d", 3 }, { "e", 4 }, { "f", 5 }, { "g", 6 }, { "h", 7 }, { "i", 8 }, { "j", 9 }, { "k", 10 }, { "l", 11 }, { "m", 12 }, { "n", 13 }, { "o", 14 }, { "p", 15 }, { "q", 16 }, { "r", 17 }, { "s", 18 }, { "t", 19 }, { "u", 20 }, { "v", 21 }, { "w", 22 }, { "x", 23 }, { "y", 24 }, { "z", 25 } });	//Ĭ������������
map<int, string> njiurBed({ { 0, "a" }, { 1, "b" }, { 2, "c" }, { 3, "d" }, { 4, "e" }, { 5, "f" }, { 6, "g" }, { 7, "h" }, { 8, "i" }, { 9, "j" }, { 10, "k" }, { 11, "l" }, { 12, "m" }, { 13, "n" }, { 14, "o" }, { 15, "p" }, { 16, "q" }, { 17, "r" }, { 18, "s" }, { 19, "t" }, { 20, "u" }, { 21, "v" }, { 22, "w" }, { 23, "x" }, { 24, "y" }, { 25, "z" } });

void prettyPrint(Expression *exp);		//������ӡ��
void simplePrint(Expression *exp);		//�򵥴�ӡ������ӡ������ʽ����
vector<Token> tokenize(string s);		//�ʷ�������
AST *parser(vector<Token>::iterator *pToken);		//�﷨�����������������﷨��
Expression *buildExpression(AST *ast);	//�������ʽ����
void simpleExpression(Expression *exp); //������ʽ���󣬷��lambdaӦ���ܻ�Ϊ���
Expression *tokenizeAndParserAndBuild(string s); //���ַ���Ϊ���ʽ����
Expression *tokenizeAndParserAndBuildAndSimpleExp(string s);	//һ��������
void deBruijnTmpChange(int k, map<string, int> *deBruijnTmp);	// �ɼ�����ȫ����k
Expression *findInDeBruijnTmpOrDeBruijnOrEvn(string s, map<string, int> *deBruijnTmp);	//�����ַ���s�ڿɼ������������������򻷾���Ѱ�ұ��ʽ

void expressionShift(Expression *exp, int d, int c);	//��λ
void replace(int j, Expression *s, Expression *exp);	//����
void betaReduction(Expression *exp);	//beta��Լ
Expression *cps(Expression *exp);	//CPS�任
void evnInit();	//������ʼ��
void deBruijnInit();	//������������ʼ��
void testMode();	//����ģ��
