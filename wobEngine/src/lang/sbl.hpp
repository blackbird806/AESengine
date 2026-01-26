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

	struct Statement
	{
		Array<struct Exp> expressions;
	};

	enum class ExpType
	{
		EAtom,
		EStatement
	};

	struct Exp
	{
		void setType(ExpType type);
		ExpType type;
		std::variant<Atom, Statement> value;

		Atom& getAtom();
		Statement& getStatement();
	};

	struct SBLLexer
	{
		int c = 0;
		std::string_view source;

		SBLLexer(std::string_view src) : source(src)
		{

		}

		void skipWhite();

		Exp parse();
		Atom parseAtom();
	};

	struct VarDecl
	{
		String name;
		Type type;
		Exp initExp;
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
		Exp body;
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

		Exp code;
		ErrorHandler errorHandler;

		void reportError(String&& errorMsg);
		void notEnoughArgError(uint current, uint expected);

		StructDecl parseStructDecl(Statement& stmt);
		VarDecl parseVarDecl(Statement& stmt);
		FuncDef parseFuncDef(Statement& stmt);

		HashMap<String, StructDecl> structs;
		HashMap<String, FuncDef> functions;

		static constexpr const char* getPrimitiveTypeName(Type type);
		Type getTypeFromString(std::string_view str);
	};

}
#endif