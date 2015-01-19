#include<bits/stdc++.h>
using namespace std;

typedef struct condition
{
        pair<string, string> lhs;
        string op;
        pair<string, string> rhs;
}condition;

typedef struct where 
{
        string optr;
        vector<condition> cond;
}where;

typedef struct query
{
        vector<string> tables;
        vector<pair<string, pair<string, string> > > columns;
        vector<string> colfunc;
        where wclause;
}query;

typedef struct table
{
        string name;
        vector<pair<string, string> > colnames;
        vector<vector<int> > row;
}table;

vector<string> qstr;
query q;
vector<table> tables;
table fint;

string toLower(string str)
{
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
}

string toUpper(string str)
{
        transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
}


vector<string> strSplit(string str, char delim)
{
        string token;
        vector<string> tokens;
        int i;
        for(i=0; i<str.size(); i++)
        {
                if(str[i] != delim)
                {
                        token += str[i];
                }
                else 
                {
                        tokens.push_back(token);
                        token.clear();
                }
        }
        tokens.push_back(token);
        return tokens;
}

pair<string, string> getTCPair(string str)
{
        vector<string> tokens = strSplit(str, '.');
        pair<string, string> p;
        if(tokens.size() == 1)
        {
                p = make_pair("", tokens[0]);
        }
        else if(tokens.size() == 2)
        {
                p = make_pair(tokens[0], tokens[1]);
        }
        return p;
}

pair<string, pair<string, string> > getTCFPair(string str)
{
        pair<string, pair<string, string> > p;
        int i;
        for(i=0; i<str.size(); i++)
        {
                if(str[i] == '(' && str[str.size() - 1] == ')')
                {
                        p.first = toUpper(str.substr(0, i));
                        pair<string, string> p1 = getTCPair(str.substr(i+1, str.size()-i-2));
                        p.second = p1;
                        return p;
                }
        }
        p.first = "";
        pair<string, string> p1 = getTCPair(str);
        p.second = p1;
        return p;
}

void createQuery(string str)
{
        string w;
        int i, word = 0;
        for(i=0; i<str.size(); i++)
        {
                if(str[i] != 32)
                {
                        w += str[i];
                        word = 1;
                }
                else if(word == 1 && str[i] == 32)
                {
                        qstr.push_back(w);
                        word = 0;
                        w.clear();
                }
        }
        qstr.push_back(w);
        w.clear();
        for(i=0; i<qstr.size(); i++)
        {
                if(toLower(qstr[i]) == "select")
                {
                        w += "select ";
                }
                else if(toLower(qstr[i]) == "from")
                {
                        w += " from ";
                }
                else if(toLower(qstr[i]) == "where")
                {
                        w += " where ";
                }
                else if(toLower(qstr[i]) == "and")
                {
                        w += " and ";
                }
                else if(toLower(qstr[i]) == "or")
                {
                        w += " or ";
                }
                else
                {
                        w += qstr[i];
                }
        }
        qstr.clear();
        qstr = strSplit(w, '=');
        w.clear();
        for(i=0; i<qstr.size(); i++)
        {
                w += qstr[i];
                if(i != qstr.size()-1)
                        w += " = ";
        }
        qstr.clear();
        qstr = strSplit(w, ' ');

        string last = qstr[qstr.size()-1];
        last.erase(last.size()-1);
        qstr[qstr.size()-1] = last;
}

