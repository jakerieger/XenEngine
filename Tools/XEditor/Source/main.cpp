// Author: Jake Rieger
// Created: 11/9/2024.
//

#include "EditorWindow.hpp"
#include "XenEngine.hpp"

using namespace XEditor;
using namespace Xen;

int main() {
    const EditorWindow editor(1300, 760);
    editor.Run();
    return 0;
}