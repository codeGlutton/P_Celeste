#pragma once

#include "ColliderBox2D.h"
#include "ColliderSphere2D.h"
#include "ColliderLine2D.h"

namespace ECCWResult
{
	enum Type
	{
		CW = -1,
		Line,
		CCW
	};
};

class CCollision
{
private:
	// 0 ~ 1 클램프
	static float Clamp(float t);
	// 2D 수직선 구하기
	static FVector3 Perp2D(const FVector3& v);
	// 선분에서 가장 근접한 정점 찾기
	static FVector3 ClosestPointOnSegment2D(const FVector3& v1,
		const FVector3& v2, const FVector3& p, float* OutT = nullptr);
	// 일정 축에 박스 투영 시, 반지름 길이 결과 얻기
	static float ProjectBoxRadiusOnAxis(const FBox2DInfo& Box,
		const FVector3& AxisN);
	// 일정 방향으로 박스의 최근접점 구하기
	static FVector3 SupportPointBox2D(const FBox2DInfo& Box,
		const FVector3& Dir);
	static bool Manifold_BoxPoint2D(FCollisionManifold& Out,
		const FBox2DInfo& Box, const FVector3& Point);
	// 박스 일정 축 오버랩 검사
	static bool ManifoldBox2DAxisOverlap(float& MinOverlap, 
		FVector3& MinAxis, FVector3 Axis,
		const FBox2DInfo& Src, const FBox2DInfo& Dest);

	/* 충돌 결과 구하기 */
public:
	static bool ManifoldBox2DToBox2D(FCollisionManifold& Out,
		const FBox2DInfo& Src, const FBox2DInfo& Dest);
	static bool ManifoldBox2DToBox2D(FCollisionManifold& Out,
		CColliderBox2D* Src, CColliderBox2D* Dest);
	static bool ManifoldSphere2DToSphere2D(FCollisionManifold& Out,
		const FSphere2DInfo& Src, const FSphere2DInfo& Dest);
	static bool ManifoldSphere2DToSphere2D(FCollisionManifold& Out,
		CColliderSphere2D* Src, CColliderSphere2D* Dest);
	static bool ManifoldBox2DToSphere2D(FCollisionManifold& Out,
		const FBox2DInfo& Box, const FSphere2DInfo& Sphere);
	static bool ManifoldBox2DToSphere2D(FCollisionManifold& Out,
		CColliderBox2D* Box, CColliderSphere2D* Sphere);
	static bool ManifoldSphere2DToBox2D(FCollisionManifold& Out,
		const FSphere2DInfo& Sphere, const FBox2DInfo& Box);
	static bool ManifoldSphere2DToBox2D(FCollisionManifold& Out,
		CColliderSphere2D* Sphere, CColliderBox2D* Box);

	static bool ManifoldSphere2DToLine2D(FCollisionManifold& Out,
		const FSphere2DInfo& Sphere, const FLine2DInfo& Line);
	static bool ManifoldSphere2DToLine2D(FCollisionManifold& Out,
		CColliderSphere2D* Sphere, CColliderLine2D* Line);
	static bool ManifoldLine2DToSphere2D(FCollisionManifold& Out,
		const FLine2DInfo& Line, const FSphere2DInfo& Sphere);
	static bool ManifoldLine2DToSphere2D(FCollisionManifold& Out,
		CColliderLine2D* Line, CColliderSphere2D* Sphere);

	static bool ManifoldBox2DToLine2D(FCollisionManifold& Out,
		const FBox2DInfo& Box, const FLine2DInfo& Line);
	static bool ManifoldBox2DToLine2D(FCollisionManifold& Out,
		CColliderBox2D* Box, CColliderLine2D* Line);
	static bool ManifoldLine2DToBox2D(FCollisionManifold& Out,
		const FLine2DInfo& Line, const FBox2DInfo& Box);
	static bool ManifoldLine2DToBox2D(FCollisionManifold& Out,
		CColliderLine2D* Line, CColliderBox2D* Box);

	static bool ManifoldLine2DToLine2D(FCollisionManifold& Out,
		const FLine2DInfo& Src, const FLine2DInfo& Dest);
	static bool ManifoldLine2DToLine2D(FCollisionManifold& Out,
		CColliderLine2D* Src, CColliderLine2D* Dest);

private:
	// 땅 충돌에 대해서 속도 클램핑 분산
	static bool GroundClamp(FVector3& Velocity, float InvMass,
		const FVector3& PushDir, const FVector3& Up);

public:
	static bool ResolveSlideStop2D(FVector3& SrcPos,
		FVector3& SrcVelocity, float SrcInvMass,
		FVector3& DestPos,
		FVector3& DestVelocity, float DestInvMass,
		const FCollisionManifold& Manifold);

	/* 충돌 여부 검사 */
public:
	static bool CollisionBox2DToBox2D(FVector3& HitPoint,
		CColliderBox2D* Src, CColliderBox2D* Dest);
	static bool CollisionAABB2DToAABB2D(FVector3& HitPoint,
		const FBox2DInfo& Src, const FBox2DInfo& Dest);
	static bool CollisionOBB2DToOBB2D(FVector3& HitPoint,
		const FBox2DInfo& Src, const FBox2DInfo& Dest);

public:
	static bool CollisionSphere2DToSphere2D(FVector3& HitPoint,
		CColliderSphere2D* Src, CColliderSphere2D* Dest);
	static bool CollisionSphere2DToSphere2D(FVector3& HitPoint,
		const FSphere2DInfo& Src, const FSphere2DInfo& Dest);

public:
	static bool CollisionBox2DToSphere2D(FVector3& HitPoint,
		CColliderBox2D* Src, CColliderSphere2D* Dest);
	static bool CollisionBox2DToSphere2D(FVector3& HitPoint,
		const FBox2DInfo& Box, const FSphere2DInfo& Sphere);

public:
	static bool CollisionBox2DToLine2D(FVector3& HitPoint,
		CColliderBox2D* Src, CColliderLine2D* Dest);
	static bool CollisionBox2DToLine2D(FVector3& HitPoint,
		const FBox2DInfo& Box, const FLine2DInfo& Line);

	static bool CollisionSphere2DToLine2D(FVector3& HitPoint,
		CColliderSphere2D* Src, CColliderLine2D* Dest);
	static bool CollisionSphere2DToLine2D(FVector3& HitPoint,
		const FSphere2DInfo& Sphere, const FLine2DInfo& Line);

	static bool CollisionLine2DToLine2D(FVector3& HitPoint,
		CColliderLine2D* Src, CColliderLine2D* Dest);
	static bool CollisionLine2DToLine2D(FVector3& HitPoint,
		const FLine2DInfo& Src, const FLine2DInfo& Dest);


	static bool CollisionBox2DToPoint(FVector3& HitPoint,
		const FBox2DInfo& Box, const FVector3& Point);
	static bool CollisionSphere2DToPoint(FVector3& HitPoint,
		const FSphere2DInfo& Sphere, const FVector3& Point);
	static bool CollisionLine2DToPoint(FVector3& HitPoint,
		const FLine2DInfo& Line, const FVector3& Point);


private:
	static bool AxisProjection(const FVector3& CenterLine,
		const FVector3& Axis, float SrcHalfSize,
		const FVector3* DestAxis, const FVector2& DestHalfSize);

	static ECCWResult::Type CCW2D(const FVector3& p1,
		const FVector3& p2,
		const FVector3& p3);
	static bool PointOnLine2D(const FVector3& LineStart,
		const FVector3& LineEnd,
		const FVector3& Point);
};

