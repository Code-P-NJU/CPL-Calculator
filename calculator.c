#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
typedef struct element // 双向链表存储所有的元素
{
    struct element *front;
    struct element *next;
    enum Token
    {
        Integer,
        Float,
        Operator,
        iValue,
        fValue,
        Variable
    } token;
    char input[30];
    int iValue;
    double fValue;
} element;
enum Type
{
    INT,
    FLOAT
};
typedef struct var // 双向链表储存所有赋值
{
    struct var *next;
    char Name[30];
    int iValue;
    double fValue;
    enum Type type;
} var;
int judge = 1; // 语法错误标识,1为ture
char operator[9] = {"+-*/()="};
element *head = NULL, *end = NULL;
var *vhead = NULL;
var *vend = NULL;
void assignment(char *input, enum Type type, double fvalue, int ivalue) // 赋值链表的创建和赋值
{
    {
        for (var *q = vhead; q != NULL; q = q->next)
        {
            if (strcmp(input, q->Name) == 0)
            {
                q->type = type;
                q->fValue = fvalue;
                q->iValue = ivalue;
                return;
            }
        }
        var *p = (var *)malloc(sizeof(var));
        strcpy(p->Name, input);
        p->type = type;
        p->fValue = fvalue;
        p->iValue = ivalue;
        p->next = NULL;
        if (vhead == NULL)
        {
            vhead = p;
            vend = p;
        }
        else
        {
            vend->next = p;
            vend = p;
        }
    }
}
void change(element *p) // 字符串、变量转数字
{
    if (p->token == Integer)
    {
        p->iValue = atoi(p->input);
        p->fValue = (double)atoi(p->input);
        p->token = iValue;
    }
    else if (p->token == Float)
    {
        p->fValue = atof(p->input);
        p->iValue = (int)p->fValue;
        p->token = fValue;
    }
    else if (p->token == Variable)
    {
        if (p->next == NULL || (p->next != NULL && p->next->token == Operator && *(p->next->input) != '='))
        {
            judge = 0;
            for (var *t = vhead; t != NULL; t = t->next)
            {
                if (strcmp(t->Name, p->input) == 0)
                {
                    judge = 1;
                    if (t->type == INT)
                    {
                        p->token = iValue;
                        p->iValue = t->iValue;
                        p->fValue = t->fValue;
                    }
                    else if (t->type == FLOAT)
                    {
                        p->token = fValue;
                        p->iValue = t->iValue;
                        p->fValue = t->fValue;
                    }
                }
            }
        }
    }
}
void Deletelist(element *left, element *right) // 删除特定范围内的element链表
{
    element *p = left, *q = right->next;
    while (p != q)
    {
        element *t = p;
        if (p != head)
        {
            p->front->next = p->next;
        }
        else
        {
            head = p->next;
        }
        if (p != end)
        {
            p->next->front = p->front;
        }
        else
        {
            end = p->front;
        }
        p = p->next;
        free(t);
    }
}
void Deletelist_var() // 删除var链表
{
    while (vhead != NULL)
    {
        var *current = vhead;
        vhead = vhead->next;
        free(current);
    }
}
int tokenizer(element *p) // 词法分析
{
    for (int i = 0; i < 9; i++)
    {
        if (strncmp(p->input, operator+ i, 1) == 0 && strlen(p->input) == 1)
        {
            p->token = Operator;
            return 1;
        };
    }
    int judge_i = 1, judge_f = 1, judge_v = 1, count = 0;
    for (int i = 0; i < strlen(p->input); i++)
    {
        if (*(p->input + i) > '9' || *(p->input + i) < '0')
        {
            judge_i = 0;
        }
        if ((*(p->input + i) > '9' || *(p->input + i) < '0') && *(p->input + i) != '.')
        {
            judge_f = 0;
        }
        if ((*(p->input + i) > '9' || *(p->input + i) < '0') && (*(p->input + i) < 'a' || *(p->input + i) > 'z') && (*(p->input + i) < 'A' || *(p->input + i) > 'Z') && *(p->input + i) != '_')
        {
            judge_v = 0;
        }
        if (*(p->input + i) == '.')
        {
            count++;
        }
    }
    if (judge_i == 1 && (strlen(p->input) == 1 || (strlen(p->input) > 1 && *(p->input) != '0')))
    {
        p->token = Integer;
        return 1;
    }
    else
    {
        if (judge_f == 1 && count == 1)
        {
            int judge_t = 0;
            if ((p->input[strlen(p->input) - 1] == '.' || p->input[0] == '.') == 0)
            {
                char *q = strchr(p->input, '.');
                *q = '\0';
                if (strlen(p->input) == 1 || (strlen(p->input) > 1 && *(p->input) != '0'))
                {
                    p->token = Float;
                    judge_t = 1;
                }
                *q = '.';
            }
            if (judge_t == 1)
            {
                return 1;
            }
        }
        if (judge_v == 1 && (*(p->input) > '9' || *(p->input) < '0'))
        {
            p->token = Variable;
            return 1;
        }
    }
    return 0;
}
void eval(element *l, element *r) // 运算函数（不考虑括号）
{
    element *p;
    for (p = l; p != r; p = p->next) // 处理乘除法
    {
        if (p->token == Operator && *(p->input) != '=')
        {
            if ((p->front->token == iValue || p->front->token == fValue) && (p->next->token == iValue || p->next->token == fValue))
            {
                if (*(p->input) == '*')
                {
                    if (p->front->token == iValue && p->next->token == iValue)
                    {
                        p->next->iValue = (p->front->iValue * p->next->iValue);
                        p->next->fValue = p->next->iValue;
                        if (p->front == l)
                        {
                            l = p->next;
                        }
                        p = p->next;
                        Deletelist(p->front->front, p->front);
                    }
                    else
                    {
                        p->next->token = fValue;
                        p->next->fValue = (p->front->fValue * p->next->fValue);
                        p->next->iValue = (int)p->next->fValue;
                        if (p->front == l)
                        {
                            l = p->next;
                        }
                        p = p->next;
                        Deletelist(p->front->front, p->front);
                    }
                    if (p == r)
                    {
                        break;
                    }
                }
                else if (*(p->input) == '/')
                {
                    if (fabs(p->next->fValue) < 0.00000000000001 && p->next->iValue == 0)
                    {
                        judge = 0;
                        return;
                    }
                    else
                    {
                        if (p->front->token == iValue && p->next->token == iValue)
                        {
                            p->next->iValue = (p->front->iValue / p->next->iValue);
                            p->next->fValue = p->next->iValue;
                            if (p->front == l)
                            {
                                l = p->next;
                            }
                            p = p->next;
                            Deletelist(p->front->front, p->front);
                        }
                        else
                        {
                            p->next->token = fValue;
                            p->next->fValue = (p->front->fValue / p->next->fValue);
                            p->next->iValue = (int)p->next->fValue;
                            if (p->front == l)
                            {
                                l = p->next;
                            }
                            p = p->next;
                            Deletelist(p->front->front, p->front);
                        }
                        if (p == r)
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                judge = 0;
                return;
            }
        }
    }
    for (p = l; p != r; p = p->next) // 处理加减法
    {
        if (p->token == Operator && *(p->input) != '=')
        {
            if ((p->front->token == iValue || p->front->token == fValue) && (p->next->token == iValue || p->next->token == fValue))
            {
                if (*(p->input) == '+')
                {
                    if (p->front->token == iValue && p->next->token == iValue)
                    {
                        p->next->iValue = (p->front->iValue + p->next->iValue);
                        p->next->fValue = p->next->iValue;
                        if (p->front == l)
                        {
                            l = p->next;
                        }
                        p = p->next;
                        Deletelist(p->front->front, p->front);
                    }
                    else
                    {
                        p->next->token = fValue;
                        p->next->fValue = (p->front->fValue + p->next->fValue);
                        p->next->iValue = (int)p->next->fValue;
                        if (p->front == l)
                        {
                            l = p->next;
                        }
                        p = p->next;
                        Deletelist(p->front->front, p->front);
                    }
                    if (p == r)
                    {
                        break;
                    }
                }
                else if (*(p->input) == '-')
                {
                    if (p->front->token == iValue && p->next->token == iValue)
                    {
                        p->next->iValue = (p->front->iValue - p->next->iValue);
                        p->next->fValue = p->next->iValue;
                        if (p->front == l)
                        {
                            l = p->next;
                        }
                        p = p->next;
                        Deletelist(p->front->front, p->front);
                    }
                    else
                    {
                        p->next->token = fValue;
                        p->next->fValue = (p->front->fValue - p->next->fValue);
                        p->next->iValue = (int)p->next->fValue;
                        if (p->front == l)
                        {
                            l = p->next;
                        }
                        p = p->next;
                        Deletelist(p->front->front, p->front);
                    }
                    if (p == r)
                    {
                        break;
                    }
                }
            }
            else
            {
                judge = 0;
                return;
            }
        }
    }
}
void process_assign(element *l, element *r) // 处理赋值语句
{
    element *p, *t = l->front;
    for (p = r; p != l; p = p->front)
    {
        if (*(p->input) == '=')
        {
            if (p != l && p != r)
            {
                if (p->front->token == Variable && p->next->token == iValue)
                {
                    assignment(p->front->input, INT, p->next->fValue, p->next->iValue);
                    p = p->next;
                    Deletelist(p->front->front, p->front);
                    if (p->front == t)
                    {
                        break;
                    }
                }
                else if (p->front->token == Variable && p->next->token == fValue)
                {
                    assignment(p->front->input, FLOAT, p->next->fValue, p->next->iValue);
                    p = p->next;
                    Deletelist(p->front->front, p->front);
                    if (p->front == t)
                    {
                        break;
                    }
                }
                else
                {
                    judge = 0;
                    return;
                }
            }
            else
            {
                judge = 0;
                return;
            }
        }
    }
}
void minus(element *l, element *r) // 处理一个无括号区间内的负号
{
    element *t = l->front;
    for (element *p = r; p != t; p = p->front)
    {
        if (*(p->input) == '-' && (p == l || (p->front->token != iValue && p->front->token != fValue)))
        {
            if (p->next->token == iValue || p->next->token == fValue)
            {
                p = p->next;
                p->fValue = (-1) * p->fValue;
                p->iValue = (-1) * p->iValue;
                Deletelist(p->front, p->front);
            }
            else
            {
                judge = 0;
            }
        }
    }
}
void parentheses() // 处理括号，但是有运算部分，实际上这个叫做process更合适。。。
{
    element *p, *q = end;
    int countR = 0, countL = 0;
    for (p = head; p != NULL; p = p->next)
    {
        if (*(p->input) == ')')
        {
            countR++;
            for (q = p->front; q != NULL; q = q->front)
            {
                if (*(q->input) == '(')
                {
                    countL++;
                    if (q == p->front)
                    {
                        judge = 0;
                        return;
                    }
                    else
                    {
                        minus(q->next, p->front);
                        if (judge == 0)
                        {
                            return;
                        }
                        eval(q->next, p->front);
                        if (judge == 0)
                        {
                            return;
                        }
                        p = p->front;
                        q = q->next;
                        if (p == q)
                        {
                            Deletelist(p->front, p->front);
                            Deletelist(q->next, q->next);
                            break;
                        }
                        else
                        {
                            judge = 0;
                            return;
                        }
                    }
                }
            }
        }
    }
    for (q = q; q != NULL; q = q->front)
    {
        if (*(q->input) == '(')
        {
            break;
        }
    }
    if ((countL == countR && q == NULL && p == NULL) == 0)
    {
        judge = 0;
        return;
    }
    minus(head, end);
    if (judge == 0)
    {
        return;
    }
    eval(head, end);
    if (judge == 0)
    {
        return;
    }
    process_assign(head, end);
    if (judge == 0)
    {
        return;
    }
}
int main()
{
    printf("欢迎使用本计算器\n本计算器支持:\n变量赋值存储和调用,格式为 变量名=表达式\n整数以及浮点数(小数)的四则运算,数据范围与INT型和Double型数据范围一致\n支持英文输入法的小括号\n变量名和数字长度必须小于30(多了程序会出问题,但正常情况下不会那么长,我就不修了)");
    printf("其中,变量由数字、字母、下划线构成且第一位不能为数字(如2a视为非法),整数除法运算自动向下取整(如5 / 2则输出2),整数和浮点数运算时整数自动类型提升为浮点(如5 / 2.0视为5.0/2.0,输出2.500000)\n");
    printf("输出规则:\n若输入变量则输出其被赋予的值,浮点数默认输出小数点后6位,当出现不合法的操作(如调用了未赋值的变量)时输出Error。");
    printf("例如,您可以输入\na = 1     此时输出1\na = a * ( a + a ) - - a / 1.0     此时输出为3.000000且a的值更改为3.000000m若不想改变a的值只要在输入时不输入 'a =' 即可\n");
    printf("输入时每个元素(如运算符、数字、变量)之间用空格隔开,负号也要！(强调！！！)\n输入/exit可退出程序,/definition可查看BNF语言描述的输入定义\n祝您使用愉快——Code_P\n");
    char input[30];
    int END;
    element *qc = NULL;
    while (scanf("%s", input) != EOF)
    {
        int j_e = strcmp(input, "/exit"), j_d = strcmp(input, "/definition");
        if (j_e == 0)
        {
            break;
        }
        else if (j_d == 0)
        {
            printf("<expr>::=(即表达式)\n<number> (即数字是一个表达式)\n| <variable>  (变量也是一个表达式)\n| '(' <expr> ')' (表达式两边加括号还是表达式)\n| <expr> '+' <expr>  (表达式相加还是表达式)\n| <expr> '-' <expr>  (依次类推)\n| <expr> '*' <expr>\n| <expr> '/' <expr>\n| '-' <expr>\n");
            printf("<assignment>::=(即赋值语句) \n<variable> '=' <expr>\n| <variable> '=' <assignment>\n");
        }
        else
        {
            element *pc = (element *)malloc(sizeof(element));
            strcpy(pc->input, input);
            if (head == NULL)
            {
                head = pc;
                pc->next = NULL;
                pc->front = NULL;
                qc = pc;
                end = pc;
            }
            else
            {
                pc->front = qc;
                qc->next = pc;
                pc->next = NULL;
                qc = pc;
                end = pc;
            }
            END = getchar();
            if (END == '\n')
            {
                int judge_token = 1;
                judge = 1;
                for (element *p = head; p != NULL; p = p->next)
                {
                    if (tokenizer(p) == 0)
                    {
                        judge_token = 0;
                    }
                }
                for (element *p = head; p != NULL; p = p->next)
                {
                    change(p);
                }
                if (judge_token * judge)
                {
                    parentheses();
                    if (judge == 1 && head == end)
                    {
                        if (head->token == iValue)
                        {
                            printf("%d\n", head->iValue);
                        }
                        else if (head->token == fValue)
                        {
                            printf("%.6f\n", head->fValue);
                        }
                    }
                    else
                    {
                        printf("Error\n");
                    }
                }
                else
                {
                    printf("Error\n");
                }
                Deletelist(head, end);
            }
        }
    }
    Deletelist_var();
}