#pragma once

namespace jlang
{
	enum class VarType
	{
		Struct,
		Int32,
		Pointer
	};

	struct AstNode
	{
		virtual ~AstNode() = default;
	}
}
