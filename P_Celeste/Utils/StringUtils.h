#pragma once

class StringUtils
{
    static const unsigned char UTF8_BOM[];

public:
    static bool HasUtf8Bom(const std::string& Str)
    {
        return Str.size() >= 3 &&
            (unsigned char)Str[0] == UTF8_BOM[0] &&
            (unsigned char)Str[1] == UTF8_BOM[1] &&
            (unsigned char)Str[2] == UTF8_BOM[2];
    }

    static std::wstring ConvertUtf8ToWString(const std::string& Utf8Str)
    {
        const char* Data = Utf8Str.data();
        int Ut8StrSize = static_cast<int>(Utf8Str.size());

        // BOM 제거
        if (HasUtf8Bom(Utf8Str) == true)
        {
            Data += 3;
            Ut8StrSize -= 3;
        }
        if (Ut8StrSize <= 0)
        {
            return std::wstring();
        }

        int WStrSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, Data, Ut8StrSize, nullptr, 0);
        if (WStrSize <= 0)
        {
            return std::wstring();
        }
        std::wstring WStr(WStrSize, L'\0');
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, Data, Ut8StrSize, &WStr[0], WStrSize);

        return WStr;
    }

    static std::string ConvertWStringToUtf8(const std::wstring& WStr, bool WithBom = false)
    {
        std::string Utf8Str;

        if (WStr.empty() == true)
        {
            return Utf8Str;
        }

        int Utf8Size = WideCharToMultiByte(CP_UTF8, 0, WStr.data(), static_cast<int>(WStr.size()), nullptr, 0, nullptr, nullptr);
        if (Utf8Size <= 0)
        {
            return Utf8Str;
        }

        // BOM 삽입
        if (WithBom == true)
        {
            Utf8Str.append(reinterpret_cast<const char*>(UTF8_BOM), 3);
        }
        size_t Offset = Utf8Str.size();
        Utf8Str.resize(Offset + Utf8Size);

        WideCharToMultiByte(CP_UTF8, 0, WStr.data(), static_cast<int>(WStr.size()), &Utf8Str[0] + Offset, Utf8Size, nullptr, nullptr);

        return Utf8Str;
    }
};

