#pragma once

#include "Asset/Animation2D/AnimMetaData.h"

struct FHairFrameData
{
    // 뒷 머리 오프셋
    FVector2 mBackHairOffset = FVector2(0.f, 0.f);
    // 앞 머리 오프셋
    FVector2 mFrontHairOffset = FVector2(0.f, 0.f);
    // 자체 플립 여부
    bool mIsFlip = false;
};

class CHairMetaData : public CAnimMetaData
{
    friend class CHairEditorWidget;

public:
    using CAnimMetaData::GetFrameMetaData;
    using CAnimMetaData::Load;
    using CAnimMetaData::Save;

protected:
    virtual const void* GetFrameMetaData(int Index) const override
    {
        if (mFrameDatas.size() <= Index)
        {
            return nullptr;
        }
        return &mFrameDatas[Index];
    }
    virtual bool Load(FILE* File) override;
    virtual bool Save(FILE* File) const override;

#ifdef _DEBUG
protected:
    virtual void SetFrameMetaData(int Index, const void* FrameData) override
    {
        if (FrameData == nullptr)
        {
            return;
        }

        if (mFrameDatas.size() <= Index)
        {
            mFrameDatas.resize(Index + 1);
        }
        mFrameDatas[Index] = *static_cast<const FHairFrameData*>(FrameData);
    }
#endif // _DEBUG

private:
    std::vector<FHairFrameData> mFrameDatas;
};
