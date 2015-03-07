#ifndef _lambda_std_h
#define _lambda_std_h
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
using namespace std;
#endif

class Tokens //������
{
public:
	string *idString; //��ʶ���ַ���
	Tokens *next;
	Tokens() :idString(NULL),next(NULL){};
	~Tokens();
	Tokens(string *s, Tokens *n);
	Tokens(string *s);
	Tokens(const Tokens &token);
};
Tokens::~Tokens()
{
	if (idString != NULL)
	{
		delete idString;
	}
	if (next != NULL)
	{
		delete next;
	}
}

Tokens::Tokens(string *s, Tokens *n)
{
	idString = new string(*s);
	next = n;
}

Tokens::Tokens(const Tokens &t)
{
	if (t.idString != NULL)
	{
		idString = new string(*t.idString);
	}
	else
	{
		idString = NULL;
	}
	if (t.next!=NULL)
	{
		next = new Tokens(*t.next);
	}
	else
	{
		next = NULL;
	}
	
}

Tokens::Tokens(string *s)
{
	Tokens *tokens=new Tokens();
	Tokens *p=tokens;
	for (size_t i = 0; i < s->size(); i++)
	{
		if ((*s)[i] == '(') //������
		{
			p->next = new Tokens(&(s->substr(i, 1)),NULL);
			p = p->next;
		}
		else if ((*s)[i] == ')') //������
		{
			p->next = new Tokens(&(s->substr(i, 1)), NULL);
			p = p->next;
		}
		else if ((*s)[i] == ' ') //�ո����ָ����ã���ʱ����¼
		{
			continue;
		}
		else
		{
			int k = i;
			while (((*s)[k + 1] != ' ') && ((*s)[k + 1] != '(') && ((*s)[k + 1] != ')'))
			{
				k++;
				if (k == s->size())
				{
					break;
				}
			}
			p->next = new Tokens(&(s->substr(i, k - i + 1)), NULL);//��ʶ��
			p = p->next;
			i = k;
		}
	}
	tokens = tokens->next;
	idString = tokens->idString;
	next = tokens->next;
}