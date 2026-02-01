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

BaseExp SBLLexer::parse()
{
	BaseExp expression;
	skipWhite();
	if (c == source.size())
		return expression;

	if (source[c] == '(')
	{
		c++; // skip (
		// parse statement
		expression.setType(ExpType::List);
		while (c < source.size() && source[c] != ')')
		{
			expression.getList().expressions.push(parse());
		}
		c++; // skip )
	}
	else
	{
		expression.setType(ExpType::Atom);
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

void BaseExp::setType(ExpType type_)
{
	type = type_;
	if (type == ExpType::Atom)
	{
		value.emplace<Atom>();
	}
	else
	{
		value.emplace<List>();
	}
}

Atom& BaseExp::getAtom()
{
	return std::get<Atom>(value);
}

List& BaseExp::getList()
{
	return std::get<List>(value);
}

void SBLParser::reportError(String&& errorMsg)
{
	Error err;
	err.msg = std::move(errorMsg);
	errorHandler.ErrorStack.push(std::move(err));
	AES_ASSERT(false);
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
StructDecl SBLParser::parseStructDecl(List& data)
{
	StructDecl struct_;
	if (data.expressions.size() < 3)
	{
		notEnoughArgError(data.expressions.size(), 3);
		return struct_;
	}

	// 0 is struct keyword
	struct_.name = data.expressions[1].getAtom().src;

	for (int i = 2; i < data.expressions.size(); i++)
	{
		struct_.members.push(parseVarDecl(data.expressions[i].getList()));
	}

	structs.add(struct_.name, struct_);
	return struct_;
}

/*
* <vardecl> ::= (type name)
*/
VarDecl SBLParser::parseVarDecl(List& data)
{
	VarDecl var;
	if (data.expressions.size() < 2)
	{
		notEnoughArgError(data.expressions.size(), 2);
		return var;
	}

	var.type = getTypeFromString(data.expressions[0].getAtom().src);
	if (data.expressions.size() > 2)
	{
		var.initExp = data.expressions[2];
	}

	if (var.type == Type::Struct)
	{
		//var.structName = stmt.expressions[0].getAtom().src.data();
	}
	var.name = data.expressions[1].getAtom().src;
	return var;
}

/*
*				arg1	arg2		etc
* (func type name <vardecl> <vardecl> ...
*	(do (Expression)
	))
*/
FuncDef SBLParser::parseFuncDef(List& data)
{
	FuncDef func;
	if (data.expressions.size() < 3)
	{
		notEnoughArgError(data.expressions.size(), 3);
		return func;
	}

	func.returnType = getTypeFromString(data.expressions[1].getAtom().src);
	func.name = data.expressions[2].getAtom().src;

	for (int i = 3; i < data.expressions.size(); i++)
	{
		if (data.expressions[i].getList().expressions[0].getAtom().src == "do"sv)
		{
			func.body = data.expressions[i].getList().expressions[1];
			break;
		}
		func.args.push(parseVarDecl(data.expressions[i].getList()));
	}
	functions.add(func.name, func);
	return func;
}

Statement SBLParser::parseStatement(BaseExp& exp)
{
	if (exp.type == ExpType::Atom)
	{

	}

	Statement data;

	List& lst = exp.getList();
	if (lst.expressions.size() < 2)
	{
		notEnoughArgError(lst.expressions.size(), 2);
		return data;
	}

	Atom& firstAtom = lst.expressions[0].getAtom();

	if (firstAtom.src == "do"sv)
	{
		data.type = StatementType::Compound;
		data.data = parseCompoundStatement(lst);
	}
	else if (firstAtom.src == "if"sv)
	{
		data.type = StatementType::If;
		data.data = parseIfStatement(lst);
	}

	return data;
}

CompoundStatement SBLParser::parseCompoundStatement(List& lst)
{
	CompoundStatement compound;

	for (auto& elem : lst.expressions)
	{
		compound.statements.push(parseStatement(elem));
	}

	return compound;
}

IfStatement SBLParser::parseIfStatement(List& lst)
{
	return IfStatement();
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
