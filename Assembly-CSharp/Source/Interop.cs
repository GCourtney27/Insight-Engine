using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Source
{
    class Interop
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void PrintCPPMsg(string msg);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetPosition(float x, float y, float z);
    }
}
