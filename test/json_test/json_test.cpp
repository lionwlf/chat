#include"json.hpp"
using json = nlohmann::json; 


#include<vector>
#include<map>
#include<string>
#include<string.h>
#include<iostream>
using namespace std;

void func(){
    // 先来个最基本的
    json js;

    map<string,string> mss;
    mss.insert({"name","wlf"});   //呦呵，pair也插不了
    mss.insert({"age","21"});     //不过这样倒是比用pair要方便

    vector<int> vc = {1,2,3,4,5,6,7,8,9};

    js["name"] = "wlf";
    js["age"] = "21";
    js["path"] = mss;
    js["vc"] = vc;

    string s = js.dump();   //和Python里面的一样

    json js2 = json::parse(s);  //先解析，这里和Python不太一样
    //返回一个JSON的对象，可以看做一个容器
    cout<<js2<<endl;

    // 直接取key-value 
    string name = js2["name"]; 
    cout << "name:" << name << endl;

    // 直接反序列化vector容器 
    vector<int> v = js2["vc"]; 
    for(int val : v) { 
        cout << val << " "; 
    }
    cout << endl;

    // 直接反序列化map容器 
    map<string, string> 
    m2 = js2["path"]; 
    for(auto p : m2) { 
        cout << p.first << " " << p.second << endl; 
    }
    cout << endl;
}


int main(){

    func();

    return 0;
}
