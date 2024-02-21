#ifndef AES_PHENIXSLANG_HPP
#define AES_PHENIXSLANG_HPP

#include "core/aes.hpp"
#include "core/array.hpp"
#include "core/string.hpp"
#include "core/vec2.hpp"
#include "core/vec3.hpp"
#include "core/vec4.hpp"
#include "core/format.hpp"

namespace aes::phenix
{
	enum class ASTNodeType
	{
		BinOp,
		UnOp,
		Literal,
		Variable,
		FnCall,
		ReturnStatement
	};

	struct ASTNode
	{
		ASTNodeType type;
	};

	struct VariableNode : ASTNode
	{
		String name;
	};

	struct FnCallNode : ASTNode
	{
		String name;
		Array<ASTNode*> args;
	};

	enum class BinaryOp
	{
		Add, Sub, Mul, Div, Set, Equ
	};

	struct BinaryOpNode : ASTNode
	{
		BinaryOp op;
		ASTNode *left, *right;
	};

	enum class UnaryOp
	{
		Neg,
	};

	struct UnaryOpNode : ASTNode
	{
		UnaryOp op;
		ASTNode* node;
	};

	struct ReturnStatement : ASTNode
	{
		ASTNode* node;
	};

	enum class Type
	{
		Void,
		Float,
		Int,
		Vec2,
		Vec3,
		Vec4,
		UserDefined
	};

	struct LiteralNode : ASTNode
	{
		LiteralNode() {};
		Type ltype;
		union {
			float f;
			int i;
			vec2 v2;
			vec3 v3;
			vec4 v4;
		};
	};

	enum class AttributeType
	{
		Color,
		Position,
		Custom
	};

	struct Attribute
	{
		String name;
		AttributeType type;
	};

	struct VarDecl
	{
		Type type;
		String name;
		Array<Attribute> attributes;
	};

	struct UniformBufferDecl
	{
		String name;
		Array<VarDecl> members;
	};

	struct Body
	{
		Array<ASTNode*> statements;
	};

	struct FunDef
	{
		String name;
		Type retType;
		Array<VarDecl> args;
		Array<Attribute> attributes;
		Body body;
	};

	String evalASTNode(ASTNode* node);

	String evalASTNode(VariableNode* var)
	{
		return var->name;
	}

	String evalASTNode(LiteralNode* lit)
	{
		switch (lit->ltype)
		{
		case Type::Int:
			return format("{}", lit->i);

		case Type::Float:
			return format("{}f", lit->f);

		case Type::Vec2:
			return format("float2({}f, {}f)", lit->v2.x, lit->v2.y);

		case Type::Vec3:
			return format("float3({}f, {}f, {}f)", lit->v3.x, lit->v3.y, lit->v3.z);

		case Type::Vec4:
			return format("float4({}f, {}f, {}f, {}f)", lit->v4.x, lit->v4.y, lit->v4.z, lit->v4.w);
		}
		AES_UNREACHABLE();
	}

	String evalASTNode(ReturnStatement* stmt)
	{
		String out("return ");
		out.append(evalASTNode(stmt->node));
		return out;
	}

	String evalASTNode(ASTNode* node)
	{
		switch (node->type)
		{
		case ASTNodeType::Literal:
			return evalASTNode((LiteralNode*)node);
		case ASTNodeType::Variable:
			return evalASTNode((VariableNode*)node);
		case ASTNodeType::ReturnStatement:
			return evalASTNode((ReturnStatement*)node);
		}
		AES_UNREACHABLE();
	}

	constexpr const char* getHLSLTypeName(Type type)
	{
		switch (type)
		{
		case Type::Void:
			return "void";
		case Type::Int:
			return "int";
		case Type::Float:
			return "float";
		case Type::Vec2:
			return "float2";
		case Type::Vec3:
			return "float3";
		case Type::Vec4:
			return "float4";
		}
		AES_UNREACHABLE();
	}

	String compileToHLSL(Array<FunDef> const& fnDefs, Array<UniformBufferDecl> const& uniformBuffers)
	{
		String out;
		for (auto const& buff : uniformBuffers)
		{
			out.append("struct ");
			out.append(buff.name);
			out.append("\n{\n");
			for (auto const& member : buff.members)
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
						attrs.append(attr.name);
						break;
					}
				}
				if (!attrs.empty())
					out.append(':');
				out.append(std::move(attrs));
				out.append(";\n");
			}
			out.append("}\n\n");
		}

		for (auto const& fn : fnDefs)
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

}


#endif