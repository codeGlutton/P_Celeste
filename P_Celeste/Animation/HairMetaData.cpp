#include "Pch.h"
#include "HairMetaData.h"

bool CHairMetaData::Load(FILE* File)
{
	std::size_t Size = 0;
	fread(&Size, sizeof(size_t), 1, File);

	mFrameDatas.clear();
	mFrameDatas.resize(Size);

	for (size_t i = 0; i < Size; ++i)
	{
		fread(&mFrameDatas[i].mBackHairOffset, sizeof(FVector2), 1, File);
		fread(&mFrameDatas[i].mFrontHairOffset, sizeof(FVector2), 1, File);
		fread(&mFrameDatas[i].mIsFlip, sizeof(bool), 1, File);
	}

    return true;
}

bool CHairMetaData::Save(FILE* File) const
{
	std::size_t Size = mFrameDatas.size();
	fwrite(&Size, sizeof(size_t), 1, File);

	for (size_t i = 0; i < Size; ++i)
	{
		fwrite(&mFrameDatas[i].mBackHairOffset, sizeof(FVector2), 1, File);
		fwrite(&mFrameDatas[i].mFrontHairOffset, sizeof(FVector2), 1, File);
		fwrite(&mFrameDatas[i].mIsFlip, sizeof(bool), 1, File);
	}

    return true;
}