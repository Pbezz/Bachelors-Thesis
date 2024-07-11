#include <bits/stdc++.h>
using namespace std;


//Adapted lcs function since we want the strings "lambda0" and "lambda1" to be considered empty
int lcs(string a, string b){
    if(a==""||b==""||a[0]=='l'||b[0]=='l')return 0;
    int dp[a.size()][b.size()];
    dp[0][0]=(int)(a[0]==b[0]);

    for(int i=1;i<a.size();i++){
        dp[i][0]=dp[i-1][0]; if(a[i]==b[0])dp[i][0]=1;
    }
    for(int j=1;j<b.size();j++){
        dp[0][j]=dp[0][j-1]; if(a[0]==b[j])dp[0][j]=1;
    }
    for(int i=1;i<a.size();i++){
        for(int j=1;j<b.size();j++){
            if(a[i]==b[j])dp[i][j]=1+dp[i-1][j-1];
            else dp[i][j]=max(dp[i-1][j],dp[i][j-1]);
        }
    }
    return dp[a.size()-1][b.size()-1];
}

//A state is a pair of either bit strings, lambda0 or lambda1
class State{
    public:
    string mem1,mem2;

    State(string u, string v){
        if(u>v)swap(u,v);
        if(v[0]=='l')swap(u,v);
        mem1=u;
        mem2=v;
    }
    string get1(){
        return mem1;
    }
    string get2(){
        return mem2;
    }
    void print(){
        cout<<mem1<<" "<<mem2<<" ";
    }
};

class MarkovChain{
    public:
    vector<State> estados;
    vector<vector<pair<int,string> >> transitions;
    vector<string> payoff;
    int tamanho;

    MarkovChain(int k, State initial){//memoria e um estado inicial
        tamanho = k;
        estados.push_back(initial);
        vector<pair<int,string> > bruh;
        transitions.push_back(bruh);
        payoff.push_back("");
        expand(initial,0);
    }

    //next -> prepares strings u,v and calls decide function
    //decide-> breaks u,v each in two parts and returns what is left + gain in lcs
    pair<pair<string,string>, int> decide(string a, string b){
        if(a>b)swap(a,b);
        int h=0; char last;
        while(h<min(a.size(),b.size()) && a[h]==b[h]){
            last = a[h];
            h++;
        }
        a=a.substr(h,a.size()-h); if(a=="")a=(string)"lambda"+last;
        b=b.substr(h,b.size()-h); if(b=="")b=(string)"lambda"+last;
        if(a[0]=='l'||b[0]=='l') return {{a,b},h};
        
        if(lcs(a,b)==0){
            if(a.size()>tamanho)a=a.substr(a.size()-tamanho,tamanho);
            if(b.size()>tamanho)b=b.substr(b.size()-tamanho,tamanho);
            return {{a,b},h};
        }else{
            double best=0;
            int besti,bestj;
            for(int i=0;i<a.size();i++){
                for(int j=0;j<b.size();j++){
                    double score = (double)((double)lcs(a.substr(0,i+1),b.substr(0,j+1))/(double)(i+j+2));

                    if(score>best){
                        best = score;
                        besti=i; bestj=j;
                    }
                }
            }
            h+= lcs(a.substr(0,besti+1),b.substr(0,bestj+1));
            last = a[besti];
            a = a.substr(besti+1,a.size()-besti-1);
            b = b.substr(bestj+1,b.size()-bestj-1);
            if(a.size()>tamanho)a=a.substr(a.size()-tamanho,tamanho);
            if(b.size()>tamanho)b=b.substr(b.size()-tamanho,tamanho);
            if(a=="")a=(string)"lambda"+last;
            if(b=="")b=(string)"lambda"+last;
            if(a>b)swap(a,b);
            return {{a,b},h};
        }

    }

    pair<State,int> next(State s, char a, char b){
        string u = s.get1();
        string v = s.get2();
        if(u[0]=='l'&&v[0]=='l'){
            if(a==b)return {State((string)"lambda"+a,(string)"lambda"+a),1};
            else return {State((string)""+a,(string)""+b),0};
        }
        if(u[0]=='l')u="";
        if(v[0]=='l')v="";
        u+=a;
        v+=b;
        pair<pair<string,string>, int> decision = decide(u,v);
        return {State(decision.first.first,decision.first.second),decision.second};
    }

    void expand(State s,int ind){
        for(char a:{'0','1'}){
        for(char b:{'0','1'}){
            pair<State,int> p = next(s,a,b);
            State n = p.first;
            bool exists=false;
            int nind;

            for(int i =0;i<estados.size();i++){
                State u = estados[i];
                if(n.get1()==u.get1()&&n.get2()==u.get2()){
                    exists=true;
                    nind = i;
                    break;
                }
            }
            if(!exists)nind = estados.size();
            string prob="";
            string u = s.get1(),v=s.get2();
            if(u[u.size()-1]!=a)prob+="p1";
            else prob+="p0";
            if(v[v.size()-1]!=b)prob+="*p1";
            else prob+="*p0";
            if(prob=="p1*p0")prob="p0*p1";
            transitions[ind].push_back({nind,prob});
            payoff[ind]+=(to_string(p.second)+prob);
            if(a!='1'||b!='1')payoff[ind]+=" + ";

            if(!exists){
                estados.push_back(n);
                vector<pair<int,string> > bruh;
                transitions.push_back(bruh);
                payoff.push_back("");
                expand(n,estados.size()-1);

            }
        }}
    }

};


int main(){

    State initial("lambda0","lambda0");
    MarkovChain MC(1,initial);//input here the parameter h

    cout<<"P = { ";
    for(int i=0;i<MC.estados.size();i++){
        sort(MC.transitions[i].begin(),MC.transitions[i].end());
        vector<string>line(MC.estados.size(),"0");

        for(int j=0;j<4;j++){
            if(j<3&&MC.transitions[i][j].first==MC.transitions[i][j+1].first){
                if(MC.transitions[i][j].second==MC.transitions[i][j+1].second)line[MC.transitions[i][j].first]="2"+MC.transitions[i][j].second;
                else line[MC.transitions[i][j].first]=MC.transitions[i][j].second + "+" + MC.transitions[i][j+1].second;
                j++;
            }else{
                line[MC.transitions[i][j].first]=MC.transitions[i][j].second;
            }

        }
        cout<<"{";
        for(int j = 0;j<line.size();j++){
            if(j<line.size()-1)cout<<line[j]<<", ";
            else cout<<line[j]<<"}";
        }
        if(i<MC.estados.size()-1)cout<<", ";
        else cout<<"};";
    }
    cout<<endl<<endl;
    cout<<"c= { ";
    for(int i=0;i<MC.estados.size();i++){
        if(i<MC.estados.size()-1)cout<<MC.payoff[i]<<", ";
        else cout<<MC.payoff[i]<<"};";
    }

    cout<<endl<<endl;
    cout<<"{ 1, ";
    for(int i=1;i<MC.estados.size();i++){
        if(i<MC.estados.size()-1)cout<<"0, ";
        else cout<<"0 }";
    }
    cout<<endl<<endl;
    cout<<MC.estados.size()<<endl;


    return 0;   
}
