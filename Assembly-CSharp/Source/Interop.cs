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
        //[DllImport("__Internal", EntryPoint = "PrintCPPMsg")]
        public extern static string PrintCPPMsg();
    }
}
