#include "sbl.hpp"
#include <cctype>
#include <core/format.hpp>

using namespace aes;
using namespace std::literals;

void SBLLexer::skipWhite()
{
	while (c < source.size() && isspace(source[c]))
	{
		c++;
	}
}

Exp SBLLexer::parse()
{
	Exp expression;
	skipWhite();
	if (c == source.size())
		return expression;
	// TODO remove loop do full recursive with parse astnode specific functions
	if (source[c] == '(')
	{
		c++; // skip (
		// parse statement
		expression.setType(ExpType::EStatement);
		while (c < source.size() && source[c] != ')')
		{
			expression.getStatement().expressions.push(parse());
		}
		c++; // skip )
	}
	else
	{
		expression.setType(ExpType::EAtom);
		expression.getAtom() = parseAtom();
	}
	skipWhite();
	return expression;
}

Atom SBLLexer::parseAtom()
{
	// parse atom
	// get atom value
	int const wordStart = c;
	while (std::isalnum(source[c]) && c < source.size())
	{
		c++;
	}
	int const wordLen = c - wordStart;
	Atom atom;
	atom.src = std::string_view(&source[wordStart], wordLen);
	return atom;
}

void Exp::setType(ExpType type_)
{
	type = type_;
	if (type == ExpType::EAtom)
	{
		value.emplace<Atom>();
	}
	else
	{
		value.emplace<Statement>();
	}
}

Atom& Exp::getAtom()
{
	return std::get<Atom>(value);
}

Statement& Exp::getStatement()
{
	return std::get<Statement>(value);
}

void SBLParser::reportError(String&& errorMsg)
{
	Error err;
	err.msg = std::move(errorMsg);
	errorHandler.ErrorStack.push(std::move(err));
}

void SBLParser::notEnoughArgError(uint current, uint expected)
{
	reportError(format("Struct declaration expect at least {} args, got {}", current, expected));
}

/* (struct name
	<vardecl>
	<vardecl>)
* 
*/
StructDecl SBLParser::parseStructDecl(Statement& stmt)
{
	StructDecl struct_;
	if (stmt.expressions.size() < 3)
	{
		notEnoughArgError(stmt.expressions.size(), 3);
		return struct_;
	}

	// 0 is struct keyword
	struct_.name = stmt.expressions[1].getAtom().src;

	for (int i = 2; i < stmt.expressions.size(); i++)
	{
		struct_.members.push(parseVarDecl(stmt.expressions[i].getStatement()));
	}

	structs.add(struct_.name, struct_);
	return struct_;
}

/*
* <vardecl> ::= (type name)
*/
VarDecl SBLParser::parseVarDecl(Statement& stmt)
{
	VarDecl var;
	if (stmt.expressions.size() < 2)
	{
		notEnoughArgError(stmt.expressions.size(), 2);
		return var;
	}

	var.type = getTypeFromString(stmt.expressions[0].getAtom().src);
	if (stmt.expressions.size() > 2)
	{
		var.initExp = stmt.expressions[2];
	}

	if (var.type == Type::Struct)
	{
		//var.structName = stmt.expressions[0].getAtom().src.data();
	}
	var.name = stmt.expressions[1].getAtom().src;
	return var;
}

/*
*				arg1	arg2		etc
* (func type name <vardecl> <vardecl> ...
*	(do (Expression)
	))
*/
FuncDef SBLParser::parseFuncDef(Statement& stmt)
{
	FuncDef func;
	if (stmt.expressions.size() < 3)
	{
		notEnoughArgError(stmt.expressions.size(), 3);
		return func;
	}

	func.returnType = getTypeFromString(stmt.expressions[1].getAtom().src);
	func.name = stmt.expressions[2].getAtom().src;

	for (int i = 3; i < stmt.expressions.size(); i++)
	{
		if (stmt.expressions[i].getStatement().expressions[0].getAtom().src == "do"sv)
		{
			func.body = stmt.expressions[i].getStatement().expressions[1];
			break;
		}
		func.args.push(parseVarDecl(stmt.expressions[i].getStatement()));
	}
	functions.add(func.name, func);
	return func;
}

constexpr const char* SBLParser::getPrimitiveTypeName(Type type)
{
	constexpr const char* primitiveTypeNames[(int)Type::Size] = { "char", "int", "uint", "bool", "float", "double" };
	return primitiveTypeNames[(int)type];
}

Type SBLParser::getTypeFromString(std::string_view str)
{
	// TODO clean primitives
	static HashMap<std::string_view, Type> const primitiveTypeMap = []() {
		HashMap<std::string_view, Type> map(16);
		map.add("char", Type::Char);
		map.add("int", Type::Int);
		map.add("uint", Type::Uint);
		map.add("float", Type::Float);
		map.add("double", Type::Double);
		map.add("bool", Type::Bool);
		return map;
		}();
	
	Type t;
	if (primitiveTypeMap.tryFind(str, t))
	{
		return t;
	}
	else // type is not a primitive, check user defined ones
	{
		if (structs.exist(str))
		{
			return Type::Struct;
		}
	}

	// error
	reportError(format("Type \"{}\" not found.", str));
	return t;
}
