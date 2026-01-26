#include "phenixslang.hpp"

using namespace aes;
using namespace aes::phenix;

static String evalASTNode(ASTNode* node);

static String evalASTNode(VariableNode* var)
{
	return var->name;
}

static String evalASTNode(LiteralNode* lit)
{
	switch (lit->ltype.type)
	{
	case TType::Int:
		return format("{}", lit->i);

	case TType::UInt:
		return format("{}", lit->u);

	case TType::Float:
		return format("{}f", lit->f);

	case TType::Double:
		return format("{}", lit->d);

	case TType::Vec2:
		return format("float2({}f, {}f)", lit->v2.x, lit->v2.y);

	case TType::Vec3:
		return format("float3({}f, {}f, {}f)", lit->v3.x, lit->v3.y, lit->v3.z);

	case TType::Vec4:
		return format("float4({}f, {}f, {}f, {}f)", lit->v4.x, lit->v4.y, lit->v4.z, lit->v4.w);
	}
	AES_UNREACHABLE();
}

static String evalASTNode(CompoundStatementNode* stmt)
{
	String out("{\n");
	for (ASTNode* node : stmt->statements)
	{
		evalASTNode(node);
	}
	out.append("}\n");
	return out;
}

static String evalASTNode(ReturnStatement* stmt)
{
	String out("return ");
	out.append(evalASTNode(stmt->node));
	return out;
}

static String evalASTNode(BinaryOpNode* exp)
{
	String out;
	out.append(evalASTNode(exp->left));
	switch (exp->op)
	{
	case BinaryOp::Add:
		out.append(" + ");
		break;
	case BinaryOp::Mul:
		out.append(" * ");
		break;
	case BinaryOp::Sub:
		out.append(" - ");
		break;
	case BinaryOp::Div:
		out.append(" / ");
		break;
	case BinaryOp::Set:
		out.append(" = ");
		break;
	}
	out.append(evalASTNode(exp->right));
	return out;
}

static String evalASTNode(ASTNode* node)
{
	switch (node->type)
	{
	case ASTNodeType::Literal:
		return evalASTNode((LiteralNode*)node);
	case ASTNodeType::VarDecl:
		return evalASTNode((VariableNode*)node);
	case ASTNodeType::ReturnStatement:
		return evalASTNode((ReturnStatement*)node);
	case ASTNodeType::CompoundStatement:
		return evalASTNode((CompoundStatementNode*)node);
	case ASTNodeType::BinOp:
		return evalASTNode((BinaryOpNode*)node);
	}
	AES_UNREACHABLE();
}

static const char* getHLSLTypeName(Type const& type)
{
	switch (type.type)
	{
	case TType::Void:
		return "void";
	case TType::Int:
		return "int";
	case TType::UInt:
		return "uint";
	case TType::Float:
		return "float";
	case TType::Double:
		return "double";
	case TType::Vec2:
		return "float2";
	case TType::Vec3:
		return "float3";
	case TType::Vec4:
		return "float4";
	case TType::UserDefined:
		return type.userTypeName.c_str();
	default:
		return "undefined";
	}
	AES_UNREACHABLE();
}


static const char* getHLSLAttributeName(AttributeType type)
{
	switch (type)
	{
	case AttributeType::Color:
		return "COLOR";
	case AttributeType::Position:
		return "SV_POSITION";

	return "undefined";
	}
	AES_UNREACHABLE();
}

String aes::phenix::compileToHLSL(ShaderProgram const& program)
{
	String out;
	for (auto const& str : program.structDecl)
	{
		out.append("struct ");
		out.append(str.name);
		out.append("\n{\n");
		for (auto const& member : str.members)
		{
			out.append('\t');
			out.append(getHLSLTypeName(member.type));
			out.append(' ');
			out.append(member.name);
			String attrs;
			for (auto const& attr : member.attributes)
			{
				if (attr.type == AttributeType::Color || attr.type == AttributeType::Position)
				{
					attrs.append(' ');
					attrs.append(getHLSLAttributeName(attr.type));
					break;
				}
			}
			if (!attrs.empty())
				out.append(" :");
			out.append(std::move(attrs));
			out.append(";\n");
		}
		out.append("}\n\n");
	}

	for (auto const& fn : program.fnDefs)
	{
		out.append(getHLSLTypeName(fn.retType));
		out.append(' ');
		out.append(fn.name);
		out.append('(');

		for (auto const& arg : fn.args)
		{
			out.append(getHLSLTypeName(arg.type));
			out.append(' ');
			out.append(arg.name);
		}
		// Add SV_TARGET here ?
		out.append(")\n{\n");

		for (auto const& stmt : fn.body.statements)
		{
			out.append("\t");
			out.append(evalASTNode(stmt));
			out.append(";\n");
		}
		out.append("}\n");
	}
	return out;
}
