/**************************************************************************
 *
 * Copyright 2010 Luca Barbieri
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "dxbc.h"
#include <iomanip>
#include <memory>
#include <string.h>
#include <d3d11shader.h>

// FIXME: move to sm4_text.cpp
static std::ostream& operator<<(std::ostream& out, const D3D11_SHADER_TYPE_DESC& type)
{
	switch (type.Type)
	{
	case D3D_SVT_VOID:
		out << "void";
		break;
	case D3D_SVT_BOOL:
		out << "bool";
		break;
	case D3D_SVT_INT:
		out << "int";
		break;
	case D3D_SVT_FLOAT:
		out << "float";
		break;
	case D3D_SVT_TEXTURE:
		out << "texture";
		break;
	case D3D_SVT_TEXTURE1D:
		out << "Texture1D";
		break;
	case D3D_SVT_TEXTURE2D:
		out << "Texture2D";
		break;
	case D3D_SVT_TEXTURE3D:
		out << "Texture3D";
		break;
	case D3D_SVT_TEXTURECUBE:
		out << "TextureCube";
		break;
	case D3D_SVT_UINT:
		out << "uint";
		break;
	case D3D_SVT_UINT8:
		out << "uint8";
		break;
	case D3D_SVT_BUFFER:
		out << "Buffer<";
		break;
	case D3D_SVT_TEXTURE1DARRAY:
		out << "Texture1DArray";
		break;
	case D3D_SVT_TEXTURE2DARRAY:
		out << "Texture2DArray";
		break;
	case D3D_SVT_TEXTURE2DMS:
		out << "Texture2DMS";
		break;
	case D3D_SVT_TEXTURE2DMSARRAY:
		out << "Texture2DMSArray";
		break;
	case D3D_SVT_DOUBLE:
		out << "double";
		break;
	case D3D_SVT_RWTEXTURE1D:
		out << "RWTexture1D";
		break;
	case D3D_SVT_RWTEXTURE1DARRAY:
		out << "RWTexture1DArray";
		break;
	case D3D_SVT_RWTEXTURE2D:
		out << "RWTexture2D";
		break;
	case D3D_SVT_RWTEXTURE2DARRAY:
		out << "RWTexture2DArray";
		break;
	case D3D_SVT_RWTEXTURE3D:
		out << "RWTexture3D";
		break;
	case D3D_SVT_RWBUFFER:
		out << "RWBuffer<";
		break;
	case D3D_SVT_BYTEADDRESS_BUFFER:
		out << "ByteAddressBuffer";
		break;
	case D3D_SVT_RWBYTEADDRESS_BUFFER:
		out << "RWByteAddressBuffer";
		break;
	case D3D_SVT_STRUCTURED_BUFFER:
		out << "StructuredBuffer<";
		break;
	case D3D_SVT_RWSTRUCTURED_BUFFER:
		out << "RWStructuredBuffer<";
		break;
	case D3D_SVT_APPEND_STRUCTURED_BUFFER:
		out << "AppendStructuredBuffer<";
		break;
	case D3D_SVT_CONSUME_STRUCTURED_BUFFER:
		out << "ConsumeStructuredBuffer<";
		break;
	default:
		assert(!"Unhandled shader variable type");
		break;
	}
	switch (type.Class)
	{
	case D3D_SVC_SCALAR:
		break;
	case D3D_SVC_VECTOR:
		out << type.Columns;
		break;
	case D3D_SVC_MATRIX_ROWS:
		out << type.Rows << "x" << type.Columns;
		break;
	case D3D_SVC_MATRIX_COLUMNS:
		out << type.Columns << "x" << type.Rows;
		break;
	default:
		assert(!"Unhandled shader variable class");
		break;
	}
	return out;
}

// FIXME: move to sm4_text.cpp
static std::ostream& operator<<(std::ostream& out, const D3D_SHADER_INPUT_TYPE& type)
{
	switch (type)
	{
	case D3D_SIT_CBUFFER:
		out << "cbuffer";
		break;
	case D3D_SIT_TBUFFER:
		out << "tbuffer";
		break;
	case D3D_SIT_TEXTURE:
		out << "texture";
		break;
	case D3D_SIT_SAMPLER:
		out << "sampler";
		break;
	case D3D_SIT_UAV_RWTYPED:
		out << "uav";
		break;
	case D3D_SIT_STRUCTURED:
		out << "sbuffer";
		break;
	case D3D_SIT_UAV_RWSTRUCTURED:
		out << "suav";
		break;
	case D3D_SIT_BYTEADDRESS:
		out << "bbuffer";
		break;
	case D3D_SIT_UAV_RWBYTEADDRESS:
		out << "buav";
		break;
	case D3D_SIT_UAV_APPEND_STRUCTURED:
		out << "asbuffer";
		break;
	case D3D_SIT_UAV_CONSUME_STRUCTURED:
		out << "csbuffer";
		break;
	case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
		out << "scuav";
		break;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const dxbc_container& container)
{
	for (unsigned i = 0; i < container.chunks.size(); ++i)
	{
		struct dxbc_chunk_header* chunk = container.chunks[i];
		char fourcc_str[5];
		memcpy(fourcc_str, &chunk->fourcc, 4);
		fourcc_str[4] = 0;
		out << "// DXBC chunk " << std::setw(2) << i << ": " << fourcc_str
			<< " offset " << ((char*)chunk - (char*)container.data) << " size "
			<< bswap_le32(chunk->size) << "\n";
		switch (chunk->fourcc)
		{
		case FOURCC_RDEF:
			auto* rdef = static_cast<dxbc_chunk_resource_definition*>(chunk);
			D3D11_SHADER_BUFFER_DESC* buffers = nullptr;
			D3D11_SHADER_TYPE_DESC* types = nullptr;
			D3D11_SHADER_VARIABLE_DESC* vars = nullptr;
			D3D11_SHADER_INPUT_BIND_DESC* bindings = nullptr;
			char* creator;
			int buffer_count, binding_count;
			dxbc_parse_resource_definition(rdef, buffer_count, &buffers, binding_count, &bindings, &creator);
			out << "// Generated by " << creator << "\n"
				"//\n";

			if (buffer_count > 0)
			{
				out << "// Buffer Definitions:\n"
					   "//\n";
				for (int j = 0; j < buffer_count; ++j)
				{
					out << "// cbuffer cbuf" << j << "\n"
						   "// {\n";
					int vcount = dxbc_parse_shader_variables(rdef, j, &types, &vars);
					for (int k = 0; k < vcount; ++k)
					{
						out << "//   " << types[k] << " " << vars[k].Name;
						if (types[k].Elements > 1)
						{
							out << "[" << types[k].Elements << "]";
						}
						out << ";\t"
							   "// Offset:\t" << vars[k].StartOffset << "\t"
							   "Size:\t" << vars[k].Size << "\t"
							   "Flags:\t" << std::hex << vars[k].uFlags << std::dec << "\n";
					}
					out << "// }\n";

					free(vars);
					vars = nullptr;
				}
			}
			out << "//\n";

			if (binding_count > 0)
			{
				out << "// Resource Bindings:\n"
					   "//\n"
					   "// Name\tType\tFormat\tDim\tHLSL Bind\tCount\tFlags\n"
					   "// -------------------------------------------------------\n";
				for (int j = 0; j < binding_count; ++j)
				{
					out << "// " << bindings[j].Name << "\t" <<
						   bindings[j].Type << "\t" <<
						   bindings[j].ReturnType << "\t" <<
						   bindings[j].Dimension << "\t" <<
						   bindings[j].BindPoint << "\t" <<
						   bindings[j].BindCount << "\t" <<
						   std::hex << bindings[j].uFlags << std::dec << "\n";
				}
				out << "//\n";
			}

			free(buffers);
			free(bindings);
			break;
		}
	}
	return out;
}
