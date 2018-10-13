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
#include <d3d11shader.h>
#include <d3dcommon.h>
#include <memory>

dxbc_container* dxbc_parse(const void* data, int size)
{
	(void)size;
	std::auto_ptr<dxbc_container> container(new dxbc_container());
	container->data = data;
	dxbc_container_header* header = (dxbc_container_header*)data;
	uint32_t* chunk_offsets = (uint32_t*)(header + 1);
	if (bswap_le32(header->fourcc) != FOURCC_DXBC)
		return 0;
	unsigned num_chunks = bswap_le32(header->chunk_count);
	for (unsigned i = 0; i < num_chunks; ++i)
	{
		unsigned offset = bswap_le32(chunk_offsets[i]);
		dxbc_chunk_header* chunk = (dxbc_chunk_header*)((char*)data + offset);
		unsigned fourcc = bswap_le32(chunk->fourcc);
		container->chunk_map[fourcc] = i;
		container->chunks.push_back(chunk);
	}
	return container.release();
}

dxbc_chunk_header* dxbc_find_chunk(const void* data, int size, unsigned fourcc)
{
	(void)size;
	dxbc_container_header* header = (dxbc_container_header*)data;
	uint32_t* chunk_offsets = (uint32_t*)(header + 1);
	if (bswap_le32(header->fourcc) != FOURCC_DXBC)
		return 0;
	unsigned num_chunks = bswap_le32(header->chunk_count);
	for (unsigned i = 0; i < num_chunks; ++i)
	{
		unsigned offset = bswap_le32(chunk_offsets[i]);
		dxbc_chunk_header* chunk = (dxbc_chunk_header*)((char*)data + offset);
		if (bswap_le32(chunk->fourcc) == fourcc)
			return chunk;
	}
	return 0;
}

int dxbc_parse_signature(dxbc_chunk_signature* sig,
						 D3D11_SIGNATURE_PARAMETER_DESC** params)
{
	unsigned count = bswap_le32(sig->count);
	*params = (D3D11_SIGNATURE_PARAMETER_DESC*)malloc(
		sizeof(D3D11_SIGNATURE_PARAMETER_DESC) * count);

	for (unsigned i = 0; i < count; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC& param = (*params)[i];
		param.SemanticName =
			(char*)&sig->count + bswap_le32(sig->elements[i].name_offset);
		param.SemanticIndex = bswap_le32(sig->elements[i].semantic_index);
		param.SystemValueType =
			(D3D_NAME)bswap_le32(sig->elements[i].system_value_type);
		param.ComponentType = (D3D_REGISTER_COMPONENT_TYPE)bswap_le32(
			sig->elements[i].component_type);
		param.Register = bswap_le32(sig->elements[i].register_num);
		param.Mask = sig->elements[i].mask;
		param.ReadWriteMask = sig->elements[i].read_write_mask;
		param.Stream = sig->elements[i].stream;
	}
	return count;
}

