#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <vector>

using namespace std;

struct Node;
struct Node
{
    Node* r;
    Node* l;
    Node* next;
    int size;
    string key;
    Node(int n = 0, char k = '\0');
    Node(int n = 0, string k = NULL);

};

struct tree
{
    Node* first;//указывает на первый элемент верхниго уровня
    Node* last; // на последние элемент верхниго уровня
    tree();//
};

tree::tree() { first = NULL; last = NULL; }

Node::Node(int n, char k) //конструктор для листяв
{
    key = '\0';
    size = n;
    key[0] = k;
    r = NULL; l = NULL;
    next = NULL;
}

Node::Node(int n, string k) //конструктор для осталных вершин , (дерево идеот от листяв к корню)
{
    size = n; //чистата встречаемости символа или строки
    key = k;// символ или строка  сответствоеше вершине
    r = NULL; l = NULL; // указател на потомков ( символы входяшие в строку )
    next = NULL; // указател на другие вершины
}


void Add(tree* b, int n, char key) //функция добавления вершины в дерево
{
    Node* p = new Node(n, key);
    if (b->first == NULL)
    {
        b->first = p;
        b->last = p;
        return;

    }
    b->last->next = p;
    b->last = p;
};

void out(tree* b)// вывод дерево
{
    int u = 0;
    Node* tmp;
    tmp = b->first;
    while (tmp)
    {
        cout << tmp->key <<'-'<< tmp->size<<' ';
        tmp = tmp->next;

    }
}

void swap(Node* a, Node* b, tree* c)// перестановка вершин
{
    Node* pred = c->first; // итератор

    if (a != c->first) {
        while (pred->next != a)// поиск элемента 'a'
            pred = pred->next;
        pred->next = b;
    }
    if (a == c->first) c->first = b; //а меняется с b (зависимости от положении а)
    if (b != c->last) {
        a->next = b->next;
    }
    if (b == c->last)
    {
        c->last = a; a->next = NULL;
    }
    b->next = a;


}


/*функция  формирует код символа по дереву*/
void search(std::map<char, vector<bool>>& mp, char k, Node* tmp)
{
    if (tmp->l || tmp->r)
    {

        if (tmp->l && tmp->l->key.find(k) != -1)
        {
            mp[k].push_back(0); tmp = tmp->l;
        };
        if (tmp->r && tmp->r->key.find(k) != -1)
        {

            mp[k].push_back(1); tmp = tmp->r;
        }
        search(mp, k, tmp);

    }


}

void newnode(tree* b)

{   // создаем предка для двух узлов с наименьшим весом
    Node* beg = b->first;
    Node* p = new Node(beg->size + beg->next->size,
        beg->key + beg->next->key);//из первых верщин на текушем уровне создается новая вершина уровнем выше

    p->l = beg;
    p->r = beg->next;

    Node* tmp = b->first->next;
    // помещаем его на нужную позицию
    while (tmp != b->last && tmp->next->size < p->size)
        tmp = tmp->next; // проходим по списку и сравнивем вес новое вершины со следуше вершины всписке
    p->next = tmp->next; tmp->next = p;
    if (tmp == b->last) b->last = p;
    if (!(b->first->next->next)) b->first = p;
    else
        b->first = b->first->next->next;
}

