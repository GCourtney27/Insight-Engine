using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Editor
{
    [ComVisible(true)]
    [ClassInterface(ClassInterfaceType.None)]

    public class Window : IWindow
    {
        Form window = new Form();
        public void Show()
        {
            window.ShowDialog();
        }

        public void Tite(string title)
        {
            window.Text = title;
        }
    }
}
