using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;

namespace Internal
{
    class Vector3
    {
        float m_x;
        float m_y;
        float m_z;
        public float X { get { return m_x; } set { m_x = value; } }
        public float Y { get { return m_y; } set { m_y = value; } }
        public float Z { get { return m_z; } set { m_z = value; } }
    }
}
