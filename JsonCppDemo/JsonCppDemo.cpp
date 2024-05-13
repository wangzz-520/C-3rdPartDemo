#include <iostream>
#include <json/json.h>
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	Json::Reader read;
	Json::Value root;
	//读取json文件
	ifstream in("test.json");
	//解析到root
	read.parse(in, root);
	//按键值对读取
	Json::Value::Members keys = root.getMemberNames();
	for (int i = 0; i < keys.size(); i++)
	{
		cout << keys.at(i) << " : ";
		if (root[keys[i]].isString()) {
			cout << root[keys[i]].asString();
		}
		if (root[keys[i]].isInt()) {
			cout << root[keys[i]].asInt();
		}
		else if (root[keys[i]].isArray())
		{
			cout << "[ ";
			for (int j = 0; j < root[keys[i]].size(); j++) 
			{
				Json::Value item = root[keys[i]][j];
				if (item.isString())
				{
					cout << item.asString() << ", ";
				}
				else if (item.isObject()) 
				{
					cout << "{ " << endl;;
					Json::Value::Members subItems = item.getMemberNames();
					for (int k = 0; k < subItems.size(); k++) 
					{
						cout << subItems.at(k) << " : " << item[subItems[k]] << endl;
					}
					cout << "}" << endl;
				}
			}
			cout << "]";
		}
		cout << endl;
	}

	return 0;
}

int main2()
{
	Json::Value root;

	// 添加一些数据
	root["name"] = "John Doe";
	root["age"] = 30;
	root["sex"] = "male";

	// 创建一个 JSON 数组
	root["courses"].append("Math");
	root["courses"].append("Physics");
	root["courses"].append("Chemistry");

	//新建两个value对象，作为calssmate的值
	Json::Value classmate1;
	classmate1["name"] = "lisi";
	classmate1["age"] = 20;
	classmate1["sex"] = "male";

	Json::Value classmate2;
	classmate2["name"] = "wangwu";
	classmate2["age"] = 20;
	classmate2["sex"] = "male";

	root["classmate"].append(classmate1);
	root["classmate"].append(classmate2);

	//输出带格式的字符串
	Json::StyledWriter sw;

	//输出到文件
	std::ofstream os;
	os.open("test.json", ios::out);
	if (!os.is_open())
		cout << "can not find or create test.json" << endl;
	os << sw.write(root);
	os.close();

	return 0;
}
