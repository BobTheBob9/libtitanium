#pragma once

#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/template_constraints.hpp>

#include <math.h>
#include <type_traits>

namespace util::maths
{
    template <std::floating_point T> constexpr T PI = 3.14159265358979323846;
    template <std::floating_point T> constexpr T DEG_TO_RAD = PI<T> / T( 180 );
    template <std::floating_point T> constexpr T RAD_TO_DEG = T( 180 ) / PI<T>;

    // these functions are used to make it easier to change which angular representation we use internally
    // if everyone uses these for their angles, then it's easier to just change what we use here
    template <std::floating_point T> constexpr T Radians( T flAngle ) { return flAngle; }
    template <std::floating_point T> constexpr T Degrees( T flAngle ) { return flAngle * DEG_TO_RAD<T>; }

    // TODO: would be good if we had a (compile time!!!) way to take a variable number of args to these
    // TODO: is this even faster branchless? need a way to check assembly of func
    template<util::templateconstraints::AnyNumeric T>
    constexpr T Min( const T nFirst, const T nSecond )
    {
        return ( nFirst * ( nFirst < nSecond ) ) + ( nSecond * ( nSecond <= nFirst ) ); 
    }

    template<util::templateconstraints::AnyNumeric T>
    constexpr T Max( const T nFirst, const T nSecond )
    {
        return ( nFirst * ( nFirst > nSecond ) ) + ( nSecond * ( nSecond >= nFirst ) ); 
    }

    template<util::templateconstraints::AnyNumeric T>
    constexpr bool WithinRange( const T tValue, const T tBegin, const T tEnd )
    {
        return tValue >= tBegin && tValue < tEnd;
    }



    template<util::templateconstraints::Arithmetic T>
    struct Vec2
    {
        T x, y;
    };

    static_assert( sizeof( Vec2<i32> ) == sizeof( i32 ) * 2 );
    static_assert( sizeof( Vec2<f32> ) == sizeof( f32 ) * 2 );

    // TODO: Vec3 and Vec3Angle should definitely be seperate types, as position => angle shouldn't be an implicit conversion
    // but it sucks that we have to implement all the vec3 methods twice!!!
    template<util::templateconstraints::Arithmetic T>
    struct Vec3
    {
        // generally, we define z as up, x as right, and y as forward from a view angle of 0, 0, 0
        T x, y, z;

        static Vec3<T> Add( const Vec3<T> vFirst, const Vec3<T> vSecond );
        static Vec3<T> MultiplyScalar( const Vec3<T> vFirst, const T tMultiplyWith );
        static void AddTo( Vec3<T> *const pvPtr, const Vec3<T> vOther );
    };

    static_assert( sizeof( Vec3<i32> ) == sizeof( i32 ) * 3 );
    static_assert( sizeof( Vec3<f32> ) == sizeof( f32 ) * 3 );

    template<util::templateconstraints::Arithmetic T>
    Vec3<T> Vec3<T>::Add( const Vec3<T> vFirst, const Vec3<T> vSecond )
    {
        return { .x = vFirst.x + vSecond.x, .y = vFirst.y + vSecond.y, .z = vFirst.z + vSecond.z };
    }

    template<util::templateconstraints::Arithmetic T>
    Vec3<T> Vec3<T>::MultiplyScalar( const Vec3<T> vFirst, const T tMultiplyWith )
    {
        return { .x = vFirst.x * tMultiplyWith, .y = vFirst.y * tMultiplyWith, .z = vFirst.z * tMultiplyWith };
    }

    template<util::templateconstraints::Arithmetic T>
    void Vec3<T>::AddTo( Vec3<T> *const pvPtr, const Vec3<T> vOther )
    {
        pvPtr->x += vOther.x;
        pvPtr->y += vOther.y;
        pvPtr->z += vOther.z;
    }


    template<util::templateconstraints::Arithmetic T>
    struct Vec3Angle
    {
        // generally, we define yaw as rotation around z (left/right), pitch as rotation around x (up/down) and roll as rotation around y
        // (x, y, z are defined above, in Vec3)
        T yaw, pitch, roll;

