#include "ObjectMovementComponent.h"
#include "SceneComponent.h"
#include "../World/World.h"
#include "../World/WorldNavigation.h"

CObjectMovementComponent::CObjectMovementComponent()
{
	SetClassType<CObjectMovementComponent>();
}

CObjectMovementComponent::CObjectMovementComponent(
	const CObjectMovementComponent& ref) :
	CMovementComponent(ref)
{
}

CObjectMovementComponent::CObjectMovementComponent(
	CObjectMovementComponent&& ref) noexcept :
	CMovementComponent(std::move(ref))
{
}

CObjectMovementComponent::~CObjectMovementComponent()
{
}

void CObjectMovementComponent::MoveStraight(const FVector3& Pos)
{
	if (mApplyMove == false)
	{
		return;
	}

	mPathList.clear();
	mPathList.emplace_back(Pos);

	auto UpdateComponent = mUpdateComponent.lock();
	mMoveDist = Pos.Distance(UpdateComponent->GetWorldPos());
	mPathIndex = 0;
}

void CObjectMovementComponent::Move(const FVector3& Pos)
{
	if (mApplyMove == false)
	{
		return;
	}

	mPathList.clear();

	mPathList.emplace_back(Pos);
}

void CObjectMovementComponent::Move(const FVector2& Pos)
{
	Move(FVector3(Pos.x, Pos.y, 0.f));
}

void CObjectMovementComponent::MovePath(const FVector3& Pos)
{
	if (mApplyMove == false)
	{
		return;
	}

	auto	World = mWorld.lock();

	auto	Nav = World->GetNavigation().lock();

	auto	UpdateComponent = mUpdateComponent.lock();

	Nav->FindPath(UpdateComponent->GetWorldPos(), Pos, &mSelf);
}

void CObjectMovementComponent::MovePath(const FVector2& Pos)
{
	MovePath(FVector3(Pos.x, Pos.y, 0.f));
}

void CObjectMovementComponent::StartPath()
{
	auto	UpdateComponent = mUpdateComponent.lock();

	FVector3	Target = mPathList[mPathIndex];

	mMoveDist = Target.Distance(UpdateComponent->GetWorldPos());
}

bool CObjectMovementComponent::Init()
{
	return true;
}

void CObjectMovementComponent::Update(float DeltaTime)
{
}

void CObjectMovementComponent::PostUpdate(float DeltaTime)
{
	if (mUpdateComponent.expired())
	{
		mVelocity = FVector3::Zero;
		mCurSpeed = 0.f;
		return;
	}

	if (mPathList.empty())
	{
		if (mMoveDir.IsZero())
		{
			mVelocity = FVector3::Zero;
			mCurSpeed = 0.f;
			return;
		}

		mMoveDir.Normalize();

		mCurSpeed = std::min<float>(mCurSpeed + mAccel * DeltaTime, mSpeed);
		mVelocity = mMoveDir * mCurSpeed * DeltaTime;

		auto	UpdateComponent = mUpdateComponent.lock();

		UpdateComponent->AddWorldPos(mVelocity);
	}

	// 길찾기
	else
	{
		auto	UpdateComponent = mUpdateComponent.lock();

		FVector3 Target;
		Target = mPathList[mPathIndex];

		// 이동 방향을 구한다.
		FVector3	Dir = Target - UpdateComponent->GetWorldPos();
		Dir.z = 0.f;
		Dir.Normalize();

		mCurSpeed = std::min<float>(mCurSpeed + mAccel * DeltaTime, mSpeed);
		mVelocity = Dir * mCurSpeed * DeltaTime;

		UpdateComponent->AddWorldPos(mVelocity);

		mMoveDist -= (mCurSpeed * DeltaTime);

		// 이동된 위치와 타겟까지의 거리를 구한다.
		float	Dist = Target.Distance(UpdateComponent->GetWorldPos());

		// 도착점에 도착했을 경우
		if (Dist <= mAcceptDistance || mMoveDist < 0.f)
		{
			++mPathIndex;

			UpdateComponent->SetWorldPos(Target);

			// 이동을 종료시킨다.
			if (mPathIndex >= mPathList.size())
			{
				mPathList.clear();
				mPathIndex = 0;
				mMoveDist = 0.f;
			}

			// 다음 포인트로 이동해야 할 경우
			else
			{
				FVector3	Target = mPathList[mPathIndex];

				mMoveDist = Target.Distance(
					UpdateComponent->GetWorldPos());
			}
		}
	}
}

void CObjectMovementComponent::PostRender()
{
	CMovementComponent::PostRender();

	mLastMoveDir = mMoveDir;
	mMoveDir = FVector3::Zero;
}

void CObjectMovementComponent::Destroy()
{
	CMovementComponent::Destroy();
}

CObjectMovementComponent* CObjectMovementComponent::Clone()	const
{
	return new CObjectMovementComponent(*this);
}
