#include <iostream>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

int main()
{
	XMLDocument* doc = new XMLDocument();

	XMLNode* declaration = doc->InsertFirstChild(doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\""));

	XMLNode* element = doc->InsertEndChild(doc->NewElement("element"));

	XMLElement* sub[3] = { doc->NewElement("sub"), doc->NewElement("sub"), doc->NewElement("sub") };
	for (int i = 0; i < 3; ++i) 
	{
		sub[i]->SetAttribute("attrib", i);
	}
	element->InsertEndChild(sub[2]);

	XMLNode* comment = element->InsertFirstChild(doc->NewComment("comment"));

	element->InsertAfterChild(comment, sub[0]);
	element->InsertAfterChild(sub[0], sub[1]);
	sub[2]->InsertFirstChild(doc->NewText("Text!"));

	XMLElement* sub4 = doc->NewElement("textApproach");

	element->InsertAfterChild(sub[2], sub4);

	XMLElement* sub5 = doc->NewElement("v");
	sub4->InsertFirstChild(sub5);

	sub5->InsertFirstChild(doc->NewText("2"));

	doc->Print();

	doc->SaveFile("./pretty.xml");

	return 0;
}

int main2()
{
	static const char* xml =
		"<?xml version=\"1.0\"?>"
		"<!DOCTYPE PLAY SYSTEM \"play.dtd\">"
		"<information>"
		"	<attributeApproach v='2' />"
		"	<textApproach>"
		"		<v>2</v>"
		"	</textApproach>"
		"</information>";

	XMLDocument doc;
	doc.Parse(xml);

	int v0 = 0;
	int v1 = 0;

	XMLElement* attributeApproachElement = doc.FirstChildElement()->FirstChildElement("attributeApproach");
	attributeApproachElement->QueryIntAttribute("v", &v0);

	XMLElement* textApproachElement = doc.FirstChildElement()->FirstChildElement("textApproach");
	textApproachElement->FirstChildElement("v")->QueryIntText(&v1);

	printf("Both values are the same: %d and %d\n", v0, v1);

	return doc.ErrorID();
}