void dxbc_parse_resource_definition(dxbc_chunk_resource_definition* rdef,
						 int& buffer_count,
						 D3D11_SHADER_BUFFER_DESC** buffers,
						 int& binding_count,
						 D3D11_SHADER_INPUT_BIND_DESC** bindings,
						 char** creator)
{
	unsigned count = bswap_le32(rdef->constant_buffer_count);
	*buffers = (D3D11_SHADER_BUFFER_DESC*)malloc(
		sizeof(D3D11_SHADER_BUFFER_DESC) * count);
	const auto* cb = (dxbc_rdef_constant_buffer*)((char*)&rdef->constant_buffer_count + bswap_le32(rdef->constant_buffer_offset));

	for (unsigned i = 0; i < count; ++i)
	{
		D3D11_SHADER_BUFFER_DESC& buffer = (*buffers)[i];
		buffer.Name = (char*)&rdef->constant_buffer_count + bswap_le32(cb[i].name_offset);
		buffer.Type = (D3D_CBUFFER_TYPE)bswap_le32(cb[i].type);
		buffer.Variables = bswap_le32(cb[i].variable_count);
		buffer.Size = bswap_le32(cb[i].size);
		buffer.uFlags = bswap_le32(cb[i].flags);
	}

	buffer_count = count;

	count = bswap_le32(rdef->resource_binding_count);
	*bindings = (D3D11_SHADER_INPUT_BIND_DESC*)malloc(
		sizeof(D3D11_SHADER_INPUT_BIND_DESC) * count);
	auto* rb = (dxbc_rdef_binding*)((char*)&rdef->constant_buffer_count + bswap_le32(rdef->resource_binding_offset));
	
	for (unsigned i = 0; i < count; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC& bind = (*bindings)[i];
		bind.Name = (char*)&rdef->constant_buffer_count + bswap_le32(rb[i].name_offset);
		bind.Type = (D3D_SHADER_INPUT_TYPE)bswap_le32(rb[i].input_type);
		bind.BindPoint = bswap_le32(rb[i].bind_point);
		bind.BindCount = bswap_le32(rb[i].bind_count);
		bind.uFlags = bswap_le32(rb[i].flags);
		bind.ReturnType = (D3D_RESOURCE_RETURN_TYPE)bswap_le32(rb[i].return_type);
		bind.Dimension = (D3D_SRV_DIMENSION)bswap_le32(rb[i].dimension);
		bind.NumSamples = bswap_le32(rb[i].sample_count);
	}

	binding_count = count;

	if (creator)
	{
		*creator = (char*)&rdef->constant_buffer_count + bswap_le32(rdef->creator_offset);
	}
}

int dxbc_parse_shader_variables(dxbc_chunk_resource_definition* rdef,
						 unsigned buffer_index,
						 D3D11_SHADER_TYPE_DESC** types,
						 D3D11_SHADER_VARIABLE_DESC** variables)
{
	const auto& cb = ((dxbc_rdef_constant_buffer*)
		((char*)&rdef->constant_buffer_count
			+ bswap_le32(rdef->constant_buffer_offset)))
		[buffer_index];
	unsigned count = bswap_le32(cb.variable_count);
	*variables = (D3D11_SHADER_VARIABLE_DESC*)malloc(
		sizeof(D3D11_SHADER_VARIABLE_DESC) * count);
	*types = (D3D11_SHADER_TYPE_DESC*)malloc(
		sizeof(D3D11_SHADER_TYPE_DESC) * count);
	const auto* v = (dxbc_rdef_variable*)((char*)&rdef->constant_buffer_count + bswap_le32(cb.variable_offset));

	const bool is_rd1_1 = rdef->optional[0].fourcc == FOURCC('R', 'D', '1', '1');
	
	for (unsigned i = 0; i < count; ++i)
	{
		D3D11_SHADER_VARIABLE_DESC& var = (*variables)[i];
		var.Name = (char*)&rdef->constant_buffer_count + bswap_le32(v[i].name_offset);
		var.StartOffset = bswap_le32(v[i].start_offset);
		var.Size = bswap_le32(v[i].size);
		var.uFlags = bswap_le32(v[i].flags);
		var.DefaultValue = (char*)&rdef->constant_buffer_count + bswap_le32(v[i].default_value_offset);
		if (is_rd1_1)
		{
			var.StartTexture = bswap_le32(v[i].optional->start_texture);
			var.TextureSize = bswap_le32(v[i].optional->texture_size);
			var.StartSampler = bswap_le32(v[i].optional->start_sampler);
			var.SamplerSize = bswap_le32(v[i].optional->sampler_size);
		}
		else
		{
			var.StartTexture = ~0u;
			var.TextureSize = 0;
			var.StartSampler = ~0u;
			var.SamplerSize = 0;
		}

		const auto& t = *(dxbc_rdef_type*)((char*)&rdef->constant_buffer_count + bswap_le32(v[i].type_offset));
		D3D11_SHADER_TYPE_DESC& type = (*types)[i];
		// FIXME: byte swapping on the 6 16-bit values below?
		type.Class = (D3D_SHADER_VARIABLE_CLASS)t.type_class;
		type.Type = (D3D_SHADER_VARIABLE_TYPE)t.type_type;
		type.Rows = t.rows;
		type.Columns = t.columns;
		type.Elements = t.element_count;
		type.Members = t.member_count;
		type.Offset = bswap_le32(t.member_offset);
		type.Name = nullptr;	// FIXME
	}

	return count;
}
