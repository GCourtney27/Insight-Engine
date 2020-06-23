using Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InsightEngine
{
    class Rotator
    {
        float m_YRotationOffset;
        public float RotationOffset { get { return m_YRotationOffset; } }

        // Called when game started or when spawned
        void BeginPlay()
        {

        }

        // Called once per frame
        void Tick(double deltaMs)
        {
            //Transform.Rotate(0.0f, Math.Sin(1.0f * deltaMs), 0.0f); 
            //Interop.TestRotate(0.0f, (float)Math.Sin(1.0f * deltaMs), 0.0f);
            m_YRotationOffset = (float)Math.Sin(1.0f * deltaMs);
            
            //Console.WriteLine(m_YRotationOffset);
        }
    }
}
