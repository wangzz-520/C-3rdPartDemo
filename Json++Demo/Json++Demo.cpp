#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>

using Json = nlohmann::json;
using namespace std;

struct Student
{
	std::string name;
	int age;
	bool is_student;
	std::vector<std::string> courses;
};

int main()
{
	// read a JSON file
	std::ifstream f("pretty.json");
	Json j = Json::parse(f);

	Student stu;

	// iterate the array
	for (Json::iterator it = j.begin(); it != j.end(); ++it)
	{
		std::cout << it.key() << " : " << it.value() << "\n";
		if (it.key() == "name")
		{
			stu.name = it.value();
		}
		else if (it.key() == "age")
		{
			stu.age = it.value();
		}
		else if (it.key() == "is_student")
		{
			stu.is_student = it.value();
		}
		else if (it.key() == "courses")
		{
			stu.courses = it.value();
		}
	}

	return 0;
}


int main1()
{
	// 创建一个 JSON 对象
	Json root;

	// 添加一些数据
	root["name"] = "John Doe";
	root["age"] = 30;
	root["is_student"] = false;

	// 创建一个 JSON 数组
	Json courses;
	courses.push_back("Math");
	courses.push_back("Physics");
	courses.push_back("Chemistry");

	root["courses"] = courses;

	// 输出 JSON 字符串
	std::cout << std::setw(4) << root << '\n';

	//保存到文件中
	// write prettified JSON to another file
	std::ofstream o("pretty.json");
	o << std::setw(4) << root << std::endl;

	return 0;
}


