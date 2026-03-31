#pragma once

#include "SceneComponent.h"

class CCollisionQuadTreeNode;

class CCollider abstract :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CObject;

protected:
	CCollider();
	CCollider(const CCollider& ref);
	CCollider(CCollider&& ref)	noexcept;

public:
	virtual ~CCollider();

	void AddCollisionNode(CCollisionQuadTreeNode* Node)
	{
		mCurrentNodeList.push_back(Node);
	}

	void EraseCollisionNode(CCollisionQuadTreeNode* Node)
	{
		auto	iter = mCurrentNodeList.begin();
		auto	iterEnd = mCurrentNodeList.end();

		for (; iter != iterEnd; ++iter)
		{
			if (*iter == Node)
			{
				mCurrentNodeList.erase(iter);
				return;
			}
		}
	}

	void ClearCollisionNode()
	{
		mCurrentNodeList.clear();
	}

	bool CheckNode(
		const std::list<CCollisionQuadTreeNode*>& NodeList)
	{
		auto	iter = mCurrentNodeList.begin();
		auto	iterEnd = mCurrentNodeList.end();

		for (; iter != iterEnd; ++iter)
		{
			auto	iter1 = NodeList.begin();
			auto	iter1End = NodeList.end();

			for (; iter1 != iter1End; ++iter1)
			{
				if (*iter == *iter1)
					return true;
			}
		}

		return false;
	}

	void ComputePrevNode()
	{
		auto	iter = mCollisionObjectMap.begin();
		auto	iterEnd = mCollisionObjectMap.end();

		for (; iter != iterEnd;)
		{
			if (!iter->first->CheckNode(mCurrentNodeList))
			{
				CallCollisionEndNode(iter->first);

				iter->first->CallCollisionEndNode(this);

				iter->first->EraseCollisionObject(this);

				iter = mCollisionObjectMap.erase(iter);
				iterEnd = mCollisionObjectMap.end();
				continue;
			}

			++iter;
		}

		if (mCollisionObjectMap.empty())
			mCollision = false;
	}

public:
	const FVector3& GetMin()	const
	{
		return mMin;
	}

	const FVector3& GetMax()	const
	{
		return mMax;
	}

	EColliderType GetColliderType()	const
	{
		return mColliderType;
	}

	bool GetDebugDraw()	const
	{
		return mDebugDraw;
	}

	FCollisionProfile* GetCollisionProfile()	const
	{
		return mProfile;
	}

	bool GetStatic()	const
	{
		return mStatic;
	}

	void SetStatic(bool Static)
	{
		mStatic = Static;
	}

	float GetInvMass()	const
	{
		return mInvMass;
	}

	void SetInvMass(float InvMass)
	{
		mInvMass = InvMass;
	}

	bool HasCollisionObject() const
	{
		return mCollisionObjectMap.empty() == false;
	}
	bool CheckCollisionObject(CCollider* Collider);
	void EraseCollisionObject(CCollider* Collider);

public:
	virtual void SetDebugDraw(bool DebugDraw);
	void SetCollisionProfile(const std::string& Name);

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void Render();
	// 충돌 시 정보가 변경될 수도 있기 때문에 충돌 시 정보가 변경되면
	// 이 함수를 호출해서 충돌체의 정보도 변경시켜줄 수 있게 한다.
	virtual void UpdateInfo() = 0;

protected:
	virtual CCollider* Clone()	const = 0;

public:
	void CallCollisionBegin(const FVector3& HitPoint,
		const std::weak_ptr<CCollider>& Dest);
	void CallCollisionEnd(CCollider* Dest);
	void CallCollisionEndNode(CCollider* Dest);
	void CallCollisionHit(const FVector3& HitPoint,
		const std::weak_ptr<CCollider>& Dest);
	void CollisionHitEnd();
	void CallCollisionMouseBegin(const FVector3& MousePos);
	void CallCollisionMouseEnd(const FVector3& MousePos);

