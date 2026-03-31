#pragma once

class CGameObject;
class CCollider;
class CInteractableObject;
class CCharacterMovement;
class CMovementEffector;

namespace ETouchedWall
{
	enum class Type : uint8
	{
		Up = 0,
		Down,
		Left,
		Right,
		Count
	};
	constexpr const uint8 TypeCount = static_cast<uint8>(Type::Count);

	enum Flag : uint8
	{
		Up = 1 << 0,
		Down = 1 << 1,
		Left = 1 << 2,
		Right = 1 << 3,

		None = 0,

		VerticalHit = Up | Down,
		HorizonHit = Left | Right,
	};
	GEN_ENUM_FLAG_OPERATORS(ETouchedWall::Flag)
}

enum class EDesiredInputRotation
{
	None,
	Up,
	Down,
	Left,
	Right,
};

/**
 * 게임 상태
 */
struct FSharedGameState
{
public:
	FSharedGameState(std::weak_ptr<CGameObject> OwnerObject) :
		mOwnerObject(OwnerObject)
	{
	}

	FSharedGameState(const FSharedGameState& ref) = default;
	FSharedGameState(FSharedGameState&& ref) noexcept = default;

public:
	virtual ~FSharedGameState() = default;

public:
	virtual void Update(float DeltaTime);

public:
	void AddTouchedWall(ETouchedWall::Type TouchType, const CCollider* Collider);
	void RemoveTouchedWall(ETouchedWall::Type TouchType, const CCollider* Collider);

public:
	std::shared_ptr<CMovementEffector> EffectTouchedWalls(ETouchedWall::Type TouchType, std::shared_ptr<CCharacterMovement> Movement);
	std::shared_ptr<CMovementEffector> EffectTouchedWalls(const FVector3& TouchDir, std::shared_ptr<CCharacterMovement> Movement);

public:
	bool IsSideOfWall() const
	{
		return mWallState & ETouchedWall::HorizonHit;
	}
	bool IsLandingOnWall() const
	{
		return mIsLand;
	}
	bool IsCrushed() const;
	ETouchedWall::Flag GetWallState() const
	{
		return mWallState;
	}

public:
	std::vector<std::function<void()>> mOnLand;
	std::vector<std::function<void()>> mOnAir;

protected:
	std::weak_ptr<CGameObject> mOwnerObject;

protected:
	ETouchedWall::Flag mWallState = ETouchedWall::None;
	std::array<int32, ETouchedWall::TypeCount> mTouchedWallCounts = {};
	std::array<std::unordered_map<const CCollider*, std::weak_ptr<CInteractableObject>>, ETouchedWall::TypeCount> mTouchedDynamicWalls = {};
	std::array<std::unordered_map<const CCharacterMovement*, std::weak_ptr<CCharacterMovement>>, ETouchedWall::TypeCount> mEffectedMovements = {};

protected:
	bool mIsLand = false;
	float mAccLandingTime = 0.f;
	const float mLandingThresholdTime = 0.02f;
};