void parseQuery()
{
        int i;
        if(toLower(qstr[0]) != "select") 
        {
                cerr << "select error\n";
                exit(0);
        }
        vector<string> tokens = strSplit(qstr[1], ',');
        for(i=0; i<tokens.size(); i++)
        {
                q.columns.push_back(getTCFPair(tokens[i]));
        }
        tokens.clear();

        if (toLower(qstr[2]) != "from") 
        {
                cerr<<"from error\n";
                exit(0);
        }

        tokens = strSplit(qstr[3], ',');
        for(i=0; i<tokens.size(); i++)
        {
                q.tables.push_back(tokens[i]);
        }

        /*        for(i=0; i<q.tables.size(); i++)
                  {
                  cout << q.tables[i] << " ";
                  }cout << endl;
                  for(i=0; i<q.columns.size(); i++)
                  {
                  cout << q.columns[i].first << q.columns[i].second.first << q.columns[i].second.second << " ";
                  }cout << endl;*/
        if(qstr.size() < 5)
        {
                return ;
        }

        if (toLower(qstr[4]) != "where") 
        {
                cerr<<"where error\n";
        }
        where wh;
        for(i=5; i<qstr.size();)
        {
                condition c;
                c.lhs = getTCPair(qstr[i++]);
                c.op = qstr[i++];
                c.rhs = getTCPair(qstr[i++]);
                if(qstr.size()>i && wh.optr == "")
                {
                        wh.optr = qstr[i++];
                }
                wh.cond.push_back(c);
        }
        q.wclause = wh;
        //        cout << q.wclause.optr << endl;
        /*        for(i=0; i<q.wclause.cond.size(); i++)
                  {
                  cout << q.wclause.cond[i].lhs.first << q.wclause.cond[i].lhs.second << " ";
                  cout << q.wclause.cond[i].op << " ";
                  cout << q.wclause.cond[i].rhs.first << q.wclause.cond[i].rhs.second << endl;
                  }*/
}

string clearTrail(string line)
{
        for(int i=0; i<line.size(); i++)
        {
                if(!(line[i] >= 32 && line[i] <= 127))
                {
                        line.erase(i);
                }
        }
        return line;
}

void readFiles()
{
        int i, tcount = 0, j, k, read = 0;
        ifstream file("metadata.txt.txt");
        string line, tname;

        while(getline(file, line))
        {
                line = clearTrail(line);
                if(line.compare("<end_table>") == 0 && read == 1)
                {
                        string table_name = tables[tcount].name + ".csv";
                        ifstream dataFile(table_name.c_str());
                        string line1;
                        while(getline(dataFile, line1))
                        {
                                line1 = clearTrail(line1);
                                vector<string> values = strSplit(line1, ',');
                                for(i=0; i<values.size(); i++)
                                {
                                        if(values[i][0] == '"' && values[i][values[i].size()-1] == '"')
                                        {
                                                values[i].erase(values[i].begin());
                                                values[i].erase(values[i].end() - 1);
                                        }
                                }
                                vector<int> ivalues;
                                for(i=0; i<values.size(); i++)
                                {
                                        ivalues.push_back(atoi(values[i].c_str()));
                                }
                                tables[tcount].row.push_back(ivalues);
                        }
                        dataFile.close();
                        tcount++;
                        read = 0;
                }
                else if(line.compare("<begin_table>") == 0)
                {
                        getline(file, line);
                        line = clearTrail(line);
                        for(i=0; i<q.tables.size(); i++)
                        {
                                if(q.tables[i].compare(line) == 0)
                                {
                                        table t;
                                        tables.push_back(t);
                                        tables[tcount].name = line;
                                        read = 1;
                                        break;
                                }
                        }
                }
                else if(read == 1)
                {
                        pair<string, string> p = getTCPair(line);
                        p.first = tables[tcount].name;               
                        tables[tcount].colnames.push_back(p);
                }
        }
        file.close();

        /*        for(i=0; i<tables.size(); i++)
                  {
                  cout << tables[i].name << endl;
                  for(j=0; j<tables[i].colnames.size(); j++)
                  {
                  cout << tables[i].colnames[j].first <<  tables[i].colnames[j].second << " ";
                  }cout << endl;
                  for(j=0; j<tables[i].row.size(); j++)
                  {
                  for(k=0; k<tables[i].row[j].size(); k++)
                  {
                  cout << tables[i].row[j][k] << " ";
                  }cout << endl;
                  }
                  }*/
}

