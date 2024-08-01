#include <iostream>
#include <fstream>
#include "core/aes.hpp"
#include "core/debug.hpp"
#include "core/uniquePtr.hpp"
#include "core/dragon4.hpp"

#include "renderer/phenixslang.hpp"

using namespace aes;

int main()
{
	auto streamSink = aes::makeUnique<aes::StreamSink>(std::cout);
	aes::Logger::instance().addSink(streamSink.get());
#ifdef __vita__
	auto debugScreenSink = aes::makeUnique<aes::PsvDebugScreenSink>(file);
	aes::Logger::instance().addSink(debugScreenSink.get());
	std::ofstream file("ux0:/log/aeslog.txt");
	auto fileSink = aes::makeUnique<aes::StreamSink>(file);
	aes::Logger::instance().addSink(fileSink.get());
#else
#endif

	//test_RHI();
	Array<phenix::FunDef> fnDefs;
	Array<phenix::StructDecl> bufferDecl;
	Array<phenix::VarDecl> members;
	phenix::VarDecl pos = { phenix::TType::Vec4, String("position") };
	pos.attributes.push(phenix::Attribute{ phenix::AttributeType::Position });
	members.push(pos);
	phenix::VarDecl col = { phenix::TType::Vec4, String("color") };
	col.attributes.push(phenix::Attribute{ phenix::AttributeType::Color });
	members.push(col);

	bufferDecl.push(phenix::StructDecl{
		String("VS_OUTPUT"),
		members
		});
	Array<phenix::VarDecl> mainArgs;
	phenix::VarDecl mainParam1;
	mainParam1.name = "input";
	mainParam1.type = phenix::TType::UserDefined;
	mainParam1.type.userTypeName = "VS_OUTPUT";
	mainArgs.push(mainParam1);
	Array<phenix::Attribute> mainAttrs;
	Array<phenix::ASTNode*> stmts;
	phenix::LiteralNode lit;
	lit.type = phenix::ASTNodeType::Literal;
	lit.ltype = phenix::TType::Vec4;
	lit.v4 = vec4(2.1, 1.0, 0.5634654, 545.32132);

	phenix::BinaryOpNode mulNode;
	mulNode.type = phenix::ASTNodeType::BinOp;
	mulNode.op = phenix::BinaryOp::Mul;
	mulNode.left = &lit;
	mulNode.right = &lit;

	phenix::ReturnStatement ret;
	ret.type = phenix::ASTNodeType::ReturnStatement;
	ret.node = &mulNode;
	stmts.push(&ret);
	phenix::CompoundStatementNode body;
	body.type = phenix::ASTNodeType::CompoundStatement;
	body.statements = stmts;
	fnDefs.push({ String("main"), phenix::TType::Vec4, mainArgs, mainAttrs, body });
	phenix::ShaderProgram program{ fnDefs, bufferDecl };

	String generated = phenix::compileToHLSL(program);
	AES_LOG("code generated\n{}", generated.c_str());
	
	return 0;
}
