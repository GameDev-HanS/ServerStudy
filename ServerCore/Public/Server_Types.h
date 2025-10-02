#pragma once

// Microsoft의 확장 기능
// 플랫폼에 종속적이지만, 명시적으로 타입의 크기를 확인 가능하다.

using Byte      = unsigned char;
using int8      = __int8;
using int16     = __int16;
using int32     = __int32;
using int64     = __int64;
using uint8     = unsigned __int8;
using uint16    = unsigned __int16;
using uint32    = unsigned __int32;
using uint64    = unsigned __int64;