public:
	virtual bool PreCollision(FVector3& HitPoint,
		std::shared_ptr<CCollider> Dest);
	virtual bool Collision(FVector3& HitPoint,
		std::shared_ptr<CCollider> Dest) = 0;
	virtual bool CollisionManifold(FCollisionManifold& Result,
		std::shared_ptr<CCollider> Dest) = 0;
	virtual bool CollisionMouse(const FVector2& MousePos) = 0;

public:
	template <typename T>
	void SetCollisionBeginFunction(T* Obj,
		void(T::* Func)(const FVector3&, CCollider*))
	{
		mCollisionBeginFunc = std::bind(Func, Obj,
			std::placeholders::_1, std::placeholders::_2);
	}
	void SetCollisionBeginFunction(std::function<void(const FVector3&, CCollider*)> Lamda)
	{
		mCollisionBeginFunc = Lamda;
	}

	template <typename T>
	void SetCollisionEndFunction(T* Obj,
		void(T::* Func)(CCollider*))
	{
		mCollisionEndFunc = std::bind(Func, Obj,
			std::placeholders::_1);
	}
	void SetCollisionEndFunction(std::function<void(CCollider*)> Lamda)
	{
		mCollisionEndFunc = Lamda;
	}

	template <typename T>
	void SetCollisionHitFunction(T* Obj,
		void(T::* Func)(const FVector3&, CCollider*))
	{
		mCollisionHitFunc = std::bind(Func, Obj,
			std::placeholders::_1, std::placeholders::_2);
	}
	void SetCollisionHitFunction(std::function<void(const FVector3&, CCollider*)> Lamda)
	{
		mCollisionHitFunc = Lamda;
	}

	template <typename T>
	void SetCollisionMouseBeginFunction(T* Obj,
		void(T::* Func)(const FVector3&))
	{
		mCollisionMouseBeginFunc = std::bind(Func, Obj,
			std::placeholders::_1);
	}
	void SetCollisionMouseBeginFunction(std::function<void(const FVector3&)> Lamda)
	{
		mCollisionMouseBeginFunc = Lamda;
	}

	template <typename T>
	void SetCollisionMouseEndFunction(T* Obj,
		void(T::* Func)(const FVector3&))
	{
		mCollisionMouseEndFunc = std::bind(Func, Obj,
			std::placeholders::_1);
	}
	void SetCollisionMouseEndFunction(std::function<void(const FVector3&)> Lamda)
	{
		mCollisionMouseEndFunc = Lamda;
	}

#ifdef _DEBUG

public:
	static bool mAllowDrawDebug;

#endif // _DEBUG

protected:
	// 콜리전 타입
	EColliderType	mColliderType;
	FVector3		mMin;
	FVector3		mMax;
	FVector3		mRenderScale;
	bool			mDebugDraw = false;
	bool			mCollision = false;
	// 정적 객체
	bool			mStatic = false;
	float			mInvMass = 1.f;
	FCollisionProfile* mProfile = nullptr;

	// 이 충돌체와 부딪친 충돌체는 서로 떨어지기 전까지
	// 이 맵에 보관해둔다.
	std::unordered_map<CCollider*, std::weak_ptr<CCollider>>	mCollisionObjectMap;

	// 속해있는 충들 노드를 저장한다.
	std::list<class CCollisionQuadTreeNode*>	mCurrentNodeList;

	std::weak_ptr<class CShader>	mShader;
	std::weak_ptr<class CMesh>		mMesh;
	std::shared_ptr<class CCBufferTransform>	mTransformCBuffer;
	std::shared_ptr<class CCBufferCollider>	mColliderCBuffer;

	// Overlap일 경우 충돌 시작과 충돌 종료 시 호출될 함수
	std::function<void(const FVector3&, CCollider*)>	
		mCollisionBeginFunc;
	std::function<void(CCollider*)>	mCollisionEndFunc;

	// Block일 경우 충돌 시 호출될 함수.
	std::function<void(const FVector3&, CCollider*)>	mCollisionHitFunc;
	
	// 마우스와 충돌 시 호출될 함수
	std::function<void(const FVector3&)>	mCollisionMouseBeginFunc;
	std::function<void(const FVector3&)>	mCollisionMouseEndFunc;
};

