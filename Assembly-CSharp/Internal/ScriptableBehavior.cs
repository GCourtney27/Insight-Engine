using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Internal
{
    class ScriptableBehavior
    {
       public ScriptableBehavior()
        {
            m_Transform = new Transform();
        }
        Transform m_Transform;
        public Transform Transform { get { return m_Transform; } set { m_Transform = value; } }
        
    }
}