        static Vec3Angle<T> Add( const Vec3Angle<T> vFirst, const Vec3Angle<T> vSecond );
        static Vec3Angle<T> MultiplyScalar( const Vec3Angle<T> vFirst, const T tMultiplyWith );
        static void AddTo( Vec3Angle<T> *const pvPtr, const Vec3Angle<T> vOther );
    };

    template<util::templateconstraints::Arithmetic T>
    Vec3Angle<T> Vec3Angle<T>::Add( const Vec3Angle<T> vFirst, const Vec3Angle<T> vSecond )
    {
        return { .yaw = vFirst.yaw + vSecond.yaw, .pitch = vFirst.pitch + vSecond.pitch, .roll = vFirst.roll + vSecond.roll };
    }

    template<util::templateconstraints::Arithmetic T>
    Vec3Angle<T> Vec3Angle<T>::MultiplyScalar( const Vec3Angle<T> vFirst, const T tMultiplyWith )
    {
        return { .yaw = vFirst.yaw * tMultiplyWith, .pitch = vFirst.pitch * tMultiplyWith, .roll = vFirst.roll * tMultiplyWith };
    }

    template<util::templateconstraints::Arithmetic T>
    void Vec3Angle<T>::AddTo( Vec3Angle<T> *const pvPtr, const Vec3Angle<T> vOther )
    {
        pvPtr->yaw += vOther.yaw;
        pvPtr->pitch += vOther.pitch;
        pvPtr->roll += vOther.roll;
    }




    // TEMP: this needs to be float-only for now, i think
    template <std::floating_point T>
    Vec3<T> AnglesToForward( const Vec3Angle<T> vAngles )
    {
        // this is fucked. i dont get it
        // fr having a breakdown over my inability to write a good AnglesToForward function right now. good lord
        // maybe i will do this tomorrow instead

        const T flYawRads = vAngles.yaw * DEG_TO_RAD<T>;
        const T flPitchRads = vAngles.pitch * DEG_TO_RAD<T>;

        return {
            .x = -sinf( flYawRads ) * sinf( flPitchRads ),
            .y = cosf( flYawRads ) * sinf( flPitchRads ),
            .z = sinf( flPitchRads )
        };

        /*const float flYawAngle = vAngles.y * ( M_PI * 2.f / 360.f );
        const float flYawSin = sinf( flYawAngle );
        const float flYawCos = cosf( flYawAngle );

        const float flPitchAngle = vAngles.x * ( M_PI * 2.f / 360.f );
        const float flPitchSin = sinf( flPitchAngle );
        const float flPitchCos = cosf( flPitchAngle );

        return {
            .x = flPitchCos * flYawSin,
            .y = -flPitchSin,
            .z = flPitchCos * flYawCos
        };*/
    }

    // TODO: should this support multiple types?
    // TODO: could simd this! probably! (lazy, so probably not)
    struct Matrix4x4
    {
        f32 flMatrices[ 4 ][ 4 ];

        static Matrix4x4 FromPosition( const Vec3<f32> fvPosition );
        static Matrix4x4 FromAngles( const Vec3Angle<f32> vfAngles );

        static Matrix4x4 FromPositionAngles( const Vec3<f32> fvPosition, const Vec3Angle<f32> fvAngles );
        static Matrix4x4 FromPositionScaleAngles();

        static Matrix4x4 FromViewPositionAngles( const Vec3<f32> fvPosition, const Vec3Angle<f32> fvAngles );

        static Matrix4x4 FromProjectionPerspective( const Vec2<uint> vnRenderSize, const float fldegFov, const float flNearDist, const float flFarDist );
        static Matrix4x4 FromProjectionOrthographic( const Vec2<uint> vnRenderSize, const uint nRenderHeight, const float fldegFov, const float flNearDist, const float flFarDist );

        static Matrix4x4 MultiplyMatrix( const Matrix4x4 mat4First, const Matrix4x4 mat4Second );
    };

    static_assert( sizeof( Matrix4x4 ) == sizeof( f32 ) * 4 * 4 );
}
