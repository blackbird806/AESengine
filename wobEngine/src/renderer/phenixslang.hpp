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
			printf("Atom : ");
			printf("%.*s\n", static_cast<int>(value.length()), value.data());
		}

		std::string_view value;
	};

	struct Pair
	{
		void print() const;

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

		std::string_view getAtomValue()
		{
			AES_ASSERT(type == Type::Atom);
			return atom.value;
		}

		Sexp* car()
		{
			AES_ASSERT(type == Type::Pair);
			return pair.car;
		}

		Sexp* cdr()
		{
			AES_ASSERT(type == Type::Pair);
			return pair.cdr;
		}

		Type type;
		union
		{
			Pair pair;
			Atom atom;
		};
	};

	void Pair::print() const
	{
		printf("Pair : \n");
		car->print();
		if (cdr)
			cdr->print();
	}

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
			Sexp* exp = parserAllocator.IAllocator::allocate<Sexp>();
			exp->type = Sexp::Type::Atom;
			exp->atom.value = value;
			return exp;
		}

		Sexp* createPair()
		{
			Sexp* exp = parserAllocator.IAllocator::allocate<Sexp>();
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
			while (isspace(source[c]) && c < source.size())
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

	constexpr TType stringToType(std::string_view str)
	{
		if (str.size() == 3)
			return TType::Int;
		
		if (str.size() == 4)
		{
			if (str[0] == 'v')
			{
				if (str[1] == 'o')
					return TType::Void;
				if (str[3] == '2')
					return TType::Vec2;
				if (str[3] == '3')
					return TType::Vec3;
				if (str[3] == '4')
					return TType::Vec4;
			}
			else if (str[0] == 'm')
			{
				if (str[3] == '2')
					return TType::Mat2;
				if (str[3] == '3')
					return TType::Mat3;
				if (str[3] == '4')
					return TType::Mat4;
			}
			else if (str[0] == 'u')
			{
				return TType::UInt;
			}
		}
		if (str[0] == 'f')
			return TType::Float;
		if (str[0] == 'd')
			return TType::Double;
		if (str[0] == 's')
			return TType::Sampler2D;

		return TType::Undefined;
	}

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

	AttributeType stringToAttributeType(std::string_view str)
	{
		AES_ASSERT(!str.empty());

		if (str.size() == 5)
			return AttributeType::Color;
		if (str.size() == 8)
			return AttributeType::Position;

		return AttributeType::Custom;
	}

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
		Array<FunDef> fnDefs;
		Array<StructDecl> structDecl;
	};

	// assuming that list is a list of at least index items 
	bool assumeListElem(Sexp* list, std::string_view value)
	{
		return list->pair.car->type == Sexp::Type::Atom && list->pair.car->atom.value == value;
	}

	struct SexpWalker
	{
		Sexp* current, *prev;

		void moveNext()
		{
			prev = current;
			current = current->pair.cdr;
		}

		Sexp* popListElem()
		{
			AES_ASSERT(isPair());
			Sexp* val = current->pair.car;
			moveNext();
			return val;
		}

		bool isPair()
		{
			return current && current->type == Sexp::Type::Pair;
		}

		bool isAtom()
		{
			return current && current->type == Sexp::Type::Atom;
		}

		bool matchListElem(std::string_view value)
		{
			AES_ASSERT(isPair());
			bool const val = current->car()->type == Sexp::Type::Atom && current->car()->getAtomValue() == value;
			if (val)
				moveNext();
			return val;
		}
	};

	/*
	*	
	*	fielddecl := (type name attribute *)
		structdecl := (defstruct structname
			(fielddecl) *
			)
	*/

	VarDecl parseField(Sexp* exp)
	{
		SexpWalker walker(exp);
		VarDecl decl;
		auto memberType = walker.popListElem()->getAtomValue();
		auto memberName = walker.popListElem()->getAtomValue();
		decl.name = String(memberName);
		return decl;
	}

	ShaderProgram createShaderProgram(Sexp* exp)
	{
		ShaderProgram shader;

		SexpWalker walker(exp);
		if (walker.isPair())
		{
			if (walker.matchListElem("defstruct"))
			{
				auto const structName = walker.popListElem()->getAtomValue();
				StructDecl newStruct;
				newStruct.name = structName;
				while (walker.current != nullptr)
				{
					Sexp* memberDecl = walker.popListElem();
					AES_ASSERT(memberDecl->type == Sexp::Type::Pair);
					SexpWalker memberDeclWalker(memberDecl);
					auto const memberType = memberDeclWalker.popListElem()->getAtomValue();
					auto const memberName = memberDeclWalker.popListElem()->getAtomValue();
					VarDecl newVar;
					newVar.type = stringToType(memberType);
					newVar.name = memberName;

					while (memberDeclWalker.current != nullptr)
					{
						auto const attribute = memberDeclWalker.popListElem()->getAtomValue();
						newVar.attributes.push(Attribute{ stringToAttributeType(attribute), attribute });
						memberDeclWalker.moveNext();
					}

					newStruct.members.push(std::move(newVar));
				}
				shader.structDecl.push(std::move(newStruct));
			}
		}
		return shader;
	}

	String compileToHLSL(ShaderProgram const& program);
}


#endif