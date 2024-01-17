#pragma once

#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/numerics.hpp>

namespace renderer::shaders
{
    namespace EBindgroup
    {
        enum : int
        {
            RENDERVIEW,
            RENDEROBJECT
        };
    }

    // I'm not entirely happy with this code
    // Currently, we need to define all enums in both code and shader and keep both defs in sync
    // this kind of sucks! ideally, we just define them once and have it propogate to both
    // maybe some kind of macro or codegen could be used for this? unsure

    // also, can't tell if i hate how these shaders are structured or i just hate wgsl, lol

    /*
     *  Builtin renderer uniforms
     *  TODO: need a way to define uniforms from code also
     */
    #pragma pack( push, 16 )
    struct UShaderView
    {
        util::maths::Matrix4x4 mat4fViewTransform;
        util::maths::Matrix4x4 mat4fPerspective;
    };
    #pragma pack( pop )
    static_assert( sizeof( UShaderView ) % 16 == 0 );

    #pragma pack( push, 16 )
    struct UShaderObjectInstance
    {
        util::maths::Matrix4x4 mat4fBaseTransform;
    };
    #pragma pack( pop )
    static_assert( sizeof( UShaderObjectInstance ) % 16 == 0 );

    // this should probably be like #embed or something bleh

    constexpr char DEFAULT_DRAW_3D[] = R"(
struct UShaderView
{
    mat4fViewTransform : mat4x4<f32>,
    mat4fPerspective : mat4x4<f32>
};

struct UShaderObjectInstance
{
    mat4fBaseTransform : mat4x4<f32>
};

@group( 0 ) @binding( 0 ) var<uniform> u_view : UShaderView;
@group( 1 ) @binding( 0 ) var<uniform> u_object : UShaderObjectInstance;

struct I_VertexShader3D
{
    @location( 0 ) position : vec3<f32>,
    @location( 1 ) uv : vec2<f32>
};

struct I_FragmentShader3D
{
    @builtin( position ) position : vec4<f32>,
    @location( 0 ) uv : vec2<f32>
};

@vertex fn main_Vert( vertex : I_VertexShader3D ) -> I_FragmentShader3D
{
    var r_vertex : I_FragmentShader3D;

    r_vertex.position = u_view.mat4fPerspective * u_view.mat4fViewTransform * u_object.mat4fBaseTransform * vec4<f32>( vertex.position, 1.0 );
    r_vertex.uv = vertex.uv;

    return r_vertex;
}



@group( 1 ) @binding( 2 ) var t_sampler : sampler;
@group( 1 ) @binding( 1 ) var t_baseColour : texture_2d<f32>;

@fragment fn main_Frag( fragment : I_FragmentShader3D ) -> @location( 0 ) vec4<f32>
{
    return vec4<f32>( textureSample( t_baseColour, t_sampler, fragment.uv ).rgb, 1.0 );
}

)";

    constexpr char DEFAULT_DRAW_2D[] = R"(
struct UShaderObjectInstance2D
{
    vec2Position : vec2<f32>
};

@group( 0 ) @binding( 0 ) var<uniform> u_object2D : UShaderObjectInstance2D;

@vertex fn main_Vert( @location( 0 ) vertexPosition : vec3<f32> ) ->  @builtin( position ) : vec4<f32>
{
    return u_object2D.position + vertex.position;
}



@group( 1 ) @binding( 2 ) var t_sampler : sampler;
@group( 1 ) @binding( 1 ) var t_baseColour : texture_2d<f32>;

@fragment fn main_Frag( @builtin( position ) fragmentPosition : vec4<f32> ) -> @location( 0 ) vec4<f32>
{
    // TODO: probably still needs uvs, arhg
    return vec4<f32>( textureSample( t_baseColour, t_sampler, fragmentPosition.xy ).rgb, 1.0 );
}

)";

}
