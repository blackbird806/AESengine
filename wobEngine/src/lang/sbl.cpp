#include "sbl.hpp"
#include <cctype>
#include <core/format.hpp>
#include <span>

using namespace aes::sbl;
using namespace std::literals;

void SBLLexer::skipWhite()
{
	while (c < source.size() && isspace(source[c]))
	{
		if (source[c] == '\n')
		{
			currentLine++;
		}
		c++;
	}
}

void SBLLexer::consume(char e)
{
	AES_ASSERT(peek() == e); // TODO real error handling
	c++;
}

char SBLLexer::peek()
{
	skipWhite();
	AES_BOUNDS_CHECK(c >= 0);
	AES_BOUNDS_CHECK(c < source.size());
	return source[c];
}

Node SBLLexer::parse()
{
	Node n;
	skipWhite();
	if (peek() == '(')
	{
		n.setType(NodeType::List);
		n.getList() = parseList();
	}
	else
	{
		n.setType(NodeType::Atom);
		n.getAtom() = parseAtom();
	}
	return n;
}

static bool isValidSymOrLiteralChar(char c)
{
	return std::isalnum(c) || c == '_';
}

SourceLoc SBLLexer::getCurrentLoc()
{
	SourceLoc loc;
	loc.line = currentLine;
	loc.column = currentLine;
	return loc;
}

Atom SBLLexer::parseAtom()
{
	int const wordStart = c;
	while (isValidSymOrLiteralChar(source[c]) && c < source.size())
	{
		c++;
	}
	int const wordLen = c - wordStart;
	Atom atom;
	atom.src = std::string_view(&source[wordStart], wordLen);
	AES_ASSERT(!atom.src.empty());
	atom.loc = getCurrentLoc();
	return atom;
}

List SBLLexer::parseList()
{
	List l;
	l.loc = getCurrentLoc();
	consume('(');
	while (peek() != ')') {
		l.nodes.push(parse());
	}
	consume(')');
	return l;
}

void Node::setType(NodeType type_)
{
	type = type_;
	if (type == NodeType::Atom)
	{
		value.emplace<Atom>();
	}
	else
	{
		value.emplace<List>();
	}
}

Atom& Node::getAtom()
{
	return std::get<Atom>(value);
}

List& Node::getList()
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
	if (data.nodes.size() < 3)
	{
		notEnoughArgError(data.nodes.size(), 3);
		return struct_;
	}

	// 0 is struct keyword
	struct_.name = data.nodes[1].getAtom().src;

	for (int i = 2; i < data.nodes.size(); i++)
	{
		struct_.members.push(parseVarDecl(data.nodes[i].getList()));
	}

	structs.add(struct_.name, struct_);
	return struct_;
}

/*
* <vardecl> ::= (var type name)
*/
VarDecl SBLParser::parseVarDecl(List& data)
{
	VarDecl var;
	if (data.nodes.size() < 3)
	{
		notEnoughArgError(data.nodes.size(), 3);
		return var;
	}

	var.type = getTypeFromString(data.nodes[1].getAtom().src);
	if (data.nodes.size() > 3)
	{
		var.initExp = data.nodes[3];
	}

	if (var.type == Type::Struct)
	{
		//var.structName = stmt.expressions[0].getAtom().src.data();
	}
	var.name = data.nodes[2].getAtom().src;
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
	if (data.nodes.size() < 3)
	{
		notEnoughArgError(data.nodes.size(), 3);
		return func;
	}

	func.returnType = getTypeFromString(data.nodes[1].getAtom().src);
	func.name = data.nodes[2].getAtom().src;

	for (int i = 3; i < data.nodes.size(); i++)
	{
		// todo try parse var, if fail parse compound

		func.args.push(parseVarDecl(data.nodes[i].getList()));
	}
	functions.add(func.name, func);
	return func;
}

Statement SBLParser::parseStatement(Node& exp)
{
	if (exp.type == NodeType::Atom)
	{

	}

	Statement data;

	List& lst = exp.getList();
	if (lst.nodes.size() < 2)
	{
		notEnoughArgError(lst.nodes.size(), 2);
		return data;
	}

	Atom& firstAtom = lst.nodes[0].getAtom();

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
	else if (firstAtom.src == "var"sv)
	{
		data.type = StatementType::VarDecl;
		data.data = parseVarDecl(lst);
	}
	else if (firstAtom.src == "struct"sv)
	{
		data.type = StatementType::StructDecl;
		data.data = parseStructDecl(lst);
	}
	else if (firstAtom.src == "fn"sv)
	{
		data.type = StatementType::FunDef;
		data.data = parseFuncDef(lst);
	}

	return data;
}

CompoundStatement SBLParser::parseCompoundStatement(List& lst)
{
	CompoundStatement compound;

	std::span<Node> nodes(&lst.nodes[1], lst.nodes.size() - 1);
	for (auto& elem : nodes)
	{
		compound.statements.push(parseStatement(elem));
	}

	return compound;
}

IfStatement SBLParser::parseIfStatement(List& lst)
{
	IfStatement stmt;

	if (lst.nodes.size() < 3)
	{
		notEnoughArgError(lst.nodes.size(), 3);
		return stmt;
	}
	stmt.condition = lst.nodes[1];
	stmt.body = parseCompoundStatement(lst.nodes[2].getList());
	stmt.elseBody = parseCompoundStatement(lst.nodes[3].getList());
	return stmt;
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
		if (structs.exist(String(str)))
		{
			return Type::Struct;
		}
	}

	// error
	reportError(format("Type \"{}\" not found.", str));
	return t;
}
