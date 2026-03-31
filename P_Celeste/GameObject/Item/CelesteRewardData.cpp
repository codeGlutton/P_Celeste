#include "Pch.h"
#include "CelesteRewardData.h"

void CCelesteRewardData::Save(FILE* File) const
{
	int32 Count = mStrawberryCollection.size();
	fwrite(&Count, sizeof(int32), 1, File);

	for (const int32& StrawberryIdx : mStrawberryCollection)
	{
		fwrite(&StrawberryIdx, sizeof(int32), 1, File);
	}
}

void CCelesteRewardData::Load(FILE* File)
{
	int32 Count;
	fread(&Count, sizeof(int32), 1, File);

	mStrawberryCollection.reserve(Count);
	for (int32 i = 0; i < Count; ++i)
	{
		int32 StrawberryIdx;
		fread(&StrawberryIdx, sizeof(int32), 1, File);
		mStrawberryCollection.insert(StrawberryIdx);
	}
}
