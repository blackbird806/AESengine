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

#include <cctype>

namespace aes::phenix
{
	// front
	struct Atom
	{
		void print() const
		{
			auto sub = value.substr(0, 5);
			printf("Atom : ");
			printf("%.*s\n", static_cast<int>(sub.length()), sub.data());
		}

		std::string_view value;
	};

	struct Pair
	{
		void print() const
		{
			printf("Pair : \n");
			car->print();
			if (cdr)
				cdr->print();
		}

		struct Sexp* car = nullptr, * cdr = nullptr;
	};

	struct Sexp
	{
		enum class Type
		{
			Atom,
			Pair
		};

		void print()
		{
			if (type == Type::Atom)
			{
				atom.print();
			}
			else
			{
				pair.print();
			}
		}

		Type type;
		union
		{
			Pair pair;
			Atom atom;
		};
	};


	struct PhenixFront
	{
		int c = 0;
		std::string_view source;
		StackAllocator parserAllocator;

		PhenixFront(std::string_view src) : source(src), parserAllocator(*context.allocator, 32_kb)
		{

		}

		Sexp* createAtom(std::string_view value)
		{
			Sexp* exp = parserAllocator.create<Sexp>();
			exp->type = Sexp::Type::Atom;
			exp->atom.value = value;
			return exp;
		}

		Sexp* createPair()
		{
			Sexp* exp = parserAllocator.create<Sexp>();
			exp->type = Sexp::Type::Pair;
			exp->pair.car = nullptr;
			exp->pair.cdr = nullptr;
			return exp;
		}

		Sexp* parseList()
		{
			Sexp* list = createPair();

			list->pair.car = parse();
			if (list->pair.car == nullptr)
				return nullptr;
			list->pair.cdr = parseList();

			return list;
		}

		Sexp* parse()
		{
			if (c == source.size())
				return nullptr;
			while (isblank(source[c]) && c < source.size())
			{
				c++;
			}
			if (c == source.size())
				return nullptr;

			if (source[c] == ')')
			{
				c++;
				return nullptr;
			}
			else if (source[c] == '(')
			{
				c++;
				return parseList();
			}
			else if (std::isalnum(source[c]))
			{
				// parse atom
				// get atom value
				int const wordStart = c;
				while (std::isalnum(source[c++]) && c < source.size())
				{
				}
				int const wordLen = c - wordStart;
				return createAtom(std::string_view(&source[wordStart], wordLen - 1));
			}
			return nullptr;
		}
	};

	// end front

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

	// assuming that list is a list of at least index items 
	bool assumeListElem(Sexp* list, std::string_view value)
	{
		return list->pair.car->type == Sexp::Type::Atom && list->pair.car->atom.value == value;
	}

	struct SexpWalker
	{
		Sexp* current;
	};

	ShaderProgram createShaderProgram(Sexp* exp)
	{
		Sexp* currentExp = exp;
		if (currentExp->type == Sexp::Type::Pair)
		{
			if (assumeListElem(currentExp, "defstruct"))
			{
				currentExp = currentExp->pair.cdr;
				auto structName = currentExp->pair.car->atom.value;
			}
		}
	}

	String compileToHLSL(ShaderProgram const& program);
}


#endif