int getColNo(pair<string, string> col)
{
        int count = 0, i;
        if(col.first != "")
        {
                for(i=0; i<fint.colnames.size(); i++)
                {
                        if(col.first == fint.colnames[i].first && col.second == fint.colnames[i].second)
                        {
                                return i;
                        }
                }
        }
        else
        {
                for(i=0; i<fint.colnames.size(); i++)
                {
                        if(col.second == fint.colnames[i].second)
                        {
                                count++;
                        }
                }
        }
        if(count > 1)
                return -1;
        else
        {
                for(i=0; i<fint.colnames.size(); i++)
                {
                        if(col.second == fint.colnames[i].second)
                        {
                                return i;
                        }
                }
        }
        return -1;
}

int applyOp(int val1, int val2, string op)
{
        if(op == "=" && val1 == val2)
                return 1;

        else if(op == ">" && val1 > val2)
                return 1;

        else if(op == "<" && val1 < val2)
                return 1;

        else if(op == ">=" && val1 >= val2)
                return 1;

        else if(op == "<=" && val1 <= val2)
                return 1;

        else
                return 0;
}

int checkDistinct(vector<int> v1, vector<int> v2)
{
        int i, j,count = 0;
        for(i=0; i<v1.size(); i++)
                for(j=0; j<v2.size(); j++)
                        if(v1[i] == v2[j])
                                count++;
        if(count == v1.size())
                return 0;
        return 1;
}
void print(vector<int> v)
{
        for(int i=0; i<v.size(); i++)
        {
                cout << v[i] << " ";
        }cout << endl;
}
void aggrFunc()
{
        vector<int> f;
        int i,j,k;
        for(i=0; i<q.columns.size(); i++)
        {
                vector<int> f;
                if(q.columns[i].first == "MAX")
                {
                        int max = -1000000000;
                        for(j=0; j<fint.row.size(); j++)
                        {
                                if(fint.row[j][i] > max)
                                {
                                        f = fint.row[j];
                                        max = fint.row[j][i];
                                }
                        }
                        fint.row.clear();
                        fint.row.push_back(f);
                }
                else if(q.columns[i].first == "MIN")
                {
                        int min = 1000000000;
                        for(j=0; j<fint.row.size(); j++)
                        {
                                if(fint.row[j][i] < min)
                                {
                                        f = fint.row[j];
                                        min = fint.row[j][i];
                                }
                        }
                        fint.row.clear();
                        fint.row.push_back(f);
                }
                else if(q.columns[i].first == "SUM")
                {
                        int sum = 0;
                        for(j=0; j<fint.row.size(); j++)
                        {
                                sum += fint.row[j][i];
                        }
                        f.push_back(sum);
                        fint.row.clear();
                        fint.row.push_back(f);
                }
                else if(q.columns[i].first == "AVG")
                {
                        float avg = 0;
                        for(j=0; j<fint.row.size(); j++)
                        {
                                avg += fint.row[j][i];
                        }
                        avg /= fint.row.size();
                        f.push_back(avg);
                        fint.row.clear();
                        fint.row.push_back(f);
                }
                else if(q.columns[i].first == "DISTINCT")
                {
                        map<vector<int>, bool> a;
                        for(j=0; j<fint.row.size(); j++)
                        {
                                if(a[fint.row[j]] == 1)
                                {
                                        fint.row.erase(fint.row.begin()+j);
                                }
                                a[fint.row[j]] = 1;


                        }
                }

        }
}

