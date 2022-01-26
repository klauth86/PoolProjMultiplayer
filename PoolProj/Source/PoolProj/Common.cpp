#include "Common.h"

FString PPTags::Player1Portal = "P1";
FString PPTags::Player2Portal = "P2";

const FString& PPTags::GetPlayerTag(int32 i) { return i == 0 ? Player1Portal : Player2Portal; }
