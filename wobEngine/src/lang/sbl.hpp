#ifndef WOB_SBL
#define WOB_SBL

#include <string_view>
#include <variant>
#include "core/array.hpp"
#include "core/hashmap.hpp"

namespace aes {

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

	struct Atom
	{
		std::string_view src;
	};

	struct List
	{
		Array<struct BaseExp> expressions;
	};

	enum class ExpType
	{
		Atom,
		List
	};

	struct BaseExp
	{
		void setType(ExpType type);
		ExpType type;
		std::variant<Atom, List> value;

		Atom& getAtom();
		List& getList();
	};

	struct SBLLexer
	{
		int c = 0;
		std::string_view source;

		SBLLexer(std::string_view src) : source(src)
		{

		}

		void skipWhite();

		BaseExp parse();
		Atom parseAtom();
	};

	struct VarDecl
	{
		String name;
		Type type;
		BaseExp initExp;
	};

	struct StructDecl
	{
		String name;
		Array<VarDecl> members;
	};

	struct FuncDef
	{
		String name;
		Type returnType;
		Array<VarDecl> args;
		BaseExp body;
	};

	struct Statement;

	struct CompoundStatement
	{
		Array<Statement> statements;
	};

	struct IfStatement
	{
		BaseExp condition;
		BaseExp body;
		BaseExp elseBody;
	};

	enum class StatementType
	{
		Compound,
		If, 
		While
	};

	struct Statement
	{
		StatementType type;
		std::variant<CompoundStatement, IfStatement> data;
	};

	struct IdentifierExp
	{

	};

	struct LiteralExp
	{

	};

	enum class PrimaryType
	{
		Literal,
		Identifier
	};

	struct PrimaryExp
	{
		PrimaryType type;
		std::variant<LiteralExp, IdentifierExp> data;
	};

	struct SBLParser
	{
		struct Error
		{
			String msg;
		};

		struct ErrorHandler
		{
			Array<Error> ErrorStack;
		};

		BaseExp code;
		ErrorHandler errorHandler;

		void reportError(String&& errorMsg);
		void notEnoughArgError(uint current, uint expected);

		StructDecl parseStructDecl(List& data);
		VarDecl parseVarDecl(List& data);
		FuncDef parseFuncDef(List& data);

		Statement parseStatement(BaseExp& exp);
		CompoundStatement parseCompoundStatement(List& lst);
		IfStatement parseIfStatement(List& lst);


		HashMap<String, StructDecl> structs;
		HashMap<String, FuncDef> functions;

		static constexpr const char* getPrimitiveTypeName(Type type);
		Type getTypeFromString(std::string_view str);
	};

}
#endif