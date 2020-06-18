using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Source
{

    class Test
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static string PrintCPPMsg();

        Test() 
        {
            Console.WriteLine("Test class created");
        }

        int GetNumber()
        {
            return 07271999;
        }

        void ChangeNumber(int num)
        {
            num = 4;
            Console.WriteLine("C# number {0}", num);
        }

        void Method()
        {
            try
            {
                string msg = PrintCPPMsg();
                Console.WriteLine(msg);
            }catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
