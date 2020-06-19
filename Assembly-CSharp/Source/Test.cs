using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Source
{

    class Test
    {

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
            //Interop.PrintCPPMsg("Hello From C#");
            Interop.SetPosition(1.0f, 2.0f, 3.0f);
        }
    }
}
