#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;
using namespace rapidjson;

typedef map<string, string> MessageMap;

#if defined(__GNUC__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(switch - enum)
#endif

struct MessageHandler : public BaseReaderHandler<UTF8<>, MessageHandler>
{
	MessageHandler() : messages_(), state_(kExpectObjectStart), name_() {}

	bool StartObject() 
	{
		switch (state_) 
		{
		case kExpectObjectStart:
			state_ = kExpectNameOrObjectEnd;
			return true;
		default:
			return false;
		}
	}

	bool String(const char* str, SizeType length, bool)
	{
		switch (state_) 
		{
		case kExpectNameOrObjectEnd:
			name_ = string(str, length);
			state_ = kExpectValue;
			return true;
		case kExpectValue:
			messages_.insert(MessageMap::value_type(name_, string(str, length)));
			state_ = kExpectNameOrObjectEnd;
			return true;
		default:
			return false;
		}
	}

	bool EndObject(SizeType) { return state_ == kExpectNameOrObjectEnd; }

	bool Default() { return false; } // All other events are invalid.

	MessageMap messages_;
	enum State
	{
		kExpectObjectStart,
		kExpectNameOrObjectEnd,
		kExpectValue
	}state_;
	std::string name_;
};

#if defined(__GNUC__)
RAPIDJSON_DIAG_POP
#endif

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

static void ParseMessages(const char* json, MessageMap& messages)
{
	Reader reader;
	MessageHandler handler;
	StringStream ss(json);
	if (reader.Parse(ss, handler))
		messages.swap(handler.messages_);   // Only change it if success.
	else 
	{
		ParseErrorCode e = reader.GetParseErrorCode();
		size_t o = reader.GetErrorOffset();
		cout << "Error: " << GetParseError_En(e) << endl;;
		cout << " at offset " << o << " near '" << string(json).substr(o, 10) << "...'" << endl;
	}
}

int main()
{
	MessageMap messages;

	const char* json1 = "{ \"greeting\" : \"Hello!\", \"farewell\" : \"bye-bye!\" }";
	cout << json1 << endl;
	ParseMessages(json1, messages);

	for (MessageMap::const_iterator itr = messages.begin(); itr != messages.end(); ++itr)
		cout << itr->first << ": " << itr->second << endl;

	cout << endl << "Parse a JSON with invalid schema." << endl;
	const char* json2 = "{ \"greeting\" : \"Hello!\", \"farewell\" : \"bye-bye!\", \"foo\" : {} }";
	cout << json2 << endl;
	ParseMessages(json2, messages);

	return 0;
}


int main1()
{
	////////////////////////////////////////////////////////////////////////////
	// 1. Parse a JSON text string to a document.
	const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
	printf("Original JSON:\n %s\n", json);

	Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

	// In-situ parsing, decode strings directly in the source string. Source must be string.
	char buffer[sizeof(json)];
	memcpy(buffer, json, sizeof(json));
	if (document.ParseInsitu(buffer).HasParseError())
		return 1;

	printf("\nParsing to document succeeded.\n");

	////////////////////////////////////////////////////////////////////////////
	// 2. Access values in document. 

	printf("\nAccess values in document:\n");
	assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

	assert(document.HasMember("hello"));
	assert(document["hello"].IsString());
	printf("hello = %s\n", document["hello"].GetString());

	// Since version 0.2, you can use single lookup to check the existing of member and its value:
	Value::MemberIterator hello = document.FindMember("hello");
	assert(hello != document.MemberEnd());
	assert(hello->value.IsString());
	assert(strcmp("world", hello->value.GetString()) == 0);
	(void)hello;

	assert(document["t"].IsBool());     // JSON true/false are bool. Can also uses more specific function IsTrue().
	printf("t = %s\n", document["t"].GetBool() ? "true" : "false");

	assert(document["f"].IsBool());
	printf("f = %s\n", document["f"].GetBool() ? "true" : "false");

	printf("n = %s\n", document["n"].IsNull() ? "null" : "?");

	assert(document["i"].IsNumber());   // Number is a JSON type, but C++ needs more specific type.
	assert(document["i"].IsInt());      // In this case, IsUint()/IsInt64()/IsUint64() also return true.
	printf("i = %d\n", document["i"].GetInt()); // Alternative (int)document["i"]

	assert(document["pi"].IsNumber());
	assert(document["pi"].IsDouble());
	printf("pi = %g\n", document["pi"].GetDouble());

	{
		const Value& a = document["a"]; // Using a reference for consecutive access is handy and faster.
		assert(a.IsArray());
		for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
			printf("a[%d] = %d\n", i, a[i].GetInt());

		int y = a[0].GetInt();
		(void)y;

		// Iterating array with iterators
		printf("a = ");
		for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
			printf("%d ", itr->GetInt());
		printf("\n");
	}

	// Iterating object members
	static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
	for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
		printf("Type of member %s is %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);

	////////////////////////////////////////////////////////////////////////////
	// 3. Modify values in document.

	// Change i to a bigger number
	{
		uint64_t f20 = 1;   // compute factorial of 20
		for (uint64_t j = 1; j <= 20; j++)
			f20 *= j;
		document["i"] = f20;    // Alternate form: document["i"].SetUint64(f20)
		assert(!document["i"].IsInt()); // No longer can be cast as int or uint.
	}

	// Adding values to array.
	{
		Value& a = document["a"];   // This time we uses non-const reference.
		Document::AllocatorType& allocator = document.GetAllocator();
		for (int i = 5; i <= 10; i++)
			a.PushBack(i, allocator);   // May look a bit strange, allocator is needed for potentially realloc. We normally uses the document's.

		// Fluent API
		a.PushBack("Lua", allocator).PushBack("Mio", allocator);
	}

	// Making string values.

	// This version of SetString() just store the pointer to the string.
	// So it is for literal and string that exists within value's life-cycle.
	{
		document["hello"] = "rapidjson";    // This will invoke strlen()
		// Faster version:
		// document["hello"].SetString("rapidjson", 9);
	}

	// This version of SetString() needs an allocator, which means it will allocate a new buffer and copy the the string into the buffer.
	Value author;
	{
		char buffer2[10];
		int len = sprintf(buffer2, "%s %s", "Milo", "Yip");  // synthetic example of dynamically created string.

		author.SetString(buffer2, static_cast<SizeType>(len), document.GetAllocator());
		// Shorter but slower version:
		// document["hello"].SetString(buffer, document.GetAllocator());

		// Constructor version: 
		// Value author(buffer, len, document.GetAllocator());
		// Value author(buffer, document.GetAllocator());
		memset(buffer2, 0, sizeof(buffer2)); // For demonstration purpose.
	}
	// Variable 'buffer' is unusable now but 'author' has already made a copy.
	document.AddMember("author", author, document.GetAllocator());

	assert(author.IsNull());        // Move semantic for assignment. After this variable is assigned as a member, the variable becomes null.

	////////////////////////////////////////////////////////////////////////////
	// 4. Stringify JSON

	printf("\nModified JSON with reformatting:\n");
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
	puts(sb.GetString());
	return 0;
}