void to_encode()
{
   int asc[256];
    int n = 0;
    for (int i = 0; i < 256; i++) asc[i] = 0;
    fstream fc("исходный текст.txt");
    char sim;
    fc >> sim;
    while (fc) // заполняем ассоциированный массив
    {
        asc[(int)sim]++;
        fc.get(sim);

    };
    tree* b = new tree;
    n = 0;
    map <char, vector<bool>> mp; // в мапы будем заносит код символов
    fstream fd("код.txt", ios::out);
    // файл для записи частот исходнго текста
    fstream fd1("частоты.txt", ios::out);
    /* ненулевые элементы заносим в листья дерева, в ключи мапы и в файл*/
    fd1 << ' ';
    for (int i = 0; i < 256; i++)

    {
        if (asc[i] != 0) //проходим по моссиву
        {
            vector <bool> vec;
            mp.insert(pair<char, vector<bool>>(char(i), vec));
            Add(b, asc[i], (char)i);
            fd1 << ' '; fd1 << char(i); fd1 << asc[i];
            n++;
        }
    }
    for (int i = 0; i <= n - 2; i++)
    {

        Node* tmp = b->first;
        for (int j = 0; j <= n - 2; j++) // сортируем листья дерева
        {

            if (tmp->size > tmp->next->size)
            {
                Node* t1 = tmp->next;
                swap(tmp, tmp->next, b);
                tmp = t1;
            }
            tmp = tmp->next;

        }
    }
    while (b->first->next != NULL)// добавляем потомка дерево пока это возможно
    {
        newnode(b);
    }
    map<char, vector<bool>>::iterator it = mp.begin();
    Node* tmp = b->first;
    // заполняем мапу кодами символов
    while (it != mp.end())
    {
        search(mp, it->first, tmp);
        /*cout << it->first<<'-';
        for (int i =0 ; i < mp[it->first].size(); i++)
            cout << mp[it->first][i];
        cout << endl;*/
        it++;

    }
    char buf = 0;
    fc.close();
    fc.open("исходный текст.txt", ios::in);

    it = mp.begin();
    int siz = 7;
    fc.get(sim);
    buf = 0; // начиная справа, формируем символ из получившихся кодов

    while (fc)
    {
        it = mp.find(sim); //находим код символа в мапе
        for (int i = 0; i < mp[it->first].size(); i++)
        {
            buf = buf | mp[it->first][i] << siz; // формируем 8 битные символ из кодов символов
            /*cout << mp[it->first][i];*/
            siz--;
            if (siz < 0) // заносим 8 битные символ в текстовые фаиьл
            {
                siz = 7; /*cout << (int)buf << endl;*/ fd << buf; buf = 0;

            }
        }
        fc.get(sim);
    }
    if (siz != 7) {
        fd << buf; /*cout << (int)buf<<endl;*/
    } // заносим последний (неполный) символ

    else siz = -1; //если неполных нет, то незначащие нули в конце не учитываем
    fd.close();
    fd1.seekp(0, ios::beg);
    //cout << siz + 1;
    fd1 << siz + 1;//заносим в файл количество незначащих нулей
    fd1.close(); fd.close();
}

void to_recover_text(tree* b, int t, int &weight)
{

    fstream fk("итог.txt", ios::out);
    ifstream fd("код.txt", ios::binary);
    Node* tmp = b->first;
    char buf, bu;
    fd.get(buf);// подсчет числа символов
    int pr = 0;
    bu = 0;
    while (fd)
    {
        if (!(fd.get(bu)))  pr = 1;// проверяем является ли текущий символ последним


        for (int i = 7; i >= 0; i--)
        {
            if (((int)buf!=13||(int)bu!=10)&&(pr == 0 || i >= t))// для байта восстанавливаем символы текста
            // в последнем остананвливаемся на незначащих нулях // 13 и 10 символы тобуляцы
            {
                if (buf & 1 << i) { // если в символе 1 идем по дереву в право
                    tmp = tmp->r; /*cout << '1';*/
                }
                else { // иначе в лево
                    tmp = tmp->l; /*cout << '0';*/
                }
                if (!(tmp->l))
                {
                    fk << tmp->key; tmp = b->first;/*cout << endl*/ //найдены символ заносим в восстановленый текст
                }

            }
        }
        buf = bu;
        /*cout << (int)  buf << endl;*/ weight++;



    }


}


bool proof() // проверяет совпадает символы в фаилах
{
    fstream fc("итог.txt");
    fstream fs("исходный текст.txt");
    char sim, sim1;
    fc.get(sim); fs.get(sim1);
    while (fs || fc)
    {
        if (sim != sim1) return false;
        fc.get(sim); fs.get(sim1);


    }
    return true;
}

void weight(int t, int r)
{

    cout << "исходный файл -" << r <<" бит"<<  endl;
    cout << "сжатый файл -" << t<<" бит" << endl;

}

int to_decode() {

    tree* b = new tree;
    char key; int size;
    int n = 0;
    fstream fc("частоты.txt");
    int t;
    fc >> t; // считываем количество незначащих нулей
    /*cout << t << endl;*/
    fc.seekg(1, ios::cur);
    fc.get(key);
    fc>>size;
    int weight1=size;
    // считываем символы исходного текста и их частоты и добавляем их в листия
    while (fc)
    {
        n++;
        Add(b, size, key);
        fc.seekg(1, ios::cur);
        fc.get(key);
        fc >> size;
        if (fc) { weight1 += size; }
    }

    // формируем дерево
    for (int i = 0; i <= n - 2; i++)
    {

        Node* tmp = b->first;
        for (int j = 0; j <= n - 2; j++)
        {

            if (tmp->size > tmp->next->size)
            {
                Node* t1 = tmp->next;
                swap(tmp, tmp->next, b);
                tmp = t1;
            }
            tmp = tmp->next;

        }
    }
    while (b -> first->next!= NULL)
    {
        newnode(b);

    }
    fc.close();


    fc.close(); int mas = 0;
    to_recover_text(b,t, mas);// декодируем текст

    fc.close();
    if (proof()) cout << "совпали" << endl;// проверяем  правильность декодирования
    else cout << "не совпали" << endl;
    weight(mas, weight1);
}

int main()
{
    to_encode();
    to_decode();
}

