using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Editor
{
    [ComVisible(true)]
    public interface IWindow
    {
        void Show();
        void Tite(string title);
    }
}
