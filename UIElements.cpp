#include "UIElements.h"


Button makeButton(
    int x, int y,
    const std::wstring& label,
    int width)
{

    int totalInner = width - 2;
    int textLen = static_cast<int>(label.size());
    int emptySpace = (totalInner - textLen) / 2;

    int extraRight = (totalInner - textLen) % 2;

    std::wstring top = L"┌" + std::wstring(width - 2, L'─') + L"┐";
    std::wstring inner = L"│" + std::wstring(emptySpace, L' ') + label + std::wstring(emptySpace + extraRight, L' ') + L"│";
    std::wstring bottom = L"└" + std::wstring(width - 2, L'─') + L"┘";


    std::wstring topHover = L"╔" + std::wstring(width - 2, L'═') + L"╗";
    std::wstring innerHover = L"║" + std::wstring(emptySpace, L' ') + label + std::wstring(emptySpace + extraRight, L' ') + L"║";
    std::wstring bottomHover = L"╚" + std::wstring(width - 2, L'═') + L"╝";


    std::vector<std::wstring> normal = { top, inner, bottom };
    std::vector<std::wstring> hover = { topHover, innerHover, bottomHover };

    return Button(x, y, normal, hover, hover);
}


Button Start = makeButton(47, 17, L"Start", 31);
Button Options = makeButton(47, 20, L"Options (Coming Soon)", 31);
Button Exit = makeButton(47, 23, L"Exit", 31);

Button BuildButton = makeButton(27, 45, L"─█", 4);
Button ClearButton = makeButton(31, 45, L"/♣", 4);
Button PlantButton = makeButton(35, 45, L".W", 4);

Button BuildStockpile = makeButton(39, 45, L"==", 4);
