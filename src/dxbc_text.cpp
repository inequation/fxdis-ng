#include "dxbc.h"

const char* dxbc_shader_type_names[] = {
	"void",
	"bool",
	"int",
	"float",
	"string",
	"texture",
	"Texture1D",
	"Texture2D",
	"Texture3D",
	"TextureCube",
	"Sampler",
	"Sampler1D",
	"Sampler2D",
	"Sampler3D",
	"SamplerCube",
	"PixelShader",
	"VertexShader",
	"PixelFragment",
	"VertexFragment",
	"uint",
	"uint8",
	"GeometryShader",
	"Rasterizer",
	"DepthStencil",
	"Blend",
	"Buffer",
	"cbuffer",
	"tbuffer",
	"Texture1DArray",
	"Texture2DArray",
	"RenderTargetView",
	"DepthStencilView",
	"Texture2DMS",
	"Texture2DMSArray",
	"TextureCubeArray",
	"HullShader",
	"DomainShader",
	"Interface_Pointer",
	"ComputeShader",
	"double",
	"RWTexture1D",
	"RWTexture1DArray",
	"RWTexture2D",
	"RWTexture2DArray",
	"RWTexture3D",
	"RWBuffer",
	"ByteAddressBuffer",
	"RWByteaddressBuffer",
	"StructuredBuffer",
	"RWStructuredBuffer",
	"AppendStructuredBuffer",
	"ConsumeStructuredBuffer",
	"Min8Float",
	"Min10Float",
	"Min16Float",
	"Min12Int",
	"Min16Int",
	"Min16Uint"
};

const char* dxbc_shader_input_type_names[] = {
	"cbuffer",
	"tbuffer",
	"texture",
	"sampler",
	"uav",
	"sbuffer",
	"uav_structured",
	"byteaddress",
	"uav_byteaddress",
	"append_structured",
	"consume_structured",
	"uav_structured_with_counter"
};

const char* dxbc_shader_input_type_file_short_names[] = {
	"cb",
	"t",
	"t",
	"s",
	"u",
	"t",
	"u",
	"t",
	"u",
	"u",
	"u",
	"u"
};

const char* dxbc_shader_return_type_names[] = {
	"NA",
	"unorm",
	"snorm",
	"sint",
	"uint",
	"float",
	"mixed",
	"double",
	"continued"
};

const char* dxbc_shader_dimension_names[] = {
	"unknown",
	"buffer",
	"1d",
	"1darray",
	"2d",
	"2darray",
	"2dMS",
	"2dMSarray",
	"3d",
	"cube",
	"cubearray",
	"bufferex",
};

const char* dxbc_register_component_type_names[] = {
	"unknown",
	"uint",
	"int",
	"float"
};

const char* dxbc_names[] = {
	"UNDEFINED",
	"POSITION",
	"CLIP_DISTANCE",
	"CULL_DISTANCE",
	"RENDER_TARGET_ARRAY_INDEX",
	"VIEWPORT_ARRAY_INDEX",
	"VERTEX_ID",
	"PRIMITIVE_ID",
	"INSTANCE_ID",
	"IS_FRONT_FACE",
	"SAMPLE_INDEX",
	"FINAL_QUAD_EDGE_TESSFACTOR",
	"FINAL_QUAD_INSIDE_TESSFACTOR",
	"FINAL_TRI_EDGE_TESSFACTOR",
	"FINAL_TRI_INSIDE_TESSFACTOR",
	"FINAL_LINE_DETAIL_TESSFACTOR",
	"FINAL_LINE_DENSITY_TESSFACTOR",
	// Skip 47 indices all the way to index 64.
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr,
	"TARGET",
	"DEPTH",
	"COVERAGE",
	"DEPTH_GREATER_EQUAL",
	"DEPTH_LESS_EQUAL"
};
