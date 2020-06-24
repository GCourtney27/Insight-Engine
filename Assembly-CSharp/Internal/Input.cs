using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Internal
{
    class Input
    {
        // Keys
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool IsKeyPressed(char KeyCode);

        // Mouse
        public enum eMouseButton : int
        { 
            IE_MOUSEBUTTON_LEFT = 0,
            IE_MOUSEBUTTON_RIGHT = 1,
            IE_MOUSEBUTTON_MIDDLE = 2
        }
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool IsMouseButtonPressed(eMouseButton MouseButton);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static int GetMouseX();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static int GetMouseY();
    }
}
