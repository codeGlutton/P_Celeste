#pragma once

#include "../Asset.h"

class CAnimMetaData abstract : public CAsset
{
    friend class CAnimationManager;

protected:
    CAnimMetaData();

public:
    virtual ~CAnimMetaData() override = default;

public:
    bool Load(const TCHAR* FileName, const std::string& PathName);
    bool Load(const TCHAR* FullPath);
    bool Save(const TCHAR* FileName, const std::string& PathName) const;
    bool Save(const TCHAR* FullPath) const;

public:
    template<typename FrameType>
    const FrameType* GetFrameMetaData(int Index) const
    {
        return static_cast<const FrameType*>(GetFrameMetaData(Index));
    }

protected:
    virtual const void* GetFrameMetaData(int Index) const = 0;
    virtual bool Load(FILE* File) = 0;
    virtual bool Save(FILE* File) const = 0;

#ifdef _DEBUG
protected:
    virtual void SetFrameMetaData(int Index, const void* FrameData) = 0;
#endif // _DEBUG
};


