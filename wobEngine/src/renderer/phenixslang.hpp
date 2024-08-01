#ifndef AES_PHENIXSLANG_HPP
#define AES_PHENIXSLANG_HPP

#include "core/aes.hpp"
#include "core/array.hpp"
#include "core/uniquePtr.hpp"
#include "core/string.hpp"
#include "core/vec2.hpp"
#include "core/vec3.hpp"
#include "core/vec4.hpp"
#include "core/format.hpp"

namespace aes::phenix
{
	enum class ASTNodeType
	{
		Undefined,
		BinOp,
		UnOp,
		Literal,
		Variable,
		FnCall,
		ReturnStatement,
		CompoundStatement
	};

	struct ASTNode
	{
		ASTNodeType type = ASTNodeType::Undefined;
		virtual ~ASTNode()
		{
		}
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

	enum class TType
	{
		Undefined,
		Void,
		Float,
		Double,
		Int,
		UInt,
		Vec2,
		Vec3,
		Vec4,
		Mat2,
		Mat3,
		Mat4,
		Sampler2D,
		UserDefined
	};

	struct Type
	{
		Type() : type(TType::Undefined) {}
		Type(TType t) : type(t) {}

		TType type;
		String userTypeName;
	};

	struct LiteralNode : ASTNode
	{
		LiteralNode() { f = 0.0f; };
		Type ltype = TType::Float;
		union {
			float f;
			double d;
			int32_t i;
			uint32_t u;
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
		AttributeType type;
		String name;
	};

	struct VarDecl
	{
		Type type;
		String name;
		Array<Attribute> attributes;
	};

	struct StructDecl
	{
		String name;
		Array<VarDecl> members;
	};

	struct CompoundStatementNode : ASTNode
	{
		Array<ASTNode*> statements;
	};

	struct FunDef
	{
		String name;
		Type retType;
		Array<VarDecl> args;
		Array<Attribute> attributes;
		CompoundStatementNode body;
	};

	struct ShaderProgram
	{
		Array<FunDef> const& fnDefs;
		Array<StructDecl> const& structDecl;
	};

	String compileToHLSL(ShaderProgram const& program);
}


#endif