void executeQuery()
{
        int i, j, k;

        for(i=0; i<tables.size(); i++)
        {
                fint.colnames.insert(fint.colnames.end(), tables[i].colnames.begin(), tables[i].colnames.end());
        }

        fint.row = tables[0].row; 
        for(i=1; i<tables.size(); i++)
        {
                vector<vector<int> > cp;
                for(j=0; j<tables[i].row.size(); j++)
                {
                        for(k=0; k<fint.row.size(); k++)
                        {
                                vector<int> dum = fint.row[k];
                                dum.insert(dum.end(), tables[i].row[j].begin(), tables[i].row[j].end());
                                cp.push_back(dum);
                        }
                }
                fint.row = cp;
        }

        /*for(i=0; i<fint.row.size(); i++)
          {
          for(j=0; j<fint.row[i].size(); j++)
          {
          cout << fint.row[i][j] << " ";
          }cout << endl;
          }*/

        vector<vector<int> > tmp;
        if(q.wclause.cond.size())
        {
                for(i=0; i<q.wclause.cond.size(); i++)
                {
                        int lcolno = getColNo(q.wclause.cond[i].lhs);
                        if(lcolno == -1)
                        {
                                cerr << "column not found\n";
                        }

                        int rcolno = getColNo(q.wclause.cond[i].rhs);
                        if(rcolno == -1)
                        {
                                int rval = atoi((q.wclause.cond[i].rhs.second).c_str());
                                for(j=0; j<fint.row.size(); j++)
                                {
                                        if(applyOp(fint.row[j][lcolno], rval, q.wclause.cond[i].op))
                                        {
                                                tmp.push_back(fint.row[j]);
                                        }
                                }
                        }

                        else 
                        {
                                for(j=0; j<fint.row.size(); j++)
                                {
                                        if(applyOp(fint.row[j][lcolno], fint.row[j][rcolno], q.wclause.cond[i].op))
                                        {
                                                tmp.push_back(fint.row[j]);
                                        }
                                }
                        }
                        if(q.wclause.optr == "AND")
                        {
                                fint.row = tmp;
                                tmp.clear();
                        }
                        else if(q.wclause.optr == "OR")
                        {
                                ;
                        }
                        else
                        {
                                fint.row = tmp;
                        }
                }


        }
        tmp.clear();
        vector<int> colInd;
        if(q.columns[0].second.second == "*")
        {
                q.columns.clear();
                for(i=0; i<fint.colnames.size(); i++)
                {
                        pair<string, pair<string, string> > p = make_pair("", fint.colnames[i]);
                        q.columns.push_back(p);
                }
        }
        for(i=0; i<q.columns.size(); i++)
        {
                int coln = getColNo(q.columns[i].second);
                colInd.push_back(coln);
                q.columns[i].second = fint.colnames[coln];

        }
        fint.colnames.clear();
        for(i=0; i<q.columns.size(); i++)
        {
                fint.colnames.push_back(q.columns[i].second); 
        }
        for(i=0; i<fint.row.size(); i++)
        {
                vector<int> tmp1;
                for(j=0; j<colInd.size(); j++)
                {
                        tmp1.push_back(fint.row[i][colInd[j]]);
                }
                tmp.push_back(tmp1);
        }
        fint.row = tmp;
        aggrFunc();
        if(q.wclause.cond.size())
        {
                for(i=0; i<q.wclause.cond.size(); i++)
                {
                        int repcol = getColNo(q.wclause.cond[0].rhs);
                        if(repcol != -1)
                        {
                                fint.colnames.erase(fint.colnames.begin() + repcol);
                                for(j=0; j<fint.row.size(); j++)
                                {
                                        fint.row[j].erase(fint.row[j].begin() + repcol);
                                }
                        }
                }
        }
        string outstr;
        for(i=0; i<fint.colnames.size(); i++)
        {
                cout << fint.colnames[i].first << "." << fint.colnames[i].second;
                if(i != fint.colnames.size()-1)
                {
                        cout << ",";
                }
        }cout << endl;

        for(i=0; i<fint.row.size(); i++)
        {
                for(j=0; j<fint.row[i].size(); j++)
                {
                        cout << fint.row[i][j];
                        if(j != fint.row[i].size()-1)
                        {
                                cout << ",";
                        }
                }
                if(i != fint.row.size()-1)
                {
                        cout << endl;
                }
        }
}

int main(int argc, char *argv[])
{
        createQuery(argv[1]);
        parseQuery();
        readFiles();
        executeQuery();
        return 0;
}
