#ifndef WOB_SBL
#define WOB_SBL

#include <string_view>
#include <variant>
#include "core/array.hpp"
#include "core/hashmap.hpp"

namespace aes::sbl 
{
	struct SourceLoc
	{
		size_t line;
		size_t column;
		std::string_view filename;
	};

	struct Atom
	{
		std::string_view src;
		SourceLoc loc;
	};

	struct List
	{
		Array<struct Node> nodes;
		SourceLoc loc;
	};

	enum class NodeType
	{
		Atom,
		List
	};

	struct Node
	{
		void setType(NodeType type);
		NodeType type;
		std::variant<Atom, List> value;

		Atom& getAtom();
		List& getList();
	};

	struct SBLLexer
	{
		int c = 0;
		int currentLine = 0;
		int currentColumn = 0;
		std::string_view source;

		SBLLexer(std::string_view src) : source(src)
		{

		}

		SourceLoc getCurrentLoc();

		void skipWhite();

		void consume(char c);
		char peek();

		Node parse();
		Atom parseAtom();
		List parseList();
	};

	enum class Type
	{
		Char,
		Int,
		Uint,
		Bool,
		Float,
		Double,
		Struct,
		Size
	};

	// TODO expression types

	enum class PrimaryType
	{
		Literal,
		Identifier
	};

	struct PrimaryExp
	{
		PrimaryType type;
		Atom atom;
	};

	struct VarDecl
	{
		String name;
		Type type;
		PrimaryExp initExp;
	};

	struct StructDecl
	{
		String name;
		Array<VarDecl> members;
	};

	struct Statement;

	struct CompoundStatement
	{
		Array<Statement> statements;
	};

	struct FuncDef
	{
		String name;
		Type returnType;
		Array<VarDecl> args;
		CompoundStatement body;
	};

	struct IfStatement
	{
		Node condition;
		CompoundStatement body;
		CompoundStatement elseBody;
	};

	struct WhileStatement
	{
		Node condition;
		CompoundStatement body;
	};

	enum class StatementType
	{
		Compound,
		If, 
		While,
		VarDecl,
		StructDecl,
		FunDef
	};

	struct Statement
	{
		StatementType type;
		std::variant<CompoundStatement, 
					IfStatement, 
					WhileStatement,
					VarDecl, 
					StructDecl, 
					FuncDef> data;
	};

	struct Symbol
	{
		String name;
		Type type;
		bool isMutable;
		SourceLoc definedAt;
	};

	class Environment
	{
		HashMap<String, Symbol> variables;
		HashMap<String, StructDecl> structs;
		HashMap<String, FuncDef> functions;
		Environment* parent = nullptr;  // For nested scopes

	public:
		Environment(Environment* p = nullptr) : parent(p) {}

		void define(const String& name, Type type, bool mutable_ = true) 
		{
			if (variables.exist(name)) {
				AES_ASSERT(false);
			}
			variables[name] = { name, type, mutable_ };
		}

		Symbol lookup(const String& name) 
		{
			Symbol sym;
			if (variables.tryFind(name, sym))
			{
				return sym;
			}

			if (parent) 
				return parent->lookup(name);
			AES_ASSERT(false);

		}

		Environment enterScope() {
			return Environment(this);
		}
	};

	struct SBLParser
	{
		struct Error
		{
			String msg;
		};

		struct ErrorHandler
		{
			Array<Error> errorStack;
		};

		Node code;
		ErrorHandler errorHandler;

		void reportError(String&& errorMsg);
		void notEnoughArgError(uint current, uint expected);

		StructDecl parseStructDecl(List& data);
		VarDecl parseVarDecl(List& data);
		FuncDef parseFuncDef(List& data);

		Statement parseStatement(Node& exp);
		CompoundStatement parseCompoundStatement(List& lst);
		IfStatement parseIfStatement(List& lst);
		WhileStatement parseWhileStatement(List& lst);

		static constexpr const char* getPrimitiveTypeName(Type type);
		Type getTypeFromString(std::string_view str);

		HashMap<String, StructDecl> structs;
		HashMap<String, FuncDef> functions;
	};

}
#endif