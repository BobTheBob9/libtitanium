#include "maths.hpp"
#include <libtitanium/dev/tests.hpp>
#include <string.h>

namespace util::maths
{
    Matrix4x4 Matrix4x4::FromPosition( const Vec3<f32> fvPosition )
    {
        return { .flMatrices = {
            { 1.f,          0.f,          0.f,          1.f },
            { 0.f,          1.f,          0.f,          1.f },
            { 0.f,          0.f,          1.f,          1.f },
            { fvPosition.x, fvPosition.y, fvPosition.z, 1.f }
        } };
    }

    Matrix4x4 Matrix4x4::FromAngles( const Vec3Angle<f32> fvAngles )
    {
        const f32 cosYaw = cosf( fvAngles.yaw );
        const f32 sinYaw = sinf( fvAngles.yaw );
        const f32 cosRoll = cosf( fvAngles.roll );
        const f32 sinRoll = sinf( fvAngles.roll );
        const f32 cosPitch = cosf( fvAngles.pitch );
        const f32 sinPitch = sinf( fvAngles.pitch );

        return { .flMatrices = {
            { cosRoll * cosYaw,  ( sinPitch * sinRoll * cosYaw ) - ( cosPitch * sinYaw ),  ( sinPitch * sinYaw ) + ( cosPitch * sinRoll * cosYaw ),  0.f },
            { cosRoll * sinYaw,  ( cosPitch * cosYaw ) + ( sinPitch * sinRoll * sinYaw ),  ( cosPitch * sinRoll * sinYaw ) - ( sinPitch * cosYaw ),  0.f },
            { -sinRoll,          sinPitch * cosRoll,                                       cosPitch * cosRoll,                                       0.f },
            { 0.f,               0.f,                                                      0.f,                                                      1.f }
        } };
    }

    Matrix4x4 Matrix4x4::FromPositionAngles( const Vec3<f32> fvPosition, const Vec3Angle<f32> fvAngles )
    {
        Matrix4x4 r_matrix = FromAngles( fvAngles );

        r_matrix.flMatrices[ 3 ][ 0 ] = fvPosition.x;
        r_matrix.flMatrices[ 3 ][ 1 ] = fvPosition.y;
        r_matrix.flMatrices[ 3 ][ 2 ] = fvPosition.z;
        r_matrix.flMatrices[ 3 ][ 3 ] = 1.f;

        return r_matrix;
    }

    Matrix4x4 Matrix4x4::FromViewPositionAngles( const Vec3<f32> fvPosition, const Vec3Angle<f32> fvAngles )
    {
        return Matrix4x4::FromPositionAngles( Vec3<f32>::MultiplyScalar( fvPosition, -1 ), Vec3Angle<f32>::MultiplyScalar( fvAngles, -1 ) );
    }

    Matrix4x4 Matrix4x4::FromProjectionPerspective( const Vec2<uint> vnRenderSize, const float fldegFov, const float flNearDist, const float flFarDist )
    {
        const f32 flAspectRatio = f32( vnRenderSize.x ) / f32( vnRenderSize.y );
        const f32 flTanHalfFov = tanf( ( fldegFov * DEG_TO_RAD<f32> ) / 2.f );

        return { .flMatrices = {
            { 1.f / ( flAspectRatio * flTanHalfFov ), 0.f,                0.f,                                                      0.f  },
            { 0.f,                                    1.f / flTanHalfFov, 0.f,                                                      0.f  },
            { 0.f,                                    0.f,                flFarDist / ( flNearDist - flFarDist ),                   -1.f },
            { 0.f,                                    0.f,                -( flFarDist * flNearDist ) / ( flFarDist - flNearDist ), 0.f  }
        } };
    }

    Matrix4x4 Matrix4x4::MultiplyMatrix( const Matrix4x4 mat4First, const Matrix4x4 mat4Second )
    {
        // god this is ugly, lol
        // i would really like to just do this as a
        Matrix4x4 r_matrix {};

        for ( uint i = 0; i < 4; i++ )
        {
            for ( uint j = 0; j < 4; j++ )
            {
                for ( uint k = 0; k < 4; k++ )
                {
                    r_matrix.flMatrices[ i ][ j ] += mat4First.flMatrices[ i ][ k ] * mat4Second.flMatrices[ k ][ j ];
                }
            }
        }

        return r_matrix;
    }
}
