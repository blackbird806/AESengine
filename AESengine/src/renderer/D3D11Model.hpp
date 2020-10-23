#ifndef D3D11MODEL_HPP
#define D3D11MODEL_HPP

#include <d3d11.h>
#include "vertex.hpp"

namespace aes {

	class D3D11Model
	{
	public:
		void init(ID3D11Device* device);
		void destroy();

		void render(ID3D11DeviceContext*);

	private:
		ID3D11Buffer* vertexBuffer = nullptr, *indexBuffer = nullptr;
		int vertexCount, indexCount;
	};

}